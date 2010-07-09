/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2010 Sebastian Trueg <trueg@kde.org>
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

#ifndef _SOPRANO_SERVER_CORE_P_H_
#define _SOPRANO_SERVER_CORE_P_H_

#include "servercore.h"
#include "soprano-server-config.h"

#include "backend.h"

namespace Soprano {
    namespace Server {

        class DBusController;
        class ModelPool;
        class LocalServer;
        class TcpServer;
        class ServerConnection;

        class ServerCorePrivate
        {
        public:
            ServerCorePrivate()
                : maxConnectionCount( 0 ),
#ifdef BUILD_DBUS_SUPPORT
                  dbusController( 0 ),
#endif
                  tcpServer( 0 ),
                  socketServer( 0 )
            {}

            const Backend* backend;
            BackendSettings settings;

            int maxConnectionCount;

            QHash<QString, Model*> models;
            QList<ServerConnection*> connections;

#ifdef BUILD_DBUS_SUPPORT
            DBusController* dbusController;
#endif

            TcpServer* tcpServer;
            LocalServer* socketServer;

            // bridge between ServerCore and ServerConnection
            ModelPool* modelPool;

            ServerCore* q;

            BackendSettings createBackendSettings( const QString& name ) {
                BackendSettings newSettings = settings;
                if ( isOptionInSettings( newSettings, BackendOptionStorageDir ) ) {
                    BackendSetting& setting = settingInSettings( newSettings, BackendOptionStorageDir );
                    setting.setValue( setting.value().toString() + '/' + name );
                }
                return newSettings;
            }

            void addConnection( ServerConnection* connection );
        };
    }
}

#endif
