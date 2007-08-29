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

#include "threestorebackend.h"
#include "threestorecore.h"
#include "threestoremodel.h"

#include <QtCore/QtPlugin>
#include <QtCore/QDebug>


Q_EXPORT_PLUGIN2(soprano_tstorebackend, Soprano::ThreeStore::BackendPlugin)

Soprano::ThreeStore::BackendPlugin::BackendPlugin()
  : QObject(),
    Backend( "3store" )
{
}


Soprano::StorageModel* Soprano::ThreeStore::BackendPlugin::createModel( const QList<BackendSetting>& settings ) const
{
    // we use the name as 3Store/SQL database name and parse the options for mysql connection data
    QString db, host,  user,  passwd;
    Q_FOREACH( BackendSetting s, settings ) {
        if ( s.option == BACKEND_OPTION_USER ) {
            if ( s.userOptionName == "db" )
                db = s.value.toString();
            else if ( s.userOptionName == "host" )
                host = s.value.toString();
            else if ( s.userOptionName == "user" )
                user = s.value.toString();
            else if ( s.userOptionName == "passwd" )
                passwd = s.value.toString();
            else {
                setError( QString( "Unsupported option: %1" ).arg( s.userOptionName ), Error::ERROR_INVALID_ARGUMENT );
                return 0;
            }
        }
        else {
            setError( QString( "Unsupported option: %1" ).arg( s.option ), Error::ERROR_INVALID_ARGUMENT );
            return 0;
        }
    }

    if ( db.isEmpty() ) {
        setError( QString( "No MySQL Datebase name specified." ), Error::ERROR_INVALID_ARGUMENT );
        return 0;
    }

    if ( host.isEmpty() ) {
        host = "localhost";
    }

    return ThreeStore::Core::instance()->createModel( this, host, db, user, passwd );
}


Soprano::BackendFeatures Soprano::ThreeStore::BackendPlugin::supportedFeatures() const
{
    return(  BACKEND_FEATURE_ADD_STATEMENT|
             BACKEND_FEATURE_LIST_STATEMENTS|
             BACKEND_FEATURE_QUERY|
             BACKEND_FEATURE_CONTEXTS );
}

#include "threestorebackend.moc"
