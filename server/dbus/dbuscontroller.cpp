/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2009 Thomas Riccardi <riccardi.thomas@gmail.com>
 * Copyright (C) 2009 Sebastian Trueg <trueg@kde.org>
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

#include "dbuscontroller.h"
#include "../servercore.h"
#include "dbusserveradaptor.h"


Soprano::Server::DBusController::DBusController( ServerCore* parent, const QString& dbusObjectPath )
    : QThread( parent )
{
    m_core = parent;
    m_dbusObjectPath = dbusObjectPath;

    start();
}

Soprano::Server::DBusController::~DBusController()
{
    quit();
    wait();
}


void Soprano::Server::DBusController::run()
{
    // we need a dummy object to register AND delete the adaptor within the thread
    QObject* dummy = new QObject();

    ( void )new Soprano::Server::DBusServerAdaptor( dummy, m_core, m_dbusObjectPath );

    QDBusConnection::sessionBus().registerObject( m_dbusObjectPath, dummy );

    // start the event loop
    exec();

    delete dummy;
}

