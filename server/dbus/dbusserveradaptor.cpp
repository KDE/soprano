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

#include "dbusserveradaptor.h"
#include "dbusutil.h"
#include "../servercore.h"
#include "dbusmodeladaptor.h"

#include <soprano/model.h>

#include <QtCore/QHash>

class Soprano::Server::DBusServerAdaptor::Private
{
public:
    ServerCore* core;
    QHash<Model*, QString> modelDBusObjectPaths;
};


Soprano::Server::DBusServerAdaptor::DBusServerAdaptor( ServerCore* parent )
    : QDBusAbstractAdaptor( parent ),
      d( new Private() )
{
    d->core = parent;
}

Soprano::Server::DBusServerAdaptor::~DBusServerAdaptor()
{
    delete d;
}

QStringList Soprano::Server::DBusServerAdaptor::allModels( const QDBusMessage& m )
{
    // handle method call org.soprano.Server.allModels
    QStringList names = d->core->allModels();
    if ( d->core->lastError() ) {
        DBus::sendErrorReply( m, d->core->lastError() );
    }
    return names;
}

QString Soprano::Server::DBusServerAdaptor::createModel( const QString& name, const QDBusMessage& m )
{
    // handle method call org.soprano.Server.createModel
//     if ( !settings.isEmpty() ) {
//         DBus::sendErrorReply( m, Error::Error( "Not settings supported yet" ) );
//         return QString();
//     }

    Model* model = d->core->model( name );
    if ( model ) {
        QHash<Model*, QString>::const_iterator it = d->modelDBusObjectPaths.find( model );
        if ( it != d->modelDBusObjectPaths.constEnd() ) {
            return it.value();
        }
        else {
            ( void )new DBusModelAdaptor( model );
            QString objectPath = "/Model_" + name;
            QDBusConnection::sessionBus().registerObject( objectPath, model );
            d->modelDBusObjectPaths.insert( model, objectPath );
            return objectPath;
        }
    }
    else {
        DBus::sendErrorReply( m, d->core->lastError() );
        return QString();
    }
}

#include "dbusserveradaptor.moc"
