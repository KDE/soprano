
/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2006 Duncan Mac-Vicar <duncan@kde.org>
 * Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "raptorparser.h"
#include "raptor-config.h"

#include "util/simplestatementiterator.h"
#include "statement.h"
#include "locator.h"
#include "error.h"

#include <raptor.h>

#include <QtCore/QUrl>
#include <QtCore/QFile>
#include <QtCore/QtPlugin>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>
#include <QtCore/QLatin1String>
#include <QtCore/QMutexLocker>


namespace {
    class RaptorInitHelper
    {
    public:
        RaptorInitHelper() {
            raptor_init();
        }
        ~RaptorInitHelper() {
            raptor_finish();
        }
    };
}

Q_EXPORT_PLUGIN2(soprano_raptorparser, Soprano::Raptor::Parser)

namespace {
    // for some strange reason raptor <= 1.4.15 can only handle application/turtle when parsing
    // and application/x-turtle when serializing, but not the other way around
    QString mimeTypeString( Soprano::RdfSerialization s, const QString& userSerialization )
    {
#ifndef RAPTOR_HAVE_TRIG
        if ( s == Soprano::SerializationTurtle ) {
            return "application/turtle"; // x-turtle does not work....
        }
        else
#endif
        {
            return serializationMimeType( s, userSerialization );
        }
    }


    void raptorMessageHandler( void* userData, raptor_locator* locator, const char* message )
    {
        Soprano::Raptor::Parser* p = static_cast<Soprano::Raptor::Parser*>( userData );
        if ( locator ) {
            p->setError( Soprano::Error::ParserError( Soprano::Error::Locator( locator->line, locator->column, locator->byte ),
                                                      QString::fromUtf8( message ),
                                                      Soprano::Error::ErrorParsingFailed ) );
        }
        else {
            p->setError( Soprano::Error::Error( QString::fromUtf8( message ), Soprano::Error::ErrorUnknown ) );
        }
    }


    Soprano::Node convertNode( const void* data, raptor_identifier_type type,
                               raptor_uri* objectLiteralDatatype = 0, const unsigned char* objectLiteralLanguage = 0 )
    {
        switch( type ) {
        case RAPTOR_IDENTIFIER_TYPE_RESOURCE:
        case RAPTOR_IDENTIFIER_TYPE_PREDICATE:
        case RAPTOR_IDENTIFIER_TYPE_ORDINAL:
            return Soprano::Node::createResourceNode( QString::fromUtf8( ( char* )raptor_uri_as_string( ( raptor_uri* )data ) ) );

        case RAPTOR_IDENTIFIER_TYPE_ANONYMOUS:
            return Soprano::Node::createBlankNode( QString::fromUtf8( ( const char* )data ) );

        case RAPTOR_IDENTIFIER_TYPE_LITERAL:
        case RAPTOR_IDENTIFIER_TYPE_XML_LITERAL:
            if ( objectLiteralDatatype ) {
                return Soprano::Node::createLiteralNode( Soprano::LiteralValue::fromString( QString::fromUtf8( ( const char* )data ),
                                                                                            QString::fromUtf8( ( char* )raptor_uri_as_string( objectLiteralDatatype ) ) ) );
            }
            else {
                return Soprano::Node::createLiteralNode( Soprano::LiteralValue::createPlainLiteral( QString::fromUtf8( ( const char* )data ),
                                                                                                    QString::fromUtf8( ( const char* )objectLiteralLanguage ) ) );
            }

        default:
            return Soprano::Node();
        }
    }


    Soprano::Statement convertTriple( const raptor_statement* triple )
    {
        return Soprano::Statement( convertNode( triple->subject, triple->subject_type ),
                                   convertNode( triple->predicate, triple->predicate_type ),
                                   convertNode( triple->object, triple->object_type,
                                                triple->object_literal_datatype,
                                                triple->object_literal_language ) );
    }


    class ParserData {
    public:
        QList<Soprano::Statement> statements;
        Soprano::Node currentContext;
    };


    void raptorTriplesHandler( void* userData, const raptor_statement* triple )
    {
        ParserData* pd = static_cast<ParserData*>( userData );
        Soprano::Statement s = convertTriple( triple );
//        qDebug() << "got triple: " << s;
        s.setContext( pd->currentContext );
        pd->statements.append( s );
    }


    void raptorGraphHandler( void* userData, raptor_uri* graph )
    {
        Soprano::Node context = Soprano::Node::createResourceNode( QString::fromUtf8( ( char* )raptor_uri_as_string( graph ) ) );
        ParserData* pd = static_cast<ParserData*>( userData );
        pd->currentContext = context;
//        qDebug() << "got graph: " << context;
    }
}


Soprano::Raptor::Parser::Parser()
    : QObject(),
      Soprano::Parser( "raptor" )
{
}


Soprano::Raptor::Parser::~Parser()
{
}


Soprano::RdfSerializations Soprano::Raptor::Parser::supportedSerializations() const
{
    return(  SerializationRdfXml
             |SerializationNTriples
             |SerializationTurtle
#ifdef RAPTOR_HAVE_TRIG
             |SerializationTrig
#endif
        );
}


raptor_parser* Soprano::Raptor::Parser::createParser( RdfSerialization serialization,
                                                      const QString& userSerialization ) const
{
    // create the parser
    QString mimeType = mimeTypeString( serialization, userSerialization );
    raptor_parser* parser = 0;
    if ( serialization == Soprano::SerializationNTriples ) {
        parser = raptor_new_parser( "ntriples" ); // mimetype for ntriple is text/plain which is useless for the method below
    }
    else {
        parser = raptor_new_parser_for_content( 0,
                                                mimeType.toLatin1().data(),
                                                0,
                                                0,
                                                0 );
    }

    if ( !parser ) {
        qDebug() << "(Soprano::Raptor::Parser) no parser for serialization " << mimeTypeString( serialization, userSerialization );
        setError( QString( "Failed to create parser for serialization %1" ).arg( mimeTypeString( serialization, userSerialization ) ) );
        return 0;
    }

    // set the erro handling method
    Parser* that = const_cast<Parser*>( this );
    raptor_set_fatal_error_handler( parser, that, raptorMessageHandler );
    raptor_set_error_handler( parser, that, raptorMessageHandler );
    raptor_set_warning_handler( parser, that, raptorMessageHandler );

    return parser;
}


Soprano::StatementIterator Soprano::Raptor::Parser::parseFile( const QString& filename,
                                                               const QUrl& baseUri,
                                                               RdfSerialization serialization,
                                                               const QString& userSerialization ) const
{
    QFile f( filename );
    if ( f.open( QIODevice::ReadOnly ) ) {
        QTextStream s( &f );
        return parseStream( s, baseUri, serialization, userSerialization );
    }
    else {
        setError( QString( "Could not open file %1 for reading." ).arg( filename ) );
        return StatementIterator();
    }

//     clearError();

//     raptor_parser* parser = createParser( serialization, userSerialization );
//     if ( !parser ) {
//         return StatementIterator();
//     }

//     // prepare the container for the parsed data
//     QList<Statement> statements;
//     raptor_set_statement_handler( parser, &statements, raptorTriplesHandler );

//     // start the atual parsing
//     QUrl uri( QUrl::fromLocalFile( filename ) );
//     if ( uri.scheme().isEmpty() ) {
//         // we need to help the stupid librdf file url handling
//         uri.setScheme("file");
//     }
//     raptor_uri* raptorBaseUri = 0;
//     if ( !baseUri.toString().isEmpty() ) {
//         raptorBaseUri = raptor_new_uri( (unsigned char *) baseUri.toString().toUtf8().data() );
//     }
//     raptor_uri* raptorUri = raptor_new_uri( (unsigned char *) uri.toString().toUtf8().data() );
//     if ( !raptorUri ) {
//         setError( QLatin1String( "Internal: Failed to create raptor_uri instance for '%1'" ).arg( uri ) );
//         return StatementIterator();
//     }

//     int r = raptor_parse_uri( parser, raptorUri, raptorBaseUri );

//     raptor_free_parser( parser );
//     raptor_free_uri( raptorUri );
//     if ( raptorBaseUri ) {
//         raptor_free_uri( raptorBaseUri );
//     }

//     if ( r == 0 ) {
//         return SimpleStatementIterator( statements );
//     }
//     else {
//         return StatementIterator();
//     }
}


Soprano::StatementIterator Soprano::Raptor::Parser::parseString( const QString& data,
                                                                 const QUrl& baseUri,
                                                                 RdfSerialization serialization,
                                                                 const QString& userSerialization ) const
{
    QString buf( data );
    QTextStream s( &buf );
    return parseStream( s, baseUri, serialization, userSerialization );
}


Soprano::StatementIterator
Soprano::Raptor::Parser::parseStream( QTextStream& stream,
                                      const QUrl& baseUri,
                                      RdfSerialization serialization,
                                      const QString& userSerialization ) const
{
    QMutexLocker lock( &m_mutex );
    RaptorInitHelper raptorInitHelper;

    clearError();

    raptor_parser* parser = createParser( serialization, userSerialization );
    if ( !parser ) {
        return StatementIterator();
    }

    // prepare the container for the parsed data
    ParserData data;
    raptor_set_statement_handler( parser, &data, raptorTriplesHandler );
#ifdef RAPTOR_HAVE_TRIG
    raptor_set_graph_handler( parser, &data, raptorGraphHandler );
#endif

    // start the atual parsing
    raptor_uri* raptorBaseUri = 0;
    if ( baseUri.isValid() ) {
        raptorBaseUri = raptor_new_uri( (unsigned char *) baseUri.toString().toUtf8().data() );
    }
    else {
        raptorBaseUri = raptor_new_uri( (unsigned char *) "http://soprano.sourceforge.net/dummyBaseUri" );
    }

    clearError();
    if ( raptor_start_parse( parser, raptorBaseUri ) ) {
        if ( !lastError() ) {
            ErrorCache::setError( QLatin1String( "Failed to start parsing." ) );
        }
        raptor_free_parser( parser );
        if ( raptorBaseUri ) {
            raptor_free_uri( raptorBaseUri );
        }
        return StatementIterator();
    }

    static const int bufSize = 1024;

    // if possible let raptor do the decoding
    if ( QIODevice* dev = stream.device() ) {
        QByteArray buf( bufSize, 0 );
        while ( !dev->atEnd() ) {
            qint64 r = dev->read( buf.data(), buf.size() );
            if ( r <= 0 ||
                 raptor_parse_chunk( parser, ( const unsigned char* )buf.data(), r, 0 ) ) {
                raptor_free_parser( parser );
                if ( raptorBaseUri ) {
                    raptor_free_uri( raptorBaseUri );
                }
                return StatementIterator();
            }
        }
    }
    else {
        while ( !stream.atEnd() ) {
            QString buf = stream.read( bufSize );
            QByteArray utf8Data = buf.toUtf8();
            if ( raptor_parse_chunk( parser, ( const unsigned char* )utf8Data.data(), utf8Data.length(), 0 ) ) {
                raptor_free_parser( parser );
                if ( raptorBaseUri ) {
                    raptor_free_uri( raptorBaseUri );
                }
                return StatementIterator();
            }
        }
    }
    raptor_parse_chunk( parser, 0, 0, 1 );

    raptor_free_parser( parser );
    if ( raptorBaseUri ) {
        raptor_free_uri( raptorBaseUri );
    }

    return Util::SimpleStatementIterator( data.statements );
}


void Soprano::Raptor::Parser::setError( const Soprano::Error::Error& error ) const
{
    ErrorCache::setError( error );
}

#include "raptorparser.moc"
