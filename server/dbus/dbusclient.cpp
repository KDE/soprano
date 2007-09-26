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

#include "dbusclient.h"
#include "dbusserverinterface.h"
#include "dbusutil.h"
#include "dbusmodel.h"
#include "dbusoperators.h"


class Soprano::Client::DBusClient::Private
{
public:
    DBusServerInterface* interface;
};

Soprano::Client::DBusClient::DBusClient( const QString& servicename, QObject* parent )
    : QObject( parent ),
      d( new Private() )
{
    qDBusRegisterMetaType<Soprano::Node>();
    qDBusRegisterMetaType<Soprano::Statement>();
    qDBusRegisterMetaType<Soprano::BindingSet>();

    d->interface = new DBusServerInterface( servicename.isEmpty() ? QString( "org.soprano.Server" ) : servicename,
                                            "/org/soprano/Server",
                                            QDBusConnection::sessionBus(),
                                            this );
}


Soprano::Client::DBusClient::~DBusClient()
{
    delete d;
}


bool Soprano::Client::DBusClient::isValid() const
{
    return d->interface->isValid();
}


QStringList Soprano::Client::DBusClient::allModels() const
{
    QDBusReply<QStringList> reply = d->interface->allModels();
    setError( DBus::convertError( reply.error() ) );
    return reply.value();
}


Soprano::Model* Soprano::Client::DBusClient::createModel( const QString& name, const BackendSettings& settings )
{
    Q_UNUSED( settings );
    QDBusReply<QString> reply = d->interface->createModel( name );
    setError( DBus::convertError( reply.error() ) );

    if ( reply.isValid() ) {
        return new DBusModel( d->interface->service(), reply.value() );
    }
    else {
        return 0;
    }
}

#include "dbusclient.moc"
