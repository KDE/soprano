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

#ifndef _SOPRANO_SERVER_DBUS_SERVER_H_
#define _SOPRANO_SERVER_DBUS_SERVER_H_

#include <QtDBus/QtDBus>

#include <soprano/backend.h>

namespace Soprano {

    class Model;

    namespace Server {

    class ServerCore;

    class DBusServerAdaptor : public QDBusAbstractAdaptor
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", "org.soprano.Server")
        Q_CLASSINFO("D-Bus Introspection", ""
                    "  <interface name=\"org.soprano.Server\" >\n"
                    "    <method name=\"createModel\" >\n"
                    "      <arg direction=\"in\" type=\"s\" name=\"name\" />\n"
                    "      <arg direction=\"out\" type=\"s\" name=\"model\" />\n"
                    "    </method>\n"
                    "    <method name=\"removeModel\" >\n"
                    "      <arg direction=\"in\" type=\"s\" name=\"name\" />\n"
                    "    </method>\n"
                    "    <method name=\"allModels\" >\n"
                    "      <arg direction=\"out\" type=\"as\" name=\"models\" />\n"
                    "    </method>\n"
                    "  </interface>\n")

    public:
        DBusServerAdaptor( ServerCore* core, const QString& dbusObjectPath );
        virtual ~DBusServerAdaptor();

    public Q_SLOTS:
        QStringList allModels( const QDBusMessage& );
        QString createModel( const QString &name, const QDBusMessage& );
        void removeModel( const QString& name, const QDBusMessage& );

    private:
        class Private;
        Private* const d;
    };
    }
}

#endif
