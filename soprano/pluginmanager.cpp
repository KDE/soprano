/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2009 Sebastian Trueg <trueg@kde.org>
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
#include "pluginstub.h"
#include "backend.h"
#include "parser.h"
#include "serializer.h"
#include "soprano-config.h"
#include "sopranopluginfile.h"
#include "version.h"
#include "sopranodirs.h"

#include <QtCore/QHash>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QMutex>
#include <QtCore/QCoreApplication>


namespace {
    QString findPluginLib( const Soprano::SopranoPluginFile& file ) {
        QStringList fileSearchPaths;
        // the folder the plugin file is in
        fileSearchPaths << file.fileName().section( '/', 0, -2 );
#ifndef Q_OS_WIN
        // the lib folder in the same prefix
        fileSearchPaths << file.fileName().section( "/", 0, -5, QString::SectionIncludeTrailingSep ) + QLatin1String( "lib"SOPRANO_LIB_SUFFIX );
#endif
        return Soprano::findLibraryPath( file.library(), fileSearchPaths, QStringList() << QLatin1String( "soprano" ) );
    }

    QStringList pluginFileSearchPaths() {
        QStringList searchPaths;
        foreach( const QString& p, Soprano::dataDirs() ) {
            searchPaths << ( p + QLatin1String( "/soprano/plugins" ) );
        }
#ifdef Q_OS_WIN
        // when used as a standalone lib on windows,
        // Soprano is not installed in a default location as on linux/unix systems.
        // Thus, we search in some well known folders for plugins
        QStringList winSearchPaths;
        winSearchPaths << QCoreApplication::applicationDirPath()
                       << QCoreApplication::libraryPaths();
        foreach( const QString& p, winSearchPaths ) {
            searchPaths << p + QLatin1String( "/soprano" );
            searchPaths << p + QLatin1String( "/plugins" );
        }
#endif

        return searchPaths;
    }
}


class Soprano::PluginManager::Private
{
public:
    Private()
        : pluginsLoaded(false) {
    }

    bool addPlugin( PluginStub& stub );

    bool pluginsLoaded;

    QHash<QString, PluginStub> backends;
    QHash<QString, PluginStub> parsers;
    QHash<QString, PluginStub> serializers;
    QHash<QString, PluginStub> queryParsers;
    QHash<QString, PluginStub> querySerializers;

    QStringList searchPaths;
    bool useDefaultSearchPaths;
};


bool Soprano::PluginManager::Private::addPlugin( PluginStub& stub )
{
    if ( QObject* p = stub.plugin() ) {
        if ( qobject_cast<Soprano::Backend*>( p ) ) {
            backends.insert( stub.name(), stub );
        }
        else if ( qobject_cast<Soprano::Parser*>( p ) ) {
            parsers.insert( stub.name(), stub );
        }
        else if ( qobject_cast<Soprano::Serializer*>( p ) ) {
            serializers.insert( stub.name(), stub );
        }
        else {
            qDebug() << "Plugin" << stub.name() << "has unknown plugin type";
            return false;
        }

        return true;
    }
    else {
        return false;
    }
}


Soprano::PluginManager::PluginManager( QObject* parent )
    : QObject( parent ),
      d( new Private )
{
    setPluginSearchPath( QStringList(), true );
}


Soprano::PluginManager::~PluginManager()
{
    delete d;
}


const Soprano::Backend* Soprano::PluginManager::discoverBackendByName( const QString& name )
{
    loadAllPlugins();
    QHash<QString, PluginStub>::iterator it = d->backends.find( name );
    if( it != d->backends.end() )
        return qobject_cast<Backend*>( it->plugin() );
    else if ( !name.endsWith( QLatin1String( "backend" ) ) )
        return discoverBackendByName( name + QLatin1String( "backend" ) );
    else if ( !name.endsWith( QLatin1String( "backend" ) ) )
        return discoverBackendByName( name + QLatin1String( "backend" ) );
    else
        return 0;
}


const Soprano::Backend* Soprano::PluginManager::discoverBackendByFeatures( BackendFeatures features, const QStringList& userFeatures )
{
    loadAllPlugins();
    for( QHash<QString, PluginStub>::iterator it = d->backends.begin(); it != d->backends.end(); ++it ) {
        if( const Backend* b = qobject_cast<Backend*>( it->plugin() ) ) {
            if( b->supportsFeatures( features, userFeatures ) ) {
                return b;
            }
        }
    }
    return 0;
}


const Soprano::Parser* Soprano::PluginManager::discoverParserByName( const QString& name )
{
    loadAllPlugins();
    QHash<QString, PluginStub>::iterator it = d->parsers.find( name );
    if( it != d->parsers.end() )
        return qobject_cast<Parser*>( it->plugin() );
    else if ( !name.endsWith( QLatin1String( "parser" ) ) )
        return discoverParserByName( name + QLatin1String( "parser" ) );
    else if ( !name.endsWith( QLatin1String( "parser" ) ) )
        return discoverParserByName( name + QLatin1String( "parser" ) );
    else
        return 0;
}


const Soprano::Parser* Soprano::PluginManager::discoverParserForSerialization( RdfSerialization serialization, const QString& userSerialization )
{
    loadAllPlugins();
    for( QHash<QString, PluginStub>::iterator it = d->parsers.begin(); it != d->parsers.end(); ++it ) {
        if( const Parser* p = qobject_cast<Parser*>( it->plugin() ) ) {
            if( p->supportsSerialization( serialization, userSerialization ) ) {
                return p;
            }
        }
    }
    return 0;
}


const Soprano::Serializer* Soprano::PluginManager::discoverSerializerByName( const QString& name )
{
    loadAllPlugins();
    QHash<QString, PluginStub>::iterator it = d->serializers.find( name );
    if( it != d->serializers.end() )
        return qobject_cast<Serializer*>( it->plugin() );
    else if ( !name.endsWith( QLatin1String( "serializer" ) ) )
        return discoverSerializerByName( name + QLatin1String( "serializer" ) );
    else if ( !name.endsWith( QLatin1String( "serializer" ) ) )
        return discoverSerializerByName( name + QLatin1String( "serializer" ) );
    else
        return 0;
}


const Soprano::Serializer* Soprano::PluginManager::discoverSerializerForSerialization( RdfSerialization serialization, const QString& userSerialization )
{
    loadAllPlugins();
    for( QHash<QString, PluginStub>::iterator it = d->serializers.begin(); it != d->serializers.end(); ++it ) {
        if( const Serializer* p = qobject_cast<Serializer*>( it->plugin() ) ) {
            if( p->supportsSerialization( serialization, userSerialization ) ) {
                return p;
            }
        }
    }
    return 0;
}


QList<const Soprano::Backend*> Soprano::PluginManager::allBackends()
{
    loadAllPlugins();
    QList<const Backend*> bl;
    for ( QHash<QString, PluginStub>::iterator it = d->backends.begin();
          it != d->backends.end(); ++it ) {
        if( const Backend* p = qobject_cast<Backend*>( it->plugin() ) ) {
            bl.append( p );
        }
    }
    return bl;
}


QList<const Soprano::Parser*> Soprano::PluginManager::allParsers()
{
    loadAllPlugins();
    QList<const Parser*> pl;
    for ( QHash<QString, PluginStub>::iterator it = d->parsers.begin();
          it != d->parsers.end(); ++it ) {
        if( const Parser* p = qobject_cast<Parser*>( it->plugin() ) ) {
            pl.append( p );
        }
    }
    return pl;
}


QList<const Soprano::Serializer*> Soprano::PluginManager::allSerializers()
{
    loadAllPlugins();
    QList<const Serializer*> pl;
    for ( QHash<QString, PluginStub>::iterator it = d->serializers.begin();
          it != d->serializers.end(); ++it ) {
        if( const Serializer* p = qobject_cast<Serializer*>( it->plugin() ) ) {
            pl.append( p );
        }
    }
    return pl;
}


void Soprano::PluginManager::loadAllPlugins()
{
    if( !d->pluginsLoaded ) {
//        qDebug() << "(Soprano::PluginManager) loading all plugins";

        QStringList searchPaths = d->searchPaths;
        if ( d->useDefaultSearchPaths ) {
            searchPaths << pluginFileSearchPaths();
        }

        Q_FOREACH( const QString& dir, searchPaths ) {
            QDir pluginDir( dir );
//            qDebug() << "(Soprano::PluginManager) searching plugin file from " << pluginDir.absolutePath();
            QStringList pluginFiles = pluginDir.entryList( QStringList( QLatin1String( "*.desktop" ) ) );
            Q_FOREACH( const QString& plugin, pluginFiles ) {
                loadPlugin( pluginDir.absoluteFilePath( plugin ) );
            }
        }

        d->pluginsLoaded = true;
    }
}

void Soprano::PluginManager::loadPlugin( const QString& path )
{
    SopranoPluginFile f;
//    qDebug() << path;
    if ( f.open( path ) ) {
//        qDebug() << "(Soprano::PluginManager) found plugin file" << path;
        if ( f.sopranoVersion().left( f.sopranoVersion().indexOf( '.' ) ).toUInt() == Soprano::versionMajor() ) {
//            qDebug() << "(Soprano::PluginManager) plugin has proper version.";

            QString libPath = findPluginLib( f );
            if ( libPath.isEmpty() ) {
                qDebug() << "Failed to find the library for plugin" << f.pluginName() << "(" << f.library() << ")";
                return;
            }

            QString type = f.serviceTypes();
            QString name = f.pluginName();
            if( name.isEmpty() ) {
                name = f.fileName().section( '/', -1 );
                if( name.endsWith( ".desktop" ) ) {
                    name.truncate( name.length() - 8 );
                }
            }

            PluginStub plugin( name, libPath );

            if( type.contains( "Soprano/Backend" ) ) {
                if ( !d->backends.contains( name ) ) {
//                    qDebug() << "(Soprano::PluginManager) found backend plugin " << name;
                    d->backends.insert( name, plugin );
                }
            }
            else if( type.contains( "Soprano/Parser" ) ) {
                if ( !d->parsers.contains( name ) ) {
//                    qDebug() << "(Soprano::PluginManager) found parser plugin " << name;
                    d->parsers.insert( name, plugin );
                }
            }
            else if( type.contains( "Soprano/Serializer" ) ) {
                if ( !d->serializers.contains( name ) ) {
//                    qDebug() << "(Soprano::PluginManager) found serializer plugin " << name;
                    d->serializers.insert( name, plugin );
                }
            }
            else if( type.contains( "Soprano/QueryParser" ) ) {
                if ( !d->queryParsers.contains( name ) ) {
//                    qDebug() << "(Soprano::PluginManager) found query parser plugin " << name;
                    d->queryParsers.insert( name, plugin );
                }
            }
            else if( type.contains( "Soprano/QuerySerializer" ) ) {
                if ( !d->querySerializers.contains( name ) ) {
//                    qDebug() << "(Soprano::PluginManager) found query serializer plugin " << name;
                    d->querySerializers.insert( name, plugin );
                }
            }
            else {
                qDebug() << "(Soprano::PluginManager) invalid plugin type:" << type;
            }
        }
        else {
        	   qDebug() << "(Soprano::PluginManager) plugin has major version:"
        	            << f.sopranoVersion().left( f.sopranoVersion().indexOf( '.' ) ).toUInt()
                        << "required major version:" << Soprano::versionMajor();
        }
    }
}


void Soprano::PluginManager::setPluginSearchPath( const QStringList& paths, bool useDefaults )
{
    d->searchPaths = paths;
    d->useDefaultSearchPaths = useDefaults;
}


bool Soprano::PluginManager::loadCustomPlugin( const QString& path )
{
    PluginStub stub( QString(), path );
    return d->addPlugin( stub );
}


const Soprano::Backend* Soprano::PluginManager::loadCustomBackend( const QString& path )
{
    PluginStub stub( QString(), path );
    if ( Backend* pp = qobject_cast<Backend*>( stub.plugin() ) ) {
        if ( d->addPlugin( stub ) ) {
            return pp;
        }
    }
    return 0;
}


const Soprano::Parser* Soprano::PluginManager::loadCustomParser( const QString& path )
{
    PluginStub stub( QString(), path );
    if ( Parser* pp = qobject_cast<Parser*>( stub.plugin() ) ) {
        if ( d->addPlugin( stub ) ) {
            return pp;
        }
    }
    return 0;
}


const Soprano::Serializer* Soprano::PluginManager::loadCustomSerializer( const QString& path )
{
    PluginStub stub( QString(), path );
    if ( Serializer* pp = qobject_cast<Serializer*>( stub.plugin() ) ) {
        if ( d->addPlugin( stub ) ) {
            return pp;
        }
    }
    return 0;
}


namespace Soprano {
    /**
     * Little hack class to ensure BC (cannot make PluginManager constructor public)
     * and at the same time use Q_GLOBAL_STATIC
     */
    class PluginManagerFactory
    {
    public:
        Soprano::PluginManager manager;
    };
}

Q_GLOBAL_STATIC( Soprano::PluginManagerFactory, s_pluginManagerFactory )

Soprano::PluginManager* Soprano::PluginManager::instance()
{
    return &s_pluginManagerFactory()->manager;
}

#include "pluginmanager.moc"
