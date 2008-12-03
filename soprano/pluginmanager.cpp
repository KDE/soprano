/*
 * This file is part of Soprano Project.
 *
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

#include "pluginmanager.h"
#include "pluginstub.h"
#include "backend.h"
#include "parser.h"
#include "parser2.h"
#include "serializer.h"
#include "serializer2.h"
#include "soprano-config.h"
#include "sopranopluginfile.h"
#include "version.h"

#include <QtCore/QHash>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QMutex>
#include <QtCore/QLibrary>

#include <stdlib.h>

#if defined _WIN32 || defined _WIN64
#define PATH_SEPARATOR ';'
#else
#define PATH_SEPARATOR ':'
#endif


namespace {
    inline QStringList makeLibName( const QString &libname )
    {
        int pos = libname.lastIndexOf('/');
        if (pos < 0) {
            pos = 0;
        }

        QStringList libNames;
        if (libname.indexOf('.', pos) < 0) {
            const char* const extList[] = { ".so", ".dylib", ".bundle", ".sl", ".dll" };
            for (uint i = 0; i < sizeof(extList) / sizeof(*extList); ++i) {
                if (QLibrary::isLibrary(libname + extList[i])) {
                    libNames.append(libname + extList[i]);
                }
            }
        }
        else {
            libNames.append(libname);
        }
        return libNames;
    }

    QStringList envDirList( const char* var )
    {
        QStringList dirs;
        QByteArray varData = qgetenv( var );
        if ( !varData.isEmpty() ) {
            QStringList d = QString::fromLocal8Bit( varData ).split( PATH_SEPARATOR );
            Q_FOREACH( const QString &dir, d ) {
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

    /**
     * lib dirs without lib suffix or trailing slash.
     */
    QStringList libDirs()
    {
        QStringList paths;
        paths << QLatin1String( SOPRANO_PREFIX"/lib" );
        paths << QLatin1String( "/usr/lib" );
        paths << QLatin1String( "/usr/local/lib" );
#ifndef Q_OS_WIN
        paths += envDirList( "LD_LIBRARY_PATH" );
#endif
        return paths;
    }

    QString findPluginLib( const Soprano::SopranoPluginFile& file )
    {
        QStringList libs = makeLibName( file.library() );
        Q_FOREACH( const QString &lib, libs ) {
            if ( lib.startsWith( '/' ) ) {
                return lib;
            }
            else {
                QStringList dirs( libDirs() );
                dirs.prepend( file.fileName().section( "/", 0, -5, QString::SectionIncludeTrailingSep ) + "lib" );

                foreach( QString dir, dirs ) {
                    if ( QFile::exists( dir + SOPRANO_LIB_SUFFIX"/soprano/" + lib ) ) {
                        return dir + SOPRANO_LIB_SUFFIX"/soprano/" + lib;
                    }
                    else if ( QFile::exists( dir + "/soprano/" + lib ) ) {
                        return dir + "/soprano/" + lib;
                    }
                    else if ( QFile::exists( dir + "64/soprano/" + lib ) ) {
                        return dir + "64/soprano/" + lib;
                    }
                }
            }
        }
        return QString();
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


bool Soprano::PluginManager::backendAvailable( const QString& name )
{
    loadAllPlugins();
    return d->backends.contains( name );
}


const Soprano::Backend* Soprano::PluginManager::discoverBackendByName( const QString& name )
{
    loadAllPlugins();
    QHash<QString, PluginStub>::iterator it = d->backends.find( name );
    if( it != d->backends.end() )
        return dynamic_cast<Backend*>( it->plugin() );
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


bool Soprano::PluginManager::parserAvailable( const QString& name )
{
    loadAllPlugins();
    return d->parsers.contains( name );
}


const Soprano::Parser* Soprano::PluginManager::discoverParserByName( const QString& name )
{
    loadAllPlugins();
    QHash<QString, PluginStub>::iterator it = d->parsers.find( name );
    if( it != d->parsers.end() )
        return dynamic_cast<Parser*>( it->plugin() );
    else
        return 0;
}


const Soprano::Parser2* Soprano::PluginManager::discoverParser2ByName( const QString& name )
{
    loadAllPlugins();
    QHash<QString, PluginStub>::iterator it = d->parsers.find( name );
    if( it != d->parsers.end() )
        return dynamic_cast<Parser2*>( it->plugin() );
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


const Soprano::Parser2* Soprano::PluginManager::discoverParser2ForSerialization( RdfSerialization serialization, const QString& userSerialization )
{
    loadAllPlugins();
    for( QHash<QString, PluginStub>::iterator it = d->parsers.begin(); it != d->parsers.end(); ++it ) {
        if( const Parser2* p = dynamic_cast<Parser2*>( it->plugin() ) ) {
            if( p->supportsSerialization( serialization, userSerialization ) ) {
                return p;
            }
        }
    }
    return 0;
}


bool Soprano::PluginManager::serializerAvailable( const QString& name )
{
    loadAllPlugins();
    return d->serializers.contains( name );
}


const Soprano::Serializer* Soprano::PluginManager::discoverSerializerByName( const QString& name )
{
    loadAllPlugins();
    QHash<QString, PluginStub>::iterator it = d->serializers.find( name );
    if( it != d->serializers.end() )
        return dynamic_cast<Serializer*>( it->plugin() );
    else
        return 0;
}


const Soprano::Serializer2* Soprano::PluginManager::discoverSerializer2ByName( const QString& name )
{
    loadAllPlugins();
    QHash<QString, PluginStub>::iterator it = d->serializers.find( name );
    if( it != d->serializers.end() )
        return dynamic_cast<Serializer2*>( it->plugin() );
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


const Soprano::Serializer2* Soprano::PluginManager::discoverSerializer2ForSerialization( RdfSerialization serialization, const QString& userSerialization )
{
    loadAllPlugins();
    for( QHash<QString, PluginStub>::iterator it = d->serializers.begin(); it != d->serializers.end(); ++it ) {
        if( const Serializer2* p = dynamic_cast<Serializer2*>( it->plugin() ) ) {
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


QList<const Soprano::Parser2*> Soprano::PluginManager::allParser2s()
{
    loadAllPlugins();
    QList<const Parser2*> pl;
    for ( QHash<QString, PluginStub>::iterator it = d->parsers.begin();
          it != d->parsers.end(); ++it ) {
        if( const Parser2* p = dynamic_cast<Parser2*>( it->plugin() ) ) {
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


QList<const Soprano::Serializer2*> Soprano::PluginManager::allSerializer2s()
{
    loadAllPlugins();
    QList<const Serializer2*> pl;
    for ( QHash<QString, PluginStub>::iterator it = d->serializers.begin();
          it != d->serializers.end(); ++it ) {
        if( const Serializer2* p = dynamic_cast<Serializer2*>( it->plugin() ) ) {
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
            Q_FOREACH( QString plugin, pluginFiles ) {
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

                // try some backwards compatibility
                // as backends redland and sesame2 were always loaded without
                // the "backend" suffix
                if( name.endsWith( "backend" ) ) {
                    name.truncate( name.length() - 7 );
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
