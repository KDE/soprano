/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2008 Sebastian Trueg <trueg@kde.org>
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

#include "dbusexportmodel.h"
#include "dbusmodeladaptor.h"

#include <QtDBus/QDBusConnection>


class Soprano::Server::DBusExportModel::Private
{
public:
    QString dbusPath;
};


Soprano::Server::DBusExportModel::DBusExportModel( Model* model )
    : FilterModel( model ),
      d( new Private() )
{
    ( void )new DBusModelAdaptor( this );
}


Soprano::Server::DBusExportModel::~DBusExportModel()
{
    delete d;
}


bool Soprano::Server::DBusExportModel::registerModel( const QString& dbusObjectPath )
{
    if( QDBusConnection::sessionBus().registerObject( dbusObjectPath, this ) ) {
        d->dbusPath = dbusObjectPath;
        return true;
    }
    else {
        d->dbusPath = QString();
        return false;
    }
}


void Soprano::Server::DBusExportModel::unregisterModel()
{
    d->dbusPath = QString();
    QDBusConnection::sessionBus().unregisterObject( d->dbusPath );
}


QString Soprano::Server::DBusExportModel::dbusObjectPath() const
{
    return d->dbusPath;
}

#include "dbusexportmodel.moc"
