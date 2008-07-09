/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
 * Copyright (C) 2007-2008 Sebastian Trueg <trueg@kde.org>
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

#include "redlandworld.h"
#include "redlandmodel.h"

#include "locator.h"

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QDebug>


static QString redlandLogFacilityToString( int facility )
{
    switch( facility ) {
    default:
    case LIBRDF_FROM_NONE:
        return QLatin1String( "unknown" );
    case LIBRDF_FROM_CONCEPTS:
        return QLatin1String( "concepts" );
    case LIBRDF_FROM_DIGEST:
        return QLatin1String( "digest" );
    case LIBRDF_FROM_FILES:
        return QLatin1String( "files" );
    case LIBRDF_FROM_HASH:
        return QLatin1String( "hash" );
    case LIBRDF_FROM_INIT:
        return QLatin1String( "init" );
    case LIBRDF_FROM_ITERATOR:
        return QLatin1String( "iterator" );
    case LIBRDF_FROM_LIST:
        return QLatin1String( "list" );
    case LIBRDF_FROM_MODEL:
        return QLatin1String( "model" );
    case LIBRDF_FROM_NODE:
        return QLatin1String( "node" );
    case LIBRDF_FROM_PARSER:
        return QLatin1String( "parser" );
    case LIBRDF_FROM_QUERY:
        return QLatin1String( "query" );
    case LIBRDF_FROM_SERIALIZER:
        return QLatin1String( "serializer" );
    case LIBRDF_FROM_STATEMENT:
        return QLatin1String( "statement" );
    case LIBRDF_FROM_STORAGE:
        return QLatin1String( "storage" );
    case LIBRDF_FROM_STREAM:
        return QLatin1String( "stream" );
    case LIBRDF_FROM_URI:
        return QLatin1String( "uri" );
    case LIBRDF_FROM_UTF8:
        return QLatin1String( "utf8" );
    case LIBRDF_FROM_MEMORY:
        return QLatin1String( "memory" );
    }
}

static QString redlandLogLevelToString( int level )
{
    switch( level ) {
    default:
    case LIBRDF_LOG_NONE:
        return QLatin1String( "unknown" );
    case LIBRDF_LOG_DEBUG:
        return QLatin1String( "debugging message" );
    case LIBRDF_LOG_INFO:
        return QLatin1String( "information" );
    case LIBRDF_LOG_WARN:
        return QLatin1String( "warning" );
    case LIBRDF_LOG_ERROR:
        return QLatin1String( "error" );
    case LIBRDF_LOG_FATAL:
        return QLatin1String( "fatal" );
    }
}


int redlandLogHandler( void* user, librdf_log_message* msg )
{
    Soprano::Redland::World* w = reinterpret_cast<Soprano::Redland::World*>( user );

    int level = librdf_log_message_level( msg );
    QString message = QString( "%1: %2 (%3)" )
                      .arg( redlandLogFacilityToString( librdf_log_message_facility( msg ) ) )
                      .arg( librdf_log_message_message( msg ) )
                      .arg( redlandLogLevelToString( level ) );

    // this is a "real" error, everything else is just debugging (FIXME: should we report those, too?)
    if ( level > LIBRDF_LOG_WARN ) {
        int code = Soprano::Error::ErrorUnknown + librdf_log_message_code( msg );
        raptor_locator* locator = librdf_log_message_locator( msg );
        if ( locator ) {
            w->setError( Soprano::Error::ParserError( Soprano::Error::Locator( locator->line, locator->column, locator->byte ), message, code ) );
        }
        else {
            w->setError( Soprano::Error::Error( message, code ) );
        }
    }

    qDebug() << "(Soprano::Redland) " << message;

    // message handled
    return 1;
}


Soprano::Redland::World::World()
{
    m_world = librdf_new_world();
    librdf_world_open( m_world );
    librdf_world_set_logger( m_world, this, redlandLogHandler );
}

Soprano::Redland::World::~World()
{
    librdf_free_world( m_world );
}


librdf_world* Soprano::Redland::World::worldPtr() const
{
    return m_world;
}


librdf_node *Soprano::Redland::World::createNode( const Node &node )
{
    librdf_world *world = worldPtr();

    if ( node.isResource() ) {
        return librdf_new_node_from_uri_string( world, (unsigned char *)node.uri().toEncoded().data() );
    }
    else if ( node.isBlank() ) {
        return librdf_new_node_from_blank_identifier( world, (unsigned char *) node.identifier().toUtf8().data() );
    }
    else if ( node.isLiteral() ) {
        return librdf_new_node_from_typed_literal( world,
                                                   (unsigned char *)node.literal().toString().toUtf8().data(),
                                                   node.language().toUtf8().data(),
                                                   librdf_new_uri( world, (const unsigned char*)node.dataType().toEncoded().data() ) );
    }

    return 0;
}


librdf_statement *Soprano::Redland::World::createStatement( const Statement &statement )
{
    librdf_world *world = worldPtr();

    librdf_node *subject = createNode( statement.subject() );
    librdf_node *predicate = createNode( statement.predicate() );
    librdf_node *object = createNode( statement.object() );

    return librdf_new_statement_from_nodes( world, subject, predicate, object );
}


// Hint: all strings in redland are Utf8, except for the literal language tags.

Soprano::Node Soprano::Redland::World::createNode( librdf_node *node )
{
    if ( librdf_node_is_resource( node ) ) {
        librdf_uri *uri = librdf_node_get_uri( node );
        return Soprano::Node( QUrl::fromEncoded( (const char *)librdf_uri_as_string(uri), QUrl::StrictMode ) );
    }
    else if ( librdf_node_is_blank( node ) ) {
        return Soprano::Node( QString::fromUtf8( (const char *)librdf_node_get_blank_identifier( node ) ) );
    }
    else if ( librdf_node_is_literal( node ) ) {
        librdf_uri* datatype = librdf_node_get_literal_value_datatype_uri( node );
        if ( !datatype ) {
            return Soprano::Node( Soprano::LiteralValue( QString::fromUtf8( (const char *)librdf_node_get_literal_value( node ) ) ),
                                  QString::fromAscii( librdf_node_get_literal_value_language( node ) ) );
        }
        return Soprano::Node( Soprano::LiteralValue::fromString( QString::fromUtf8( (const char *)librdf_node_get_literal_value( node ) ),
                                                                 QUrl::fromEncoded( (const char *)librdf_uri_as_string( datatype ),
                                                                                    QUrl::StrictMode ) ),
                              QString::fromAscii( librdf_node_get_literal_value_language( node ) ) );
    }

    return Soprano::Node();
}


Soprano::Statement Soprano::Redland::World::createStatement( librdf_statement *st )
{
    librdf_node *subject = librdf_statement_get_subject( st );
    librdf_node *predicate = librdf_statement_get_predicate( st );
    librdf_node *object = librdf_statement_get_object( st );

    return Soprano::Statement( createNode( subject), createNode( predicate), createNode( object ) );
}

void Soprano::Redland::World::freeNode( librdf_node* node )
{
    if( node ) {
        librdf_free_node( node );
    }
}


void Soprano::Redland::World::freeStatement( librdf_statement* statement )
{
    if( statement ) {
        librdf_free_statement( statement );
    }
}
