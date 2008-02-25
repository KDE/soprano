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
#include "dbusexportmodel.h"

#include "model.h"

#include <QtCore/QHash>
#include <QtCore/QRegExp>


class Soprano::Server::DBusServerAdaptor::Private
{
public:
    ServerCore* core;
    QHash<QString, DBusExportModel*> modelDBusObjectPaths;

    QString dbusObjectPath;
};


Soprano::Server::DBusServerAdaptor::DBusServerAdaptor( ServerCore* parent, const QString& dbusObjectPath )
    : QDBusAbstractAdaptor( parent ),
      d( new Private() )
{
    d->core = parent;
    d->dbusObjectPath = dbusObjectPath;
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


namespace {
    QString normalizeModelName( const QString& name )
    {
        QString n( name );
        n.replace( QRegExp( "[^\\d\\w]" ), "_" );
        return n;
    }
}


QString Soprano::Server::DBusServerAdaptor::createModel( const QString& name, const QDBusMessage& m )
{
    // handle method call org.soprano.Server.createModel
    QHash<QString, DBusExportModel*>::const_iterator it = d->modelDBusObjectPaths.find( name );
    if ( it != d->modelDBusObjectPaths.constEnd() ) {
        return it.value()->dbusObjectPath();
    }
    else {
        Model* model = d->core->model( name );
        if ( model ) {
            QString objectPath = d->dbusObjectPath + "/models/" + normalizeModelName( name );
            DBusExportModel* mw = new DBusExportModel( model );
            connect( model, SIGNAL( destroyed( QObject* ) ), mw, SLOT( deleteLater() ) );
            mw->registerModel( objectPath );
            d->modelDBusObjectPaths.insert( name, mw );
            return objectPath;
        }
        else {
            DBus::sendErrorReply( m, d->core->lastError() );
            return QString();
        }
    }
}


void Soprano::Server::DBusServerAdaptor::removeModel( const QString& name, const QDBusMessage& m )
{
    d->core->removeModel( name );
    d->modelDBusObjectPaths[name]->unregisterModel();
    delete d->modelDBusObjectPaths[name];
    d->modelDBusObjectPaths.remove( name );
    if ( d->core->lastError() ) {
        DBus::sendErrorReply( m, d->core->lastError() );
    }
}

#include "dbusserveradaptor.moc"
