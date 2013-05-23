
/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2006 Duncan Mac-Vicar <duncan@kde.org>
 * Copyright (C) 2007,2011 Sebastian Trueg <trueg@kde.org>
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

#include "util/simplestatementiterator.h"
#include "statement.h"
#include "locator.h"
#include "error.h"

#include <QtCore/QUrl>
#include <QtCore/QFile>
#include <QtCore/QtPlugin>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>
#include <QtCore/QLatin1String>
#include <QtCore/QMutexLocker>


Q_EXPORT_PLUGIN2(soprano_raptorparser, Soprano::Raptor::Parser)

namespace {
    void raptorLogHandler(void *userData,raptor_log_message *message)
    {
        Soprano::Raptor::Parser* p = static_cast<Soprano::Raptor::Parser*>( userData );
        if ( message->locator ) {
            p->setError( Soprano::Error::ParserError( Soprano::Error::Locator( message->locator->line, message->locator->column, message->locator->byte ),
                                                      QString::fromUtf8( message->text ),
                                                      Soprano::Error::ErrorParsingFailed ) );
        }
        else {
            p->setError( Soprano::Error::Error( QString::fromUtf8( message->text ), Soprano::Error::ErrorUnknown ) );
        }
    }

    Soprano::Node convertNode( raptor_term * term )
    {
        if(!term) {
            return Soprano::Node();
        }

        switch( term->type ) {
        case RAPTOR_TERM_TYPE_URI: {
            return Soprano::Node::createResourceNode(
                        QString::fromUtf8( ( char* )raptor_uri_as_string( term->value.uri ) ) );
        }

        case RAPTOR_TERM_TYPE_BLANK: {
            return Soprano::Node::createBlankNode(
                        QString::fromUtf8( ( const char* )(term->value.blank.string) ) );
        }

        case RAPTOR_TERM_TYPE_LITERAL: {
            if ( term->value.literal.datatype ) {
                return Soprano::Node::createLiteralNode(
                            Soprano::LiteralValue::fromString(
                                QString::fromUtf8( ( const char* )term->value.literal.string ),
                                QString::fromUtf8(
                                    ( char* )raptor_uri_as_string(term->value.literal.datatype ) )
                                )
                            );
            }
            else {
                return Soprano::Node::createLiteralNode(
                            Soprano::LiteralValue::createPlainLiteral(
                                QString::fromUtf8( ( const char* )term->value.literal.string ),
                                QString::fromUtf8( ( const char* )term->value.literal.language ) ) );
            }
        }

        default:
            return Soprano::Node();
        }

        // make gcc shut up
        return Soprano::Node();
    }

    Soprano::Statement convertTriple( const raptor_statement* triple )
    {
        return Soprano::Statement( convertNode( triple->subject ),
                                   convertNode( triple->predicate ),
                                   convertNode( triple->object ),
                                   convertNode( triple->graph ) );
    }


    class ParserData {
    public:
        QList<Soprano::Statement> statements;
        Soprano::Node currentContext;
    };


    void raptorTriplesHandler( void* userData, raptor_statement* triple )
    {
        Q_ASSERT(userData);
        ParserData* pd = static_cast<ParserData*>( userData );
        pd->statements.append( convertTriple( triple ) );
    }
}


class Soprano::Raptor::Parser::Private
{
public:
    raptor_world * world;
    mutable QMutex mutex;
};


Soprano::Raptor::Parser::Parser()
    : QObject(),
      Soprano::Parser( "raptor" ),
      d(new Private())
{
    d->world = raptor_new_world();
    raptor_world_open(d->world);
    Q_ASSERT(d->world);
}


Soprano::Raptor::Parser::~Parser()
{
    raptor_free_world(d->world);
    delete d;
}


Soprano::RdfSerializations Soprano::Raptor::Parser::supportedSerializations() const
{
    return( SerializationRdfXml
            |SerializationNTriples
            |SerializationTurtle
            |SerializationTrig
            );
}


raptor_parser* Soprano::Raptor::Parser::createParser( RdfSerialization serialization,
                                                      const QString& userSerialization ) const
{
    // create the parser
    const QString mimeType = Soprano::serializationMimeType( serialization, userSerialization );
    raptor_parser* parser = 0;
    if ( serialization == Soprano::SerializationNTriples ) {
        parser = raptor_new_parser(d->world,"ntriples"); // mimetype for ntriple is text/plain which is useless for the method below
    }
    else {
        parser = raptor_new_parser_for_content( d->world,
                                                0,
                                                mimeType.toLatin1().data(),
                                                0,
                                                0,
                                                0 );
    }

    if ( !parser ) {
        qDebug() << "(Soprano::Raptor::Parser) no parser for serialization " << Soprano::serializationMimeType( serialization, userSerialization );
        setError( QString( "Failed to create parser for serialization %1" ).arg( Soprano::serializationMimeType( serialization, userSerialization ) ) );
        return 0;
    }

    // set the error handling method
    Parser* that = const_cast<Parser*>( this );
    raptor_world_set_log_handler(d->world, that, raptorLogHandler);

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


Soprano::StatementIterator Soprano::Raptor::Parser::parseStream( QTextStream& stream,
                                                                 const QUrl& baseUri,
                                                                 RdfSerialization serialization,
                                                                 const QString& userSerialization ) const
{
    QMutexLocker lock( &d->mutex );

    clearError();

    raptor_parser* parser = createParser( serialization, userSerialization );
    if ( !parser ) {
        return StatementIterator();
    }

    // prepare the container for the parsed data
    ParserData data;
    raptor_parser_set_statement_handler( parser, &data, raptorTriplesHandler );

    // start the atual parsing
    raptor_uri* raptorBaseUri = 0;
    if ( baseUri.isValid() ) {
        raptorBaseUri = raptor_new_uri( d->world,(unsigned char *) baseUri.toString().toUtf8().data() );
    }
    else {
        raptorBaseUri = raptor_new_uri( d->world, (unsigned char *) "http://soprano.sourceforge.net/dummyBaseUri" );
    }

    clearError();
    if ( raptor_parser_parse_start( parser, raptorBaseUri ) != 0 ) {
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
                 raptor_parser_parse_chunk( parser, ( const unsigned char* )buf.data(), r, 0 ) ) {
                // parse_chunck return failure code.
                // Call it with END=true and then free
                raptor_parser_parse_chunk(parser,0,0,/*END=*/1);
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
            if ( raptor_parser_parse_chunk( parser, ( const unsigned char* )utf8Data.data(), utf8Data.length(), 0 ) ) {
                // parse_chunck return failure code.
                // Call it with END=true and then free
                raptor_parser_parse_chunk(parser,0,0,/*END=*/1);
                raptor_free_parser( parser );
                if ( raptorBaseUri ) {
                    raptor_free_uri( raptorBaseUri );
                }
                return StatementIterator();
            }
        }
    }
    // Call parse_chunk with END=true
    raptor_parser_parse_chunk( parser, 0, 0, 1 );

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

