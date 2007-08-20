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
    // check all settings.
    // we pass all user settings on to librdf
    QHash<QString, QString> redlandOptions;

    // set some defaults
    redlandOptions["contexts"] = "yes";
    redlandOptions["storageType"] = "hashes";
    redlandOptions["hash-type"] = "memory";

    Q_FOREACH( BackendSetting s, settings ) {
        if ( s.option == BACKEND_OPTION_USER ) {
            redlandOptions[s.userOptionName] = s.value;
        }
        else if ( s.option == BACKEND_OPTION_STORAGE_MEMORY ) {
            redlandOptions["hash-type"] = "memory";
        }
        else if ( s.option == BACKEND_OPTION_STORAGE_DIR ) {
            redlandOptions["dir"] = s.value;
        }
    }

    // remove unused options from the option hash
    QString storageType = redlandOptions["storageType"];
    redlandOptions.remove( "storageType" );

    QString os = createRedlandOptionString( redlandOptions );

    // FIXME: does this always work without a name
    qDebug() << "(Soprano::Redland::BackendPlugin) creating model of type " << storageType << " with options " << os << endl;

    // create a new storage
    librdf_storage* storage = librdf_new_storage( World::self()->worldPtr(),
                                                  storageType.toLatin1().data(),
                                                  0,
                                                  os.toLatin1().data() );
    if( !storage ) {
        qDebug() << "(Soprano::Redland) storage creation failed!" << endl;
        return 0;
    }

    librdf_model *model = librdf_new_model( World::self()->worldPtr(), storage, 0 );
    if ( !model ) {
        librdf_free_storage( storage );
        return 0;
    }

    return new RedlandModel( this, model, storage );
}


Soprano::BackendFeatures Soprano::Redland::BackendPlugin::supportedFeatures() const
{
    return(  BACKEND_FEATURE_MEMORY_STORAGE|
             BACKEND_FEATURE_ADD_STATEMENT|
             BACKEND_FEATURE_REMOVE_STATEMENTS|
             BACKEND_FEATURE_LIST_STATEMENTS|
             BACKEND_FEATURE_QUERY|
             BACKEND_FEATURE_CONTEXTS );
}

#include "redlandbackend.moc"
