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


namespace {
    QString findPluginLib( const Soprano::SopranoPluginFile& file ) {
        return Soprano::findLibraryPath( "soprano/" + file.library(),
                                         QStringList() << file.fileName().section( "/", 0, -5, QString::SectionIncludeTrailingSep ) + "lib" );
    }
}


class Soprano::PluginManager::Private
{
public:
    Private()
        : pluginsLoaded(false) {
    }

    bool pluginsLoaded;

    QHash<QString, PluginStub> backends;
    QHash<QString, PluginStub> parsers;
    QHash<QString, PluginStub> serializers;
    QHash<QString, PluginStub> queryParsers;
    QHash<QString, PluginStub> querySerializers;
};


Soprano::PluginManager::PluginManager( QObject* parent )
    : QObject( parent ),
      d( new Private )
{
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
        return dynamic_cast<Backend*>( it->plugin() );
    else if ( !name.endsWith( QLatin1String( "backend" ) ) )
        return discoverBackendByName( name + QLatin1String( "backend" ) );
    else
        return 0;
}


const Soprano::Backend* Soprano::PluginManager::discoverBackendByFeatures( BackendFeatures features, const QStringList& userFeatures )
{
    loadAllPlugins();
    for( QHash<QString, PluginStub>::iterator it = d->backends.begin(); it != d->backends.end(); ++it ) {
        if( const Backend* b = dynamic_cast<Backend*>( it->plugin() ) ) {
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
        return dynamic_cast<Parser*>( it->plugin() );
    else if ( !name.endsWith( QLatin1String( "parser" ) ) )
        return discoverParserByName( name + QLatin1String( "parser" ) );
    else
        return 0;
}


const Soprano::Parser* Soprano::PluginManager::discoverParserForSerialization( RdfSerialization serialization, const QString& userSerialization )
{
    loadAllPlugins();
    for( QHash<QString, PluginStub>::iterator it = d->parsers.begin(); it != d->parsers.end(); ++it ) {
        if( const Parser* p = dynamic_cast<Parser*>( it->plugin() ) ) {
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
        return dynamic_cast<Serializer*>( it->plugin() );
    else if ( !name.endsWith( QLatin1String( "serializer" ) ) )
        return discoverSerializerByName( name + QLatin1String( "serializer" ) );
    else
        return 0;
}


const Soprano::Serializer* Soprano::PluginManager::discoverSerializerForSerialization( RdfSerialization serialization, const QString& userSerialization )
{
    loadAllPlugins();
    for( QHash<QString, PluginStub>::iterator it = d->serializers.begin(); it != d->serializers.end(); ++it ) {
        if( const Serializer* p = dynamic_cast<Serializer*>( it->plugin() ) ) {
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
        if( const Backend* p = dynamic_cast<Backend*>( it->plugin() ) ) {
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
        if( const Parser* p = dynamic_cast<Parser*>( it->plugin() ) ) {
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
        if( const Serializer* p = dynamic_cast<Serializer*>( it->plugin() ) ) {
            pl.append( p );
        }
    }
    return pl;
}


void Soprano::PluginManager::loadAllPlugins()
{
    if( !d->pluginsLoaded ) {
        qDebug() << "(Soprano::PluginManager) loading all plugins";

        QStringList dirs = dataDirs();
        Q_FOREACH( const QString &dir, dirs ) {
            QDir pluginDir( dir + "/soprano/plugins" );
            qDebug() << "(Soprano::PluginManager) searching plugin file from " << pluginDir.absolutePath();
            QStringList pluginFiles = pluginDir.entryList( QStringList( QLatin1String( "*.desktop" ) ) );
            Q_FOREACH( const QString &plugin, pluginFiles ) {
                loadPlugin( pluginDir.absoluteFilePath( plugin ) );
            }
        }

        d->pluginsLoaded = true;
    }
}

void Soprano::PluginManager::loadPlugin( const QString& path )
{
    SopranoPluginFile f;
    if ( f.open( path ) ) {
        qDebug() << "(Soprano::PluginManager) found plugin file" << path;
        if ( f.sopranoVersion().left( f.sopranoVersion().indexOf( '.' ) ).toUInt() == Soprano::versionMajor() ) {
            qDebug() << "(Soprano::PluginManager) plugin has proper version.";
            QString libPath = findPluginLib( f );

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
                    qDebug() << "(Soprano::PluginManager) found backend plugin " << name;
                    d->backends.insert( name, plugin );
                }
            }
            else if( type.contains( "Soprano/Parser" ) ) {
                if ( !d->parsers.contains( name ) ) {
                    qDebug() << "(Soprano::PluginManager) found parser plugin " << name;
                    d->parsers.insert( name, plugin );
                }
            }
            else if( type.contains( "Soprano/Serializer" ) ) {
                if ( !d->serializers.contains( name ) ) {
                    qDebug() << "(Soprano::PluginManager) found serializer plugin " << name;
                    d->serializers.insert( name, plugin );
                }
            }
            else if( type.contains( "Soprano/QueryParser" ) ) {
                if ( !d->queryParsers.contains( name ) ) {
                    qDebug() << "(Soprano::PluginManager) found query parser plugin " << name;
                    d->queryParsers.insert( name, plugin );
                }
            }
            else if( type.contains( "Soprano/QuerySerializer" ) ) {
                if ( !d->querySerializers.contains( name ) ) {
                    qDebug() << "(Soprano::PluginManager) found query serializer plugin " << name;
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


Soprano::PluginManager* Soprano::PluginManager::instance()
{
    static PluginManager* s_instance = 0;
    static QMutex s_mutex;
    s_mutex.lock();
    if( !s_instance ) {
        s_instance = new PluginManager();
    }
    s_mutex.unlock();
    return s_instance;
}

#include "pluginmanager.moc"
