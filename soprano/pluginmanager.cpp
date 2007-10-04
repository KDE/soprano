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
#include "sopranopluginfile.h"
#include "version.h"

#include <QtCore/QHash>
#include <QtCore/QDir>
#include <QtCore/QPluginLoader>
#include <QtCore/QDebug>
#include <QtCore/QMutex>
#include <QtCore/QLibrary>

#include <stdlib.h>

#if defined _WIN32 || defined _WIN64
#define PATH_SEPARATOR ';'
#else
#define PATH_SEPARATOR ':'
#endif


class Soprano::PluginManager::Private
{
public:
    QHash<QString, Backend*> backends;
    QHash<QString, Parser*> parsers;
    QHash<QString, Serializer*> serializers;
    QHash<QString, Query::Parser*> queryParsers;
    QHash<QString, Query::Serializer*> querySerializers;
};


namespace {
    inline QString makeLibName( const QString &libname )
    {
#ifdef Q_OS_WIN
        if (!libname.endsWith(".dll")) {
            return libname + ".dll";
        }
        return libname;
#else
        int pos = libname.lastIndexOf('/');
        if (pos < 0) {
            pos = 0;
        }
        if (libname.indexOf('.', pos) < 0) {
            const char* const extList[] = { ".so", ".dylib", ".bundle", ".sl" };
            for (uint i = 0; i < sizeof(extList) / sizeof(*extList); ++i) {
                if (QLibrary::isLibrary(libname + extList[i]))
                    return libname + extList[i];
            }
        }
        return libname;
#endif
    }

    QStringList envDirList( const char* var )
    {
        QStringList dirs;
        QByteArray varData = qgetenv( var );
        if ( !varData.isEmpty() ) {
            QStringList d = QString::fromLocal8Bit( varData ).split( PATH_SEPARATOR );
            Q_FOREACH( QString dir, d ) {
                dirs += QDir::fromNativeSeparators( dir );
            }
        }
        return dirs;
    }

    QStringList dataDirs()
    {
        QStringList paths;

        paths += QLatin1String(SOPRANO_PREFIX"/share");
        paths += envDirList( "SOPRANO_DIRS" );
        paths += envDirList( "XDG_DATA_DIRS" );

        return paths;
    }

    QString findPluginLib( const Soprano::SopranoPluginFile& file )
    {
        QString lib = makeLibName( file.library() );
        if ( lib.startsWith( "/" ) ) {
            return lib;
        }
        else {
            // using the same prefix should be sufficient
            lib.prepend( file.fileName().section( "/", 0, -5, QString::SectionIncludeTrailingSep ) + "lib/soprano/" );
            qDebug() << lib;
            if ( QFile::exists( lib ) ) {
                return lib;
            }
            return QString();
        }
    }
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
    qDeleteAll( d->backends );
    qDeleteAll( d->parsers );
    qDeleteAll( d->serializers );
    qDeleteAll( d->queryParsers );
    qDeleteAll( d->querySerializers );
    delete d;
}


void Soprano::PluginManager::loadAllPlugins()
{
    qDebug() << "(Soprano::PluginManager) loading all plugins";

    QStringList dirs = dataDirs();
    Q_FOREACH( QString dir, dirs ) {
        QDir pluginDir( dir + "/soprano/plugins" );
        QStringList pluginFiles = pluginDir.entryList( QStringList( QLatin1String( "*.desktop" ) ) );
        Q_FOREACH( QString plugin, pluginFiles ) {
            loadPlugin( pluginDir.absoluteFilePath( plugin ) );
        }
    }
}

void Soprano::PluginManager::loadPlugin( const QString& path )
{
    SopranoPluginFile f;
    if ( f.open( path ) ) {
        qDebug() << "(Soprano::PluginManager) found plugin file" << path;
        if ( f.sopranoVersion().left( f.sopranoVersion().indexOf( '.' ) ).toUInt() == Soprano::versionMajor() ) {
            qDebug() << "(Soprano::PluginManager) plugin has proper version. Trying to load lib:" << f.library();
            QString libPath = findPluginLib( f );
            QPluginLoader loader( libPath );
            QObject* plugin = loader.instance();
            Plugin* sopranoPlugin = dynamic_cast<Plugin*>( plugin );
            if( plugin && sopranoPlugin ) {
                qDebug() << "(Soprano::PluginManager) loaded something from" << loader.fileName();
                if ( !sopranoPlugin->isAvailable() ) {
                    qDebug() << "(Soprano::PluginManager) plugin " << sopranoPlugin->pluginName() << "is not available.";
                }
                else {
                    if( Backend* backend = qobject_cast<Backend*>( plugin ) ) {
                        if ( !d->backends.contains( backend->pluginName() ) ) {
                            qDebug() << "(Soprano::PluginManager) found backend plugin " << backend->pluginName();
                            d->backends.insert( backend->pluginName(), backend );
                        }
                    }
                    else if( Parser* parser = qobject_cast<Parser*>( plugin ) ) {
                        if ( !d->parsers.contains( parser->pluginName() ) ) {
                            qDebug() << "(Soprano::PluginManager) found parser plugin " << parser->pluginName();
                            d->parsers.insert( parser->pluginName(), parser );
                        }
                    }
                    else if( Serializer* serializer = qobject_cast<Serializer*>( plugin ) ) {
                        if ( !d->serializers.contains( serializer->pluginName() ) ) {
                            qDebug() << "(Soprano::PluginManager) found serializer plugin " << serializer->pluginName();
                            d->serializers.insert( serializer->pluginName(), serializer );
                        }
                    }
                    else if( Query::Parser* parser = qobject_cast<Query::Parser*>( plugin ) ) {
                        if ( !d->queryParsers.contains( parser->pluginName() ) ) {
                            qDebug() << "(Soprano::PluginManager) found query parser plugin " << parser->pluginName();
                            d->queryParsers.insert( parser->pluginName(), parser );
                        }
                    }
                    else if( Query::Serializer* serializer = qobject_cast<Query::Serializer*>( plugin ) ) {
                        if ( !d->querySerializers.contains( serializer->pluginName() ) ) {
                            qDebug() << "(Soprano::PluginManager) found query serializer plugin " << serializer->pluginName();
                            d->querySerializers.insert( serializer->pluginName(), serializer );
                        }
                    }
                    else {
                        qDebug() << "(Soprano::PluginManager) found no soprano plugin at " << loader.fileName() << plugin;
                        delete plugin;
                    }
                }
            }
            else {
                qDebug() << "(Soprano::PluginManager) found no plugin at " << libPath;
            }
        }
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
