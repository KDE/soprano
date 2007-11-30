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

#ifndef _SOPRANO_SERVER_DBUS_STATEMENTITERATOR_ADAPTOR_H_
#define _SOPRANO_SERVER_DBUS_STATEMENTITERATOR_ADAPTOR_H_

#include <QtDBus/QtDBus>

namespace Soprano {

    class Statement;

    namespace Server {

    class IteratorWrapper;

    class DBusStatementIteratorAdaptor : public QDBusAbstractAdaptor
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", "org.soprano.StatementIterator")
        Q_CLASSINFO("D-Bus Introspection", ""
            "  <interface name=\"org.soprano.StatementIterator\" >\n"
            "    <method name=\"next\" >\n"
            "      <arg direction=\"out\" type=\"b\" name=\"reply\" />\n"
            "    </method>\n"
            "    <method name=\"current\" >\n"
            "      <arg direction=\"out\" type=\"((isss)(isss)(isss)(isss))\" name=\"statement\" />\n"
            "      <annotation value=\"Soprano::Statement\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\" />\n"
            "    </method>\n"
            "    <method name=\"close\" />\n"
            "  </interface>\n"
            "")

    public:
        DBusStatementIteratorAdaptor( IteratorWrapper* parent );
        virtual ~DBusStatementIteratorAdaptor();

    public Q_SLOTS:
        Soprano::Statement current( const QDBusMessage& m );
        bool next( const QDBusMessage& m );
        void close( const QDBusMessage& m );

    private:
        IteratorWrapper* m_iteratorWrapper;
    };
    }
}

#endif
