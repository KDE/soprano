/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
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


Q_GLOBAL_STATIC( Soprano::Redland::World, worldInstance )


Soprano::Redland::World* Soprano::Redland::World::self()
{
    return worldInstance();
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
