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

#ifndef _SOPRANO_SERVER_DBUS_MODEL_ADAPTOR_H_
#define _SOPRANO_SERVER_DBUS_MODEL_ADAPTOR_H_

#include <QtDBus/QtDBus>

namespace Soprano {

    class Statement;
    class Node;
    class Model;

    namespace Server {
        class DBusModelAdaptor: public QDBusAbstractAdaptor
        {
            Q_OBJECT
            Q_CLASSINFO("D-Bus Interface", "org.soprano.Model")
            Q_CLASSINFO("D-Bus Introspection", ""
                        "  <interface name=\"org.soprano.Model\" >\n"
                        "    <method name=\"addStatement\" >\n"
                        "      <arg direction=\"in\" type=\"((isss)(isss)(isss)(isss))\" name=\"statement\" />\n"
                        "      <arg direction=\"out\" type=\"i\" name=\"errorCode\" />\n"
                        "      <annotation value=\"Soprano::Statement\" name=\"com.trolltech.QtDBus.QtTypeName.In0\" />\n"
                        "    </method>\n"
                        "    <method name=\"removeStatement\" >\n"
                        "      <arg direction=\"in\" type=\"((isss)(isss)(isss)(isss))\" name=\"statement\" />\n"
                        "      <arg direction=\"out\" type=\"i\" name=\"errorCode\" />\n"
                        "      <annotation value=\"Soprano::Statement\" name=\"com.trolltech.QtDBus.QtTypeName.In0\" />\n"
                        "    </method>\n"
                        "    <method name=\"removeAllStatements\" >\n"
                        "      <arg direction=\"in\" type=\"((isss)(isss)(isss)(isss))\" name=\"statement\" />\n"
                        "      <arg direction=\"out\" type=\"i\" name=\"errorCode\" />\n"
                        "      <annotation value=\"Soprano::Statement\" name=\"com.trolltech.QtDBus.QtTypeName.In0\" />\n"
                        "    </method>\n"
                        "    <method name=\"containsStatement\" >\n"
                        "      <arg direction=\"in\" type=\"((isss)(isss)(isss)(isss))\" name=\"statement\" />\n"
                        "      <arg direction=\"out\" type=\"b\" name=\"reply\" />\n"
                        "      <annotation value=\"Soprano::Statement\" name=\"com.trolltech.QtDBus.QtTypeName.In0\" />\n"
                        "    </method>\n"
                        "    <method name=\"containsAnyStatement\" >\n"
                        "      <arg direction=\"in\" type=\"((isss)(isss)(isss)(isss))\" name=\"statement\" />\n"
                        "      <arg direction=\"out\" type=\"b\" name=\"reply\" />\n"
                        "      <annotation value=\"Soprano::Statement\" name=\"com.trolltech.QtDBus.QtTypeName.In0\" />\n"
                        "    </method>\n"
                        "    <method name=\"listStatements\" >\n"
                        "      <arg direction=\"in\" type=\"((isss)(isss)(isss)(isss))\" name=\"statement\" />\n"
                        "      <arg direction=\"out\" type=\"s\" name=\"iterator\" />\n"
                        "      <annotation value=\"Soprano::Statement\" name=\"com.trolltech.QtDBus.QtTypeName.In0\" />\n"
                        "    </method>\n"
                        "    <method name=\"listContexts\" >\n"
                        "      <arg direction=\"out\" type=\"s\" name=\"iterator\" />\n"
                        "    </method>\n"
                        "    <method name=\"statementCount\" >\n"
                        "      <arg direction=\"out\" type=\"i\" name=\"count\" />\n"
                        "    </method>\n"
                        "    <method name=\"isEmpty\" >\n"
                        "      <arg direction=\"out\" type=\"b\" name=\"reply\" />\n"
                        "    </method>\n"
                        "    <method name=\"executeQuery\" >\n"
                        "      <arg direction=\"in\" type=\"s\" name=\"query\" />\n"
                        "      <arg direction=\"in\" type=\"s\" name=\"queryLang\" />\n"
                        "      <arg direction=\"out\" type=\"s\" name=\"iterator\" />\n"
                        "    </method>\n"
                        "    <method name=\"createBlankNode\" >\n"
                        "      <arg direction=\"out\" type=\"(isss)\" name=\"node\" />\n"
                        "      <annotation value=\"Soprano::Node\" name=\"com.trolltech.QtDBus.QtTypeName.Out0\" />\n"
                        "    </method>\n"
                        "    <signal name=\"statementsAdded\" />\n"
                        "    <signal name=\"statementsRemoved\" />\n"
                        "    <signal name=\"statementAdded\">\n"
                        "      <arg name=\"statement\" type=\"((isss)(isss)(isss)(isss))\" />\n"
                        "      <annotation name=\"com.trolltech.QtDBus.QtTypeName.In0\" value=\"Soprano::Statement\" />\n"
                        "    </signal>\n"
                        "    <signal name=\"statementRemoved\">\n"
                        "      <arg name=\"statement\" type=\"((isss)(isss)(isss)(isss))\" />\n"
                        "      <annotation name=\"com.trolltech.QtDBus.QtTypeName.In0\" value=\"Soprano::Statement\" />\n"
                        "    </signal>\n"
                        "  </interface>\n")

        public:
            DBusModelAdaptor( Model* model, QObject* parent, const QString& dbusObjectPath );
            virtual ~DBusModelAdaptor();

        public Q_SLOTS:
            int addStatement( const Soprano::Statement& statement, const QDBusMessage& m );
            bool containsAnyStatement( const Soprano::Statement& statement, const QDBusMessage& m );
            bool containsStatement( const Soprano::Statement& statement, const QDBusMessage& m );
            Soprano::Node createBlankNode( const QDBusMessage& m );
            QString executeQuery( const QString &query, const QString& queryLang, const QDBusMessage& m );
            bool isEmpty( const QDBusMessage& m );
            QString listContexts( const QDBusMessage& m );
            QString listStatements( const Soprano::Statement& statement, const QDBusMessage& m );
            int removeAllStatements( const Soprano::Statement& statement, const QDBusMessage& m );
            int removeStatement( const Soprano::Statement& statement, const QDBusMessage& m );
            int statementCount( const QDBusMessage& m );

        Q_SIGNALS:
            void statementsAdded();
            void statementsRemoved();
            void statementAdded( const Soprano::Statement& statement );
            void statementRemoved( const Soprano::Statement& statement );

        private Q_SLOTS:
            void slotServiceOwnerChanged( const QString & name, const QString & oldOwner, const QString & newOwner );

        private:
            class Private;
            Private* const d;
        };
    }
}

#endif
