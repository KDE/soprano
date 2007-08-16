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
#include "../../backends/redland/redlandworld.h"
#include "../../backends/redland/redlandstatementiterator.h"
#include "../../backends/redland/redlandutil.h"

#include <soprano/statementiterator.h>
#include <soprano/statement.h>

#include <redland.h>

#include <QtCore/QUrl>
#include <QtCore/QtPlugin>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>


Q_EXPORT_PLUGIN2(soprano_raptorparser, Soprano::Raptor::Parser)

// for some strange reason librdf can only handle application/turtle when parsing and application/x-turtle when serializing, but not the other way around
static QString mimeTypeString( Soprano::RdfSerialization s )
{
    if ( s == Soprano::SERIALIZATION_TURTLE ) {
        return "application/turtle"; // x-turtle does not work....
    }
    else {
        return serializationMimeType( s );
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
    // FIXME: test for XML/RDF fails
//    return SERIALIZATION_RDF_XML|SERIALIZATION_N_TRIPLES|
    return SERIALIZATION_TURTLE;
}


Soprano::StatementIterator Soprano::Raptor::Parser::parseFile( const QString& filename,
                                                               const QUrl& baseUri,
                                                               RdfSerialization serialization,
                                                               const QString& userSerialization ) const
{
    QUrl uri( QUrl::fromLocalFile( filename ) );
    if ( uri.scheme().isEmpty() ) {
        // we need to help the stupid librdf file url handling
        uri.setScheme("file");
    }

    librdf_uri* redlandUri = librdf_new_uri( Redland::World::self()->worldPtr(),
                                             (unsigned char *) uri.toString().toLatin1().data() );
    if ( !redlandUri ) {
        return StatementIterator();
    }

    librdf_parser *parser = librdf_new_parser( Redland::World::self()->worldPtr(),
                                               0, // use all factories
                                               mimeTypeString( serialization ).toLatin1().data(),
                                               0 ); // what is the URI of the syntax used for?
    if ( !parser ) {
        qDebug() << "(Soprano::Raptor::Parser) no parser for serialization " << serializationMimeType( serialization );
        librdf_free_uri( redlandUri );
        return StatementIterator();
    }

    librdf_uri* redlandBaseUri = 0;
    if ( !baseUri.toString().isEmpty() ) {
        redlandBaseUri = librdf_new_uri( Redland::World::self()->worldPtr(),
                                         (unsigned char *) baseUri.toString().toLatin1().data() );
    }

    librdf_stream* stream = librdf_parser_parse_as_stream( parser, redlandUri, redlandBaseUri );

    librdf_free_uri( redlandUri );
    librdf_free_parser( parser );

    if ( !stream ) {
        return StatementIterator();
    }
    else {
        return new Redland::RedlandStatementIterator( 0, stream, Node() );
    }
}


Soprano::StatementIterator Soprano::Raptor::Parser::parseString( const QString& data,
                                                                 const QUrl& baseUri,
                                                                 RdfSerialization serialization,
                                                                 const QString& userSerialization ) const
{
    librdf_parser* parser = librdf_new_parser( Redland::World::self()->worldPtr(),
                                               0, // use all factories
                                               mimeTypeString( serialization ).toLatin1().data(),
                                               0 ); // what is the URI of the syntax used for?
    if ( !parser ) {
        return StatementIterator();
    }

    librdf_uri* redlandBaseUri = 0;
    if ( !baseUri.toString().isEmpty() ) {
        redlandBaseUri = librdf_new_uri( Redland::World::self()->worldPtr(),
                                         (unsigned char *) baseUri.toString().toLatin1().data() );
    }

    // FIXME: do we need to convert the string data into something else than UTF8 with some serialization
    librdf_stream* stream = librdf_parser_parse_string_as_stream( parser,
                                                                  ( const unsigned char* )data.toUtf8().data(),
                                                                  redlandBaseUri );
    librdf_free_parser( parser );

    if ( !stream ) {
        return StatementIterator();
    }
    else {
        return new Redland::RedlandStatementIterator( 0, stream, Node() );
    }
}


Soprano::StatementIterator
Soprano::Raptor::Parser::parseStream( QTextStream* stream,
                                      const QUrl& baseUri,
                                      RdfSerialization serialization,
                                      const QString& userSerialization ) const
{
    return parseString( stream->readAll(), baseUri, serialization );
}


int raptorIOStreamWriteByte( void* data, const int byte )
{
    QTextStream* s = reinterpret_cast<QTextStream*>( data );
    // an int is not a byte. Strange raptor API!
    ( *s ) << ( char )byte;
    return 0;
}


int raptorIOStreamWriteBytes( void* data, const void* ptr, size_t size, size_t nmemb )
{
    // the raptor API is very weird. But it seems that ATM raptor only uses size == 1
    QTextStream* s = reinterpret_cast<QTextStream*>( data );
    switch( size ) {
    case 1: {
        const char* p = reinterpret_cast<const char*>( ptr );
        for ( int i = 0; i < nmemb; ++i ) {
            ( *s ) << p[i];
        }
        break;
    }
    default:
        qDebug() << "Unsupported data size: " << size;
        return -1;
    }
    return 0;
}


class StreamData {
public:
    Soprano::StatementIterator it;
    bool initialized;
    bool atEnd;
};

// the raptor API is aweful: it seems that first atEnd is called, then get, and then next until next returns false.
// So we have to call it.next() manually if we don't want to get the first statement twice
int streamIsEnd( void* data )
{
    StreamData* it = reinterpret_cast<StreamData*>( data );
    if ( !it->initialized ) {
        it->initialized = true;
        it->atEnd = !it->it.next();
    }
    return it->atEnd;
}


int streamNext( void* data )
{
    StreamData* it = reinterpret_cast<StreamData*>( data );
    it->atEnd = !it->it.next();
    return it->atEnd;
}


void* streamGet( void* data, int what )
{
    StreamData* it = reinterpret_cast<StreamData*>( data );

    if ( what == 0 ) {
        // statement (stupid librdf does not export it)
        return Soprano::Redland::Util::createStatement( it->it.current() );
    }
    else {
        // context
        return Soprano::Redland::Util::createNode( it->it.current().context() );
    }
}


void streamFinished( void* )
{}


bool Soprano::Raptor::Parser::serialize( StatementIterator it,
                                         QTextStream* stream,
                                         RdfSerialization serialization,
                                         const QString& userSerialization ) const
{
    librdf_serializer* serializer = librdf_new_serializer( Redland::World::self()->worldPtr(),
                                                           0, // all factories
                                                           serializationMimeType( serialization ).toLatin1().data(),
                                                           0 );
    if ( !serializer ) {
        qDebug() << "(Soprano::Raptor::Parser) no serializer for mimetype " << mimeTypeString( serialization );
        return false;
    }

    bool success = true;

    raptor_iostream_handler raptorStreamHandler = {
        0,
        0,
        raptorIOStreamWriteByte,
        raptorIOStreamWriteBytes,
        0
    };
    raptor_iostream* raptorStream = raptor_new_iostream_from_handler( stream,
                                                                      &raptorStreamHandler );

    if ( !raptorStream ) {
        qDebug() << "(Soprano::Raptor::Parser) failed to create Raptor stream.";
        librdf_free_serializer( serializer );
        return false;
    }

    StreamData streamData;
    streamData.it = it;
    streamData.atEnd = false;
    streamData.initialized = false;
    librdf_stream* rdfStream = librdf_new_stream( Redland::World::self()->worldPtr(),
                                                  &streamData,
                                                  streamIsEnd,
                                                  streamNext,
                                                  streamGet,
                                                  streamFinished );

    if ( !rdfStream ) {
        qDebug() << "(Soprano::Raptor::Parser) failed to create librdf stream.";
        raptor_free_iostream( raptorStream );
        return false;
    }

    if ( librdf_serializer_serialize_stream_to_iostream( serializer,
                                                         0,
                                                         rdfStream,
                                                         raptorStream ) ) {
        qDebug() << "(Soprano::Raptor::Parser) serialization failed.";
        success = false;
    }

//     librdf_free_stream( rdfStream );
//     raptor_free_iostream( raptorStream );
    librdf_free_serializer( serializer );

    return success;
}

#include "raptorparser.moc"
