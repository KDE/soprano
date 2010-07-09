/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2008 Sebastian Trueg <trueg@kde.org>
 *
 * Based on RedlandModelFactory.cpp
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
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

#include "redlandbackend.h"

#include <redland.h>

#include "redlandworld.h"
#include "redlandmodel.h"

#include <QtCore/QtPlugin>
#include <QtCore/QDebug>
#include <QtCore/QHash>
#include <QtCore/QDir>
#include <QtCore/QMutexLocker>


Q_EXPORT_PLUGIN2(soprano_redlandbackend, Soprano::Redland::BackendPlugin)


static QString createRedlandOptionString( const QHash<QString, QString>& options )
{
  QStringList os;
  for( QHash<QString, QString>::const_iterator it = options.begin(); it != options.end(); ++it )
    os += QString("%1='%2'").arg( it.key() ).arg( it.value() );
  return os.join(",");
}



Soprano::Redland::BackendPlugin::BackendPlugin()
  : QObject(),
    Backend( "redland" )
{
}


Soprano::StorageModel* Soprano::Redland::BackendPlugin::createModel( const BackendSettings& settings ) const
{
    QMutexLocker lock( &m_mutex );

    clearError();

    // check all settings.
    // we pass all user settings on to librdf
    QHash<QString, QString> redlandOptions;

    // set some defaults
    redlandOptions["contexts"] = "yes";       // always use context
    redlandOptions["storageType"] = "hashes"; // default to hashes storage, so even the memory storage has an index
    redlandOptions["hash-type"] = "memory";   // default to the memory hash type

    // for persistent stores we need an indentifier
    redlandOptions["name"] = "soprano";

    Q_FOREACH( const BackendSetting &s, settings ) {
        if ( s.option() == BackendOptionUser ) {
            redlandOptions[s.userOptionName()] = s.value().toString();
        }
        else if ( s.option() == BackendOptionStorageMemory ) {
            if ( s.value().toBool() ) {
                redlandOptions["hash-type"] = "memory";
            }
            else {
                redlandOptions["hash-type"] = "bdb";
            }
        }
        else if ( s.option() == BackendOptionStorageDir ) {
            redlandOptions["dir"] = s.value().toString();
            redlandOptions["hash-type"] = "bdb";
        }
    }

    // create if nothing is there (stupid version)
    if ( redlandOptions.contains( "dir" ) &&
         !redlandOptions["dir"].isEmpty() &&
         QDir( redlandOptions["dir"] ).entryList( QDir::Files ).isEmpty() &&
        !redlandOptions.contains( "new" ) ) {
        redlandOptions["new"] = "yes";
    }

    // remove unused options from the option hash
    QString storageType = redlandOptions["storageType"];
    QString storageName = redlandOptions["name"];
    redlandOptions.remove( "storageType" );
    redlandOptions.remove( "name" );

    QString os = createRedlandOptionString( redlandOptions );

    qDebug() << "(Soprano::Redland::BackendPlugin) creating model of type" << storageType << "with options" << os;

//    World* world = new World();
    World* world = World::theWorld();
    // create a new storage
    librdf_storage* storage = librdf_new_storage( world->worldPtr(),
                                                  storageType.toUtf8().data(),
                                                  storageName.toUtf8().data(),
                                                  os.toUtf8().data() );
    if( !storage ) {
        qDebug() << "(Soprano::Redland) storage creation failed!";
        setError( world->lastError() );
//        delete world;
        return 0;
    }

    librdf_model *model = librdf_new_model( world->worldPtr(), storage, 0 );
    if ( !model ) {
        librdf_free_storage( storage );
        setError( world->lastError() );
//        delete world;
        return 0;
    }

    return new RedlandModel( this, model, storage, world );
}


bool Soprano::Redland::BackendPlugin::deleteModelData( const BackendSettings& settings ) const
{
    QString path;
    QString name = "soprano";
    Q_FOREACH( const BackendSetting &s, settings ) {
        if ( s.option() == BackendOptionUser &&
            s.userOptionName() == "name" ) {
            name = s.value().toString();
        }
        else if ( s.option() == BackendOptionStorageDir ) {
            path = s.value().toString();
            break;
        }
    }

    if ( path.isEmpty() ) {
        setError( "No storage path set.", Error::ErrorInvalidArgument );
        return false;
    }

    // FIXME: <name>-*.db is probably only valid for the bdb backend
    // is there a way to get the actual list of files?
    QDir dir( path );
    QStringList files = dir.entryList( QStringList() << ( name + "-*.db" ), QDir::Files );
    foreach( const QString &file, files ) {
        if ( !dir.remove( file ) ) {
            setError( "Failed to remove file '" + dir.filePath( file ), Error::ErrorUnknown );
            return false;
        }
    }

    clearError();
    return true;
}


Soprano::BackendFeatures Soprano::Redland::BackendPlugin::supportedFeatures() const
{
    return(  BackendFeatureStorageMemory|
             BackendFeatureAddStatement|
             BackendFeatureRemoveStatements|
             BackendFeatureListStatements|
             BackendFeatureQuery|
             BackendFeatureContext );
}

#include "redlandbackend.moc"
