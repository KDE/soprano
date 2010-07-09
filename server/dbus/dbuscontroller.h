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

#ifndef _SOPRANO_SERVER_DBUS_CONTROLLER_H_
#define _SOPRANO_SERVER_DBUS_CONTROLLER_H_

#include <QtCore/QThread>

namespace Soprano {
    namespace Server {

        class ServerCore;

        /**
         * Starts listening to incoming DBus commands in a separate thread.
         */
        class DBusController : public QThread
        {
            Q_OBJECT

        public:
            DBusController( ServerCore* core, const QString& dbusObjectPath );
            ~DBusController();

        private:
            void run();

            ServerCore* m_core;
            QString m_dbusObjectPath;
        };
    }
}

#endif
