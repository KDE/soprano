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

#include "raptorserializer.h"
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


Q_EXPORT_PLUGIN2(soprano_raptorserializer, Soprano::Raptor::Serializer)


Soprano::Raptor::Serializer::Serializer()
    : QObject(),
      Soprano::Serializer( "raptor" )
{
}


Soprano::Raptor::Serializer::~Serializer()
{
}


Soprano::RdfSerializations Soprano::Raptor::Serializer::supportedSerializations() const
{
    return SerializationRdfXml|SerializationNTriples|SerializationTurtle|SerializationUser;
}


QStringList Soprano::Raptor::Serializer::supportedUserSerializations() const
{
    QStringList sl;
    int i = 0;
    const char* name = 0;
    const char* label = 0;
    const char* mimeType = 0;
    const unsigned char* uri = 0;
    while ( !raptor_serializers_enumerate( i,
                                           &name,
                                           &label,
                                           &mimeType,
                                           &uri ) ) {
        sl << QString::fromUtf8( name );
        ++i;
    }
    return sl;
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
        for ( unsigned int i = 0; i < nmemb; ++i ) {
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


bool Soprano::Raptor::Serializer::serialize( StatementIterator it,
                                             QTextStream& stream,
                                             RdfSerialization serialization,
                                             const QString& userSerialization ) const
{
    clearError();

    librdf_serializer* serializer = 0;
    if ( serialization == SerializationRdfXml ) {
        serializer = librdf_new_serializer( Redland::World::self()->worldPtr(),
                                            "rdfxml",
                                            0,
                                            0 );
    }
    else if ( serialization == SerializationUser ) {
        serializer = librdf_new_serializer( Redland::World::self()->worldPtr(),
                                            userSerialization.toLatin1().data(),
                                            0,
                                            0 );
    }
    else {
        serializer = librdf_new_serializer( Redland::World::self()->worldPtr(),
                                            0, // all factories
                                            serializationMimeType( serialization, userSerialization ).toLatin1().data(),
                                            0 );
    }

    if ( !serializer ) {
        qDebug() << "(Soprano::Raptor::Serializer) no serializer for mimetype " << serializationMimeType( serialization, userSerialization );
        setError( Redland::World::self()->lastError() );
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
    raptor_iostream* raptorStream = raptor_new_iostream_from_handler( &stream,
                                                                      &raptorStreamHandler );

    if ( !raptorStream ) {
        qDebug() << "(Soprano::Raptor::Serializer) failed to create Raptor stream.";
        librdf_free_serializer( serializer );
        setError( Redland::World::self()->lastError() );
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
        qDebug() << "(Soprano::Raptor::Serializer) failed to create librdf stream.";
        raptor_free_iostream( raptorStream );
        setError( Redland::World::self()->lastError() );
        return false;
    }

    librdf_uri* baseUri = librdf_new_uri( Redland::World::self()->worldPtr(),
                                          ( const unsigned char* )"http://soprano.org/FIXME/WeNeedABaseUriParameter" );

    if ( librdf_serializer_serialize_stream_to_iostream( serializer,
                                                         baseUri,
                                                         rdfStream,
                                                         raptorStream ) ) {
        qDebug() << "(Soprano::Raptor::Serializer) serialization failed.";
        setError( Redland::World::self()->lastError() );
        success = false;
    }

    librdf_free_uri( baseUri );
    librdf_free_stream( rdfStream );
    librdf_free_serializer( serializer );

    return success;
}

#include "raptorserializer.moc"
