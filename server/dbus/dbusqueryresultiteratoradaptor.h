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

#ifndef _SOPRANO_SERVER_DBUS_QUERYRESULTITERATOR_ADAPTOR_H_
#define _SOPRANO_SERVER_DBUS_QUERYRESULTITERATOR_ADAPTOR_H_

#include <QtDBus/QtDBus>

namespace Soprano {

    class BindingSet;
    class Node;
    class Statement;

    namespace Server {

        class DBusExportIterator;

        class DBusQueryResultIteratorAdaptor : public QDBusAbstractAdaptor
        {
            Q_OBJECT
            Q_CLASSINFO("D-Bus Interface", "org.soprano.QueryResultIterator")
            Q_CLASSINFO("D-Bus Introspection", ""
                        "  <interface name=\"org.soprano.QueryResultIterator\" >\n"
                        "    <method name=\"next\" >\n"
                        "      <arg direction=\"out\" type=\"b\" name=\"reply\" />\n"
                        "    </method>\n"
                        "    <method name=\"current\" >\n"
                        "      <arg direction=\"out\" type=\"a{s(isss)}\" name=\"node\" />\n"
                        "      <annotation value=\"Soprano::BindingSet\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\" />\n"
                        "    </method>\n"
                        "    <method name=\"close\" />\n"
                        "    <method name=\"currentStatement\" >\n"
                        "      <arg direction=\"out\" type=\"((isss)(isss)(isss)(isss))\" name=\"statement\" />\n"
                        "      <annotation value=\"Soprano::Statement\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\" />\n"
                        "    </method>\n"
                        "    <method name=\"bindingByName\" >\n"
                        "      <arg direction=\"in\" type=\"s\" name=\"name\" />\n"
                        "      <arg direction=\"out\" type=\"(isss)\" name=\"node\" />\n"
                        "      <annotation value=\"Soprano::Node\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\" />\n"
                        "    </method>\n"
                        "    <method name=\"bindingByIndex\" >\n"
                        "      <arg direction=\"in\" type=\"i\" name=\"index\" />\n"
                        "      <arg direction=\"out\" type=\"(isss)\" name=\"node\" />\n"
                        "      <annotation value=\"Soprano::Node\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\" />\n"
                        "    </method>\n"
                        "    <method name=\"bindingCount\" >\n"
                        "      <arg direction=\"out\" type=\"i\" name=\"names\" />\n"
                        "    </method>\n"
                        "    <method name=\"bindingNames\" >\n"
                        "      <arg direction=\"out\" type=\"as\" name=\"names\" />\n"
                        "    </method>\n"
                        "    <method name=\"boolValue\" >\n"
                        "      <arg direction=\"out\" type=\"b\" name=\"reply\" />\n"
                        "    </method>\n"
                        "    <method name=\"isGraph\" >\n"
                        "      <arg direction=\"out\" type=\"b\" name=\"reply\" />\n"
                        "    </method>\n"
                        "    <method name=\"isBinding\" >\n"
                        "      <arg direction=\"out\" type=\"b\" name=\"reply\" />\n"
                        "    </method>\n"
                        "    <method name=\"isBool\" >\n"
                        "      <arg direction=\"out\" type=\"b\" name=\"reply\" />\n"
                        "    </method>\n"
                        "  </interface>\n")

        public:
            DBusQueryResultIteratorAdaptor( DBusExportIterator* parent );
            virtual ~DBusQueryResultIteratorAdaptor();

        public Q_SLOTS:
            Soprano::BindingSet current( const QDBusMessage& m );
            bool next( const QDBusMessage& m );
            void close( const QDBusMessage& m );
            Soprano::Statement currentStatement( const QDBusMessage& m );
            Soprano::Node bindingByIndex( int index, const QDBusMessage& m );
            Soprano::Node bindingByName( const QString& name, const QDBusMessage& m );
            int bindingCount( const QDBusMessage& m );
            QStringList bindingNames( const QDBusMessage& m );
            bool boolValue( const QDBusMessage& m );
            bool isBinding( const QDBusMessage& m );
            bool isBool( const QDBusMessage& m );
            bool isGraph( const QDBusMessage& m );

        private:
            DBusExportIterator* m_iteratorWrapper;
        };
    }
}

#endif
