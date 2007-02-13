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

#include <QHash>
#include <QDir>
#include <QPluginLoader>
#include <QDebug>

#include <stdlib.h>


class Soprano::PluginManager::Private
{
public:
  QHash<QString, Backend*> backends;
};


const Soprano::Backend* Soprano::PluginManager::discoverBackendByName( const QString& name )
{
  QHash<QString, Backend*>::iterator it = d->backends.find( name );
  if( it != d->backends.end() )
    return *it;
  else
    return 0;
}


const Soprano::Backend* Soprano::PluginManager::discoverBackendByFeatures( const QStringList& features )
{
  for( QHash<QString, Backend*>::const_iterator it = d->backends.begin(); it != d->backends.end(); ++it ) {
    const Backend* b = *it;
    if( b->hasFeatures( features ) )
      return b;
  }
  return 0;    
}


Soprano::PluginManager::PluginManager( QObject* parent )
  : QObject( parent )
{
  d = new Private;
}


Soprano::PluginManager::~PluginManager()
{
  // delete all plugins (is this necessary?)
  for( QHash<QString, Backend*>::const_iterator it = d->backends.begin(); it != d->backends.end(); ++it )
    delete it.value();
  delete d;
}


void Soprano::PluginManager::loadAllPlugins()
{
  qDebug() << "(Soprano::PluginManager) loading all plugins" << endl;

  // FIXME: we need a way to define where plugins are installed (something like <prefix>/lib/soprano/)
  // or a generic way to determine all the search paths.
  QStringList libDirs;
  libDirs << "/usr/lib" << "/usr/lib64" << "/usr/local/lib" << "/usr/local/lib64";

  char* libPath = getenv( "LD_LIBRARY_PATH" );
  if( libPath ) {
    libDirs += QString::fromLocal8Bit( libPath ).split(":");
  }

  for( QStringList::const_iterator it = libDirs.begin(); it != libDirs.end(); ++it ) {
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
      Backend* backend = qobject_cast<Backend*>( plugin );
      if( backend ) {
	qDebug() << "(Soprano::PluginManager) found plugin " << backend->backendName() << endl;
	d->backends.insert( backend->backendName(), backend );	
      }
      else {
	qDebug() << "(Soprano::PluginManager) found no backend plugin at " << loader.fileName() << endl;
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
  if( !s_instance ) {
    s_instance = new PluginManager();
    s_instance->loadAllPlugins();
  }
  return s_instance;
}

#include "pluginmanager.moc"
