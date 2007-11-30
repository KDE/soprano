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
#include "modelwrapper.h"

#include <soprano/model.h>

#include <QtCore/QHash>
#include <QtCore/QRegExp>


class Soprano::Server::DBusServerAdaptor::Private
{
public:
    ServerCore* core;
    QHash<Model*, QString> modelDBusObjectPaths;

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
    Model* model = d->core->model( name );
    if ( model ) {
        QHash<Model*, QString>::const_iterator it = d->modelDBusObjectPaths.find( model );
        if ( it != d->modelDBusObjectPaths.constEnd() ) {
            return it.value();
        }
        else {
            // the ModelWrapper makes sure that the QObject hierachy lives in the same thread
            ModelWrapper* mw = new ModelWrapper( model );
            connect( model, SIGNAL( destroyed( QObject* ) ), mw, SLOT( deleteLater() ) );
            QString objectPath = d->dbusObjectPath + "/models/" + normalizeModelName( name );
            ( void )new DBusModelAdaptor( model, mw, objectPath );
            QDBusConnection::sessionBus().registerObject( objectPath, mw );
            d->modelDBusObjectPaths.insert( model, objectPath );
            return objectPath;
        }
    }
    else {
        DBus::sendErrorReply( m, d->core->lastError() );
        return QString();
    }
}


void Soprano::Server::DBusServerAdaptor::removeModel( const QString& name, const QDBusMessage& m )
{
    d->core->removeModel( name );
    if ( d->core->lastError() ) {
        DBus::sendErrorReply( m, d->core->lastError() );
    }
}

#include "dbusserveradaptor.moc"
