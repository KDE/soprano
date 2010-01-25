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

#include "statementiterator.h"
#include "statement.h"
#include "node.h"

#include <raptor.h>
#include <string.h>

#include <QtCore/QUrl>
#include <QtCore/QtPlugin>
#include <QtCore/QTextStream>
#include <QtCore/QStringList>
#include <QtCore/QDebug>

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

    bool convertNode( const Soprano::Node& node, const void** data, raptor_identifier_type* type, raptor_uri** dataType = 0, const unsigned char** lang = 0 )
    {
        if ( node.isResource() ) {
            *data = raptor_new_uri( ( const unsigned char* )node.uri().toEncoded().data() );
            *type = RAPTOR_IDENTIFIER_TYPE_RESOURCE;
            return true;
        }
        else if ( node.isBlank() ) {
            *data = strdup( node.identifier().toUtf8().data() );
            *type = RAPTOR_IDENTIFIER_TYPE_ANONYMOUS;
            return true;
        }
        else if ( node.isLiteral() ) {
            *data = strdup( node.identifier().toUtf8().data() );
            if ( node.literal().isPlain() ) {
                *lang = ( unsigned char* )strdup( ( const char* )node.language().toUtf8().data() );
            }
            else {
                *dataType = raptor_new_uri( ( const unsigned char* )node.dataType().toEncoded().data() );
            }
            *type = RAPTOR_IDENTIFIER_TYPE_LITERAL;
            return true;
        }

        return false;
    }


    raptor_statement* convertStatement( const Soprano::Statement& statement )
    {
        raptor_statement* s = new raptor_statement;
        memset( s, 0, sizeof( raptor_statement ) );
        convertNode( statement.subject(), &s->subject, &s->subject_type );
        convertNode( statement.predicate(), &s->predicate, &s->predicate_type );
        convertNode( statement.object(), &s->object, &s->object_type, &s->object_literal_datatype, &s->object_literal_language );
        return s;
    }


    void free_node( const void* data, raptor_identifier_type type )
    {
        switch( type ) {
        case RAPTOR_IDENTIFIER_TYPE_RESOURCE:
            raptor_free_uri( ( raptor_uri* )data );
            break;
        default:
            free( ( char* )data );
            break;
        }
    }

    void free_statement( raptor_statement* s )
    {
        free_node( s->subject, s->subject_type );
        free_node( s->predicate, s->predicate_type );
        free_node( s->object, s->object_type );
        if ( s->object_literal_datatype )
            raptor_free_uri( ( raptor_uri* )s->object_literal_datatype );
        if ( s->object_literal_language )
            free( ( char* )s->object_literal_language );
        delete s;
    }


    int raptorIOStreamWriteByte( void* data, const int byte )
    {
        QTextStream* s = reinterpret_cast<QTextStream*>( data );
        // an int is not a byte. Strange raptor API!
        if( s->device() ) {
            s->device()->putChar( (char)byte );
        }
        else {
            ( *s ) << ( char )byte;
        }
        return 0;
    }


    int raptorIOStreamWriteBytes( void* data, const void* ptr, size_t size, size_t nmemb )
    {
        // the raptor API is very weird. But it seems that ATM raptor only uses size == 1
        QTextStream* s = reinterpret_cast<QTextStream*>( data );
        switch( size ) {
        case 1: {
            const char* p = reinterpret_cast<const char*>( ptr );
            if( s->device() ) {
                s->device()->write( p, nmemb );
            }
            else {
                for ( unsigned int i = 0; i < nmemb; ++i ) {
                    raptorIOStreamWriteByte( data, p[i] );
                }
            }
            break;
        }
        default:
            qDebug() << "Unsupported data size: " << size;
            return -1;
        }
        return 0;
    }
}

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


bool Soprano::Raptor::Serializer::serialize( StatementIterator it,
                                             QTextStream& stream,
                                             RdfSerialization serialization,
                                             const QString& userSerialization ) const
{
    clearError();

    RaptorInitHelper raptorHelper;

    raptor_serializer* serializer = 0;
    if ( serialization == SerializationRdfXml ) {
        serializer = raptor_new_serializer( "rdfxml-abbrev" ); // we always want the abbreviated xmlrdf
    }
    else {
        for ( int i = 0; 1; ++i ) {
            const char* syntax_name = 0;
            const char* syntax_label = 0;
            const char* mime_type = 0;
            const unsigned char* uri_string = 0;
            if ( raptor_serializers_enumerate( i,
                                               &syntax_name,
                                               &syntax_label,
                                               &mime_type,
                                               &uri_string ) )
                break;
            if ( !qstrcmp( serializationMimeType( serialization, userSerialization ).toLatin1().data(), mime_type ) ) {
                serializer = raptor_new_serializer( syntax_name );
                break;
            }
        }
    }

    if ( !serializer ) {
        qDebug() << "(Soprano::Raptor::Serializer) no serializer for mimetype " << serializationMimeType( serialization, userSerialization );
        return false;
    }

    // add prefixes
    QHash<QString, QUrl> namespaces = prefixes();
    for ( QHash<QString, QUrl>::const_iterator pfit = namespaces.constBegin();
          pfit != namespaces.constEnd(); ++pfit ) {
        raptor_uri* ns = raptor_new_uri( reinterpret_cast<unsigned char*>( pfit.value().toEncoded().data() ) );
        raptor_serialize_set_namespace( serializer,
                                        ns,
                                        ( unsigned char* )pfit.key().toLatin1().data() );
        raptor_free_uri( ns );
    }

    bool success = true;

#ifdef HAVE_IOSTREAM_HANDLER2
    raptor_iostream_handler2 raptorStreamHandler = {
        2,
        0,
        0,
        raptorIOStreamWriteByte,
        raptorIOStreamWriteBytes,
        0,
        0,
        0
    };
    raptor_iostream* raptorStream = raptor_new_iostream_from_handler2( &stream,
                                                                       &raptorStreamHandler );
#else
    raptor_iostream_handler raptorStreamHandler = {
        0,
        0,
        raptorIOStreamWriteByte,
        raptorIOStreamWriteBytes,
        0
    };
    raptor_iostream* raptorStream = raptor_new_iostream_from_handler( &stream,
                                                                      &raptorStreamHandler );
#endif

    raptor_serialize_start_to_iostream( serializer, 0, raptorStream );

    while ( it.next() ) {
        raptor_statement* rs = convertStatement( *it );
        raptor_serialize_statement( serializer, rs );
        free_statement( rs );
    }

    raptor_serialize_end( serializer );
    raptor_free_iostream( raptorStream );
    raptor_free_serializer( serializer );

    return success;
}

#include "raptorserializer.moc"
