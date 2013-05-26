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

#include <QtCore/QUrl>
#include <QtCore/QtPlugin>
#include <QtCore/QTextStream>
#include <QtCore/QStringList>
#include <QtCore/QDebug>

namespace {
    class RaptorInitHelper
    {
    public:
        raptor_world* world;

        RaptorInitHelper() {
            world = raptor_new_world();
            raptor_world_open(world);
        }
        ~RaptorInitHelper() {
            raptor_free_world(world);
        }
    };

    /* Probably unnecessary for serializer*/
    /*
    QString mimeTypeString( Soprano::RdfSerialization s, const QString& userSerialization )
    {
        if ( s == Soprano::SerializationTurtle ) {
            return "application/turtle"; // x-turtle does not work....
        }
        else {
            return serializationMimeType( s, userSerialization );
        }
    }*/

    raptor_term*  convertNode( raptor_world * world, const Soprano::Node& node)
    {
        raptor_term* term = 0;

        // According to documentation, raptor_new_term family creates deep copies of
        // all given input parameters
        if ( node.isResource() ) {
            raptor_uri* uri  = raptor_new_uri(world, ( const unsigned char* )node.uri().toEncoded().data() );
            term = raptor_new_term_from_uri(world, uri);
            raptor_free_uri(uri);
        }
        else if ( node.isBlank() ) {
            term = raptor_new_term_from_blank(world, (const unsigned char*)node.identifier().toUtf8().data() );
        }
        else if ( node.isLiteral() ) {
            QByteArray lang;
            raptor_uri* datatype = 0;

            if ( node.literal().isPlain() ) {
                lang = node.language().toUtf8();
            }
            else {
                datatype = raptor_new_uri( world, ( const unsigned char* )node.dataType().toEncoded().data() );
            }

            term = raptor_new_term_from_literal(world,
                                                reinterpret_cast<const unsigned char*>(node.literal().toString().toUtf8().constData()),
                                                datatype,
                                                reinterpret_cast<const unsigned char*>(lang.constData()));

            if( datatype ) {
                raptor_free_uri( datatype );
            }
        }

        return term;
    }


    raptor_statement* convertStatement(raptor_world * world, const Soprano::Statement& statement )
    {
        // Get terms
        raptor_term * subject_term = 0, *object_term = 0, * predicate_term = 0;
        raptor_term * graph_term = 0;

        subject_term = convertNode(world,statement.subject());
        if ( !subject_term) {
            qDebug() << "Failed to convert subject to raptor_term";
            return 0;
        }

        predicate_term = convertNode(world,statement.predicate());
        if (!predicate_term) {
            qDebug() << "Failed to convert predicate to raptor_term";
            raptor_free_term(subject_term);
            return 0;
        }

        object_term = convertNode(world,statement.object());
        if (!object_term) {
            qDebug() << "Failed to convert object to raptor_term";
            raptor_free_term(subject_term);
            raptor_free_term(predicate_term);
            return 0;
        }

        if ( !statement.context().isEmpty() ) {
            graph_term = convertNode(world,statement.context());
            if(!graph_term) {
                qDebug() << "Failed to convert graph/context to raptor_term. Node is" << statement.context();
                raptor_free_term(subject_term);
                raptor_free_term(predicate_term);
                raptor_free_term(object_term);
                return 0;
            }
        }


        raptor_statement* s = raptor_new_statement_from_nodes(
                world,
                subject_term,
                predicate_term,
                object_term,
                graph_term);

        if (!s) {
            qDebug() << "Failed to build raptor_statement from terms";
            raptor_free_term(subject_term);
            raptor_free_term(predicate_term);
            raptor_free_term(object_term);
            raptor_free_term(graph_term);
        }

        return s;
    }


    /*
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
    */


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
        //qDebug() << "Write char:" << (char)(byte);
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
            //qDebug() << "Write string: " << p;
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



/* We can not rely on RaptorInitHelper anymore because
 * we need raptor_world in supportedUserSerializations.
 * Instead of constantly creating new raptor_world just to
 * detect user supported serializations, it is easier to
 * embed raptor_world into class.
 * And we can not declare raptor_world as a member of
 * Serializer directly, because raptor_world is a typedef,
 * not a class. So it can not be forward declarated
 */
class Soprano::Raptor::Serializer::Private
{
    public:
        raptor_world * world;
};

Soprano::Raptor::Serializer::Serializer()
    : QObject(),
      Soprano::Serializer( "raptor" )
{
    this->d = new Private();
    this->d->world = raptor_new_world();
    raptor_world_open(d->world);
}


Soprano::Raptor::Serializer::~Serializer()
{
    raptor_free_world(d->world);
    delete d;
}


Soprano::RdfSerializations Soprano::Raptor::Serializer::supportedSerializations() const
{
    return SerializationRdfXml|SerializationNTriples|SerializationTurtle|SerializationUser;
}


QStringList Soprano::Raptor::Serializer::supportedUserSerializations() const
{
    QStringList sl;
    const raptor_syntax_description * serializer_descr = 0;
    for ( int i = 0; 1; ++i ) {
        serializer_descr =
            raptor_world_get_serializer_description(d->world,i);

        /* raptor_world_get_serializer_description will return
         * NULL when conter runs out of bonds and there is no
         * more serializers
         */
        if (!serializer_descr)
            break;

        sl << QString::fromUtf8( serializer_descr->names[0] );
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

    raptor_world * world = this->d->world;

    raptor_serializer* serializer = 0;
    QString mimeType = serializationMimeType(serialization,userSerialization);

    if ( serialization == SerializationRdfXml ) {
        serializer = raptor_new_serializer( world, "rdfxml-abbrev" ); // we always want the abbreviated xmlrdf
    }
    else {
        const raptor_syntax_description * serializer_descr = 0;
        for ( int i = 0; 1; ++i ) {
            serializer_descr =
                raptor_world_get_serializer_description(world,i);

            /* raptor_world_get_serializer_description will return
             * NULL when conter runs out of bonds and there is no
             * more serializers
             */
            if (!serializer_descr)
                break;

            /* In serializer_descr->mime_types we have a array of pairs
             * (mime_type,Q) of mime_type for this syntax. It looks like
             * we can ignore Q value in this case
             * According to documentation, this array can have zero elements
             */
            for( unsigned int mt_number = 0; mt_number < serializer_descr->mime_types_count;
                    mt_number++)
            {
                const char * mime_type_N = serializer_descr->mime_types[mt_number].mime_type;
                if ( !qstrcmp( serializationMimeType( serialization, userSerialization ).toLatin1().data(), mime_type_N ) ) {
                    serializer = raptor_new_serializer( world, serializer_descr->names[0] );
                    break;
                }
            }
            if ( serializer )
                break;
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
        raptor_uri* ns = raptor_new_uri( world,reinterpret_cast<unsigned char*>( pfit.value().toEncoded().data() ) );
        raptor_serializer_set_namespace( serializer,
                                        ns,
                                        ( unsigned char* )pfit.key().toLatin1().data() );
        raptor_free_uri( ns );
    }

    bool success = true;

    raptor_iostream_handler raptorStreamHandler;
    // ATTENTION: Raptor documentation is incorrect! Always set
    // version to 2 and raptor_iostream_calculate_modes internal
    // call will correctly determine operation mode ( read,write,readwrite)
    raptorStreamHandler.version = 2;
    raptorStreamHandler.init = 0;
    raptorStreamHandler.finish = 0;
    raptorStreamHandler.write_byte = raptorIOStreamWriteByte;
    raptorStreamHandler.write_bytes = raptorIOStreamWriteBytes;
    raptorStreamHandler.write_end = 0;
    raptorStreamHandler.read_bytes = 0;
    raptorStreamHandler.read_eof = 0;

    raptor_iostream* raptorStream = raptor_new_iostream_from_handler(
            world,
            &stream,
            &raptorStreamHandler
            );

    if (!raptorStream) {
        qDebug() << "Can not create raptor iostream";
        raptor_free_serializer(serializer);
        return false;
    }

    // raptor_serialize_start takes ownership of raptorStream
    raptor_serializer_start_to_iostream( serializer,0, raptorStream );

    while ( it.next() ) {
        raptor_statement * rs = convertStatement(world, *it );
        if (rs) {
            //qDebug() << "Serializing statement: " << *it;
            raptor_serializer_serialize_statement(serializer, rs );
            raptor_free_statement( rs );
        }
        else {
            qDebug() << "Fail to convert Soprano::Statement " <<
                *it << 
                " to raptor_statement";
            success = false;
            break;
        }
    }

    raptor_serializer_serialize_end( serializer );
    raptor_free_serializer( serializer );
    raptor_free_iostream(raptorStream);

    return success;
}

