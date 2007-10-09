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

#include "dbusmodelinterface.h"

// we need a big timeout for methods such as query which can take a rather long time
// let's set it to the aweful big value of 10 minutes! :)
static const int bigTimeout = 600000;

Soprano::Client::DBusModelInterface::DBusModelInterface( const QString& service, const QString& path, const QDBusConnection& connection, QObject* parent )
    : QDBusAbstractInterface( service, path, "org.soprano.Model", connection, parent )
{
}

Soprano::Client::DBusModelInterface::~DBusModelInterface()
{
}


QDBusMessage Soprano::Client::DBusModelInterface::callWithArgumentListAndBigTimeout( QDBus::CallMode mode,
                                                                                     const QString& method,
                                                                                     const QList<QVariant>& args )
{
    QDBusMessage msg = QDBusMessage::createMethodCall( service(), path(), interface(), method );
    msg.setArguments( args );

    QDBusMessage reply = connection().call( msg, mode, bigTimeout );

    // ensure that there is at least one element
    if (reply.arguments().isEmpty())
        reply << QVariant();

    return reply;
}

#include "dbusmodelinterface.moc"
