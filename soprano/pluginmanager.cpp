/*
 * This file is part of Soprano Project.
 *
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

#include "pluginmanager.h"
#include "backend.h"
#include "parser.h"
#include "serializer.h"
#include "query/queryparser.h"
#include "query/queryserializer.h"
#include "config.h"

#include <QHash>
#include <QDir>
#include <QPluginLoader>
#include <QDebug>
#include <QtCore/QMutex>

#include <stdlib.h>


class Soprano::PluginManager::Private
{
public:
    QHash<QString, Backend*> backends;
    QHash<QString, Parser*> parsers;
    QHash<QString, Serializer*> serializers;
    QHash<QString, Query::Parser*> queryParsers;
    QHash<QString, Query::Serializer*> querySerializers;
};


const QStringList Soprano::PluginManager::libraryPath()
{
    QStringList pluginPaths;

    pluginPaths += QString(SOPRANO_LIB_DIR);

    QByteArray libPath = qgetenv( "SOPRANO_LIBRARY_PATH" );
    if( !libPath.isEmpty() ) {
        pluginPaths += QString::fromLocal8Bit( libPath ).split(":");
    }

    return pluginPaths;
}

const Soprano::Backend* Soprano::PluginManager::discoverBackendByName( const QString& name )
{
    QHash<QString, Backend*>::iterator it = d->backends.find( name );
    if( it != d->backends.end() )
        return *it;
    else
        return 0;
}


const Soprano::Backend* Soprano::PluginManager::discoverBackendByFeatures( BackendFeatures features, const QStringList& userFeatures )
{
    for( QHash<QString, Backend*>::const_iterator it = d->backends.begin(); it != d->backends.end(); ++it ) {
        const Backend* b = *it;
        if( b->supportsFeatures( features, userFeatures ) )
            return b;
    }
    return 0;
}


const Soprano::Parser* Soprano::PluginManager::discoverParserByName( const QString& name )
{
    QHash<QString, Parser*>::iterator it = d->parsers.find( name );
    if( it != d->parsers.end() )
        return *it;
    else
        return 0;
}


const Soprano::Parser* Soprano::PluginManager::discoverParserForSerialization( RdfSerialization serialization, const QString& userSerialization )
{
    for( QHash<QString, Parser*>::const_iterator it = d->parsers.begin(); it != d->parsers.end(); ++it ) {
        const Parser* p = *it;
        if( p->supportsSerialization( serialization, userSerialization ) ) {
            return p;
        }
    }
    return 0;
}


const Soprano::Serializer* Soprano::PluginManager::discoverSerializerByName( const QString& name )
{
    QHash<QString, Serializer*>::iterator it = d->serializers.find( name );
    if( it != d->serializers.end() )
        return *it;
    else
        return 0;
}


const Soprano::Serializer* Soprano::PluginManager::discoverSerializerForSerialization( RdfSerialization serialization, const QString& userSerialization )
{
    for( QHash<QString, Serializer*>::const_iterator it = d->serializers.begin(); it != d->serializers.end(); ++it ) {
        const Serializer* p = *it;
        if( p->supportsSerialization( serialization, userSerialization ) ) {
            return p;
        }
    }
    return 0;
}


const Soprano::Query::Parser* Soprano::PluginManager::discoverQueryParserByName( const QString& name )
{
    QHash<QString, Query::Parser*>::iterator it = d->queryParsers.find( name );
    if( it != d->queryParsers.end() )
        return *it;
    else
        return 0;
}


const Soprano::Query::Parser* Soprano::PluginManager::discoverQueryParserForQueryLanguage( Query::QueryLanguage lang, const QString& userQueryLanguage )
{
    for( QHash<QString, Query::Parser*>::const_iterator it = d->queryParsers.begin(); it != d->queryParsers.end(); ++it ) {
        const Query::Parser* p = *it;
        if( p->supportsQueryLanguage( lang, userQueryLanguage ) ) {
            return p;
        }
    }
    return 0;
}


const Soprano::Query::Serializer* Soprano::PluginManager::discoverQuerySerializerByName( const QString& name )
{
    QHash<QString, Query::Serializer*>::iterator it = d->querySerializers.find( name );
    if( it != d->querySerializers.end() )
        return *it;
    else
        return 0;
}


const Soprano::Query::Serializer* Soprano::PluginManager::discoverQuerySerializerForQueryLanguage( Query::QueryLanguage lang, const QString& userQueryLanguage )
{
    for( QHash<QString, Query::Serializer*>::const_iterator it = d->querySerializers.begin(); it != d->querySerializers.end(); ++it ) {
        const Query::Serializer* p = *it;
        if( p->supportsQueryLanguage( lang, userQueryLanguage ) ) {
            return p;
        }
    }
    return 0;
}


QList<const Soprano::Backend*> Soprano::PluginManager::allBackends()
{
    QList<const Backend*> bl;
    for ( QHash<QString, Backend*>::const_iterator it = d->backends.constBegin();
          it != d->backends.constEnd(); ++it ) {
        bl.append( it.value() );
    }
    return bl;
}


QList<const Soprano::Parser*> Soprano::PluginManager::allParsers()
{
    QList<const Parser*> pl;
    for ( QHash<QString, Parser*>::const_iterator it = d->parsers.constBegin();
          it != d->parsers.constEnd(); ++it ) {
        pl.append( it.value() );
    }
    return pl;
}


QList<const Soprano::Serializer*> Soprano::PluginManager::allSerializers()
{
    QList<const Serializer*> pl;
    for ( QHash<QString, Serializer*>::const_iterator it = d->serializers.constBegin();
          it != d->serializers.constEnd(); ++it ) {
        pl.append( it.value() );
    }
    return pl;
}


QList<const Soprano::Query::Parser*> Soprano::PluginManager::allQueryParsers()
{
    QList<const Query::Parser*> pl;
    for ( QHash<QString, Query::Parser*>::const_iterator it = d->queryParsers.constBegin();
          it != d->queryParsers.constEnd(); ++it ) {
        pl.append( it.value() );
    }
    return pl;
}


QList<const Soprano::Query::Serializer*> Soprano::PluginManager::allQuerySerializers()
{
    QList<const Query::Serializer*> pl;
    for ( QHash<QString, Query::Serializer*>::const_iterator it = d->querySerializers.constBegin();
          it != d->querySerializers.constEnd(); ++it ) {
        pl.append( it.value() );
    }
    return pl;
}


Soprano::PluginManager::PluginManager( QObject* parent )
    : QObject( parent ),
      d( new Private )
{
}


Soprano::PluginManager::~PluginManager()
{
    // delete all plugins (is this necessary?)
    for( QHash<QString, Backend*>::const_iterator it = d->backends.begin(); it != d->backends.end(); ++it )
        delete it.value();
    for( QHash<QString, Parser*>::const_iterator it = d->parsers.begin(); it != d->parsers.end(); ++it )
        delete it.value();
    for( QHash<QString, Serializer*>::const_iterator it = d->serializers.begin(); it != d->serializers.end(); ++it )
        delete it.value();
    for( QHash<QString, Query::Parser*>::const_iterator it = d->queryParsers.begin(); it != d->queryParsers.end(); ++it )
        delete it.value();
    for( QHash<QString, Query::Serializer*>::const_iterator it = d->querySerializers.begin(); it != d->querySerializers.end(); ++it )
        delete it.value();
    delete d;
}


void Soprano::PluginManager::loadAllPlugins()
{
    qDebug() << "(Soprano::PluginManager) loading all plugins" << endl;

    QStringList libPath = libraryPath();
    for( QStringList::const_iterator it = libPath.begin(); it != libPath.end(); ++it ) {
        if( QFile::exists( *it + "/soprano" ) )
            loadPlugins( *it + "/soprano" );
    }
}


void Soprano::PluginManager::loadPlugins( const QString& path )
{
    qDebug() << "(Soprano::PluginManager) searching plugin dir " << path << endl;

    QDir pluginsDir( path );
    foreach( QString fileName, pluginsDir.entryList(QDir::Files) ) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject* plugin = loader.instance();
        if( plugin ) {
            if( Backend* backend = qobject_cast<Backend*>( plugin ) ) {
                qDebug() << "(Soprano::PluginManager) found backend plugin " << backend->pluginName() << endl;
                d->backends.insert( backend->pluginName(), backend );
            }
            else if( Parser* parser = qobject_cast<Parser*>( plugin ) ) {
                qDebug() << "(Soprano::PluginManager) found parser plugin " << parser->pluginName() << endl;
                d->parsers.insert( parser->pluginName(), parser );
            }
            else if( Serializer* serializer = qobject_cast<Serializer*>( plugin ) ) {
                qDebug() << "(Soprano::PluginManager) found serializer plugin " << serializer->pluginName() << endl;
                d->serializers.insert( serializer->pluginName(), serializer );
            }
            else if( Query::Parser* parser = qobject_cast<Query::Parser*>( plugin ) ) {
                qDebug() << "(Soprano::PluginManager) found query parser plugin " << parser->pluginName() << endl;
                d->queryParsers.insert( parser->pluginName(), parser );
            }
            else if( Query::Serializer* serializer = qobject_cast<Query::Serializer*>( plugin ) ) {
                qDebug() << "(Soprano::PluginManager) found query serializer plugin " << serializer->pluginName() << endl;
                d->querySerializers.insert( serializer->pluginName(), serializer );
            }
            else {
                qDebug() << "(Soprano::PluginManager) found no soprano plugin at " << loader.fileName() << plugin << endl;
                delete plugin;
            }
        }
        else
            qDebug() << "(Soprano::PluginManager) found no plugin at " << loader.fileName() << endl;
    }
}


Soprano::PluginManager* Soprano::PluginManager::instance()
{
    static PluginManager* s_instance = 0;
    static QMutex s_mutex;
    s_mutex.lock();
    if( !s_instance ) {
        s_instance = new PluginManager();
        s_instance->loadAllPlugins();
    }
    s_mutex.unlock();
    return s_instance;
}

#include "pluginmanager.moc"
