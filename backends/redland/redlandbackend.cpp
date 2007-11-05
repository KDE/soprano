/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>
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


Q_EXPORT_PLUGIN2(soprano_redlandbackend, Soprano::Redland::BackendPlugin)


static QString createRedlandOptionString( const QHash<QString, QString>& options )
{
  QStringList os;
  for( QHash<QString, QString>::const_iterator it = options.begin(); it != options.end(); it++ )
    os += QString("%1='%2'").arg( it.key() ).arg( it.value() );
  return os.join(",");
}



Soprano::Redland::BackendPlugin::BackendPlugin()
  : QObject(),
    Backend( "redland" )
{
}


Soprano::StorageModel* Soprano::Redland::BackendPlugin::createModel( const QList<BackendSetting>& settings ) const
{
    clearError();

    // check all settings.
    // we pass all user settings on to librdf
    QHash<QString, QString> redlandOptions;

    // set some defaults
    redlandOptions["contexts"] = "yes";
    redlandOptions["storageType"] = "hashes";
    redlandOptions["hash-type"] = "memory";

    // for persistent stores we need an indentifier
    redlandOptions["name"] = "soprano";

    Q_FOREACH( BackendSetting s, settings ) {
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
    if ( !redlandOptions["dir"].isEmpty() &&
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

    qDebug() << "(Soprano::Redland::BackendPlugin) creating model of type " << storageType << " with options " << os;

    // create a new storage
    librdf_storage* storage = librdf_new_storage( World::self()->worldPtr(),
                                                  storageType.toUtf8().data(),
                                                  storageName.toUtf8().data(),
                                                  os.toUtf8().data() );
    if( !storage ) {
        qDebug() << "(Soprano::Redland) storage creation failed!";
        setError( Redland::World::self()->lastError() );
        return 0;
    }

    librdf_model *model = librdf_new_model( World::self()->worldPtr(), storage, 0 );
    if ( !model ) {
        librdf_free_storage( storage );
        setError( Redland::World::self()->lastError() );
        return 0;
    }

    return new RedlandModel( this, model, storage );
}


Soprano::BackendFeatures Soprano::Redland::BackendPlugin::supportedFeatures() const
{
    return(  BackendFeatureStorageMemory|
             BackendFeatureAddStatement|
             BackendFeatureRemoveStatementS|
             BackendFeatureListStatements|
             BackendFeatureQuery|
             BackendFeatureContext );
}

#include "redlandbackend.moc"
