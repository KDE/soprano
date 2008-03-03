/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2008 Sebastian Trueg <trueg@kde.org>
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

#ifndef _SOPRANO_SERVER_DBUS_ITERATOR_H_
#define _SOPRANO_SERVER_DBUS_ITERATOR_H_

#include <QtCore/QObject>
#include "error.h"
#include "soprano_export.h"

namespace Soprano {

    class StatementIterator;
    class NodeIterator;
    class QueryResultIterator;

    namespace Server {
        /**
         * \class DBusExportIterator dbusexportiterator.h Soprano/Server/DBusExportIterator
         *
         * \brief Exports a %Soprano Iterator via D-Bus.
         *
         * The DBusExportIterator can be used to export a single iterator via
         * D-Bus. This may be useful in case one does not want to expose
         * the complete Model API but restrict access via custom methods
         * that return %Soprano Iterators.
         *
         * The most common usage is probably as a fire-and-forget class:
         * \code
         * void myDbusMethod( const QDBusMessage& m ) {
         *    Soprano::StatementIterator it = model->listStatements();
         *    Soprano::Server::DBusExportIterator* dbusIt = new Soprano::Server::DBusExportIterator( it, this );
         *    dbusIt->setDeleteOnClose( true );
         *    dbusIt->registerIterator( myFancyDBusObjectPath, m.service() );
         *    return myFancyDBusObjectPath;
         * }
         * \endcode
         *
         * This is a class for advanced usage. In most situations using
         * DBusExportModel or even ServerCore::registerAsDBusObject
         * is probably enough.
         * 
         * \author Sebastian Trueg <trueg@kde.org>
         *
         * \sa \ref soprano_server_dbus
         *
         * \since 2.1
         */
        class SOPRANO_SERVER_EXPORT DBusExportIterator : public QObject, public Error::ErrorCache
        {
            Q_OBJECT
        
        public:
            /**
             * Create a new instance to export a StatementIterator
             */
            DBusExportIterator( StatementIterator it, QObject* parent = 0 );

            /**
             * Create a new instance to export a NodeIterator
             */
            DBusExportIterator( NodeIterator it, QObject* parent = 0 );

            /**
             * Create a new instance to export a QueryResultIterator
             */
            DBusExportIterator( QueryResultIterator it, QObject* parent = 0 );

            /**
             * Destructor
             */
            ~DBusExportIterator();

            /**
             * \return The used StatementIterator or an invalid one if
             * this DBusExportIterator does not export a StatementIterator.
             */
            StatementIterator statementIterator() const;

            /**
             * \return The used NodeIterator or an invalid one if
             * this DBusExportIterator does not export a NodeIterator.
             */
            NodeIterator nodeIterator() const;

            /**
             * \return The used QueryResultIterator or an invalid one if
             * this DBusExportIterator does not export a QueryResultIterator.
             */
            QueryResultIterator queryResultIterator() const;

            /**
             * The path this iterator is exported on.
             * This is an empty string if the iterator is not exported.
             */
            QString dbusObjectPath() const;

            /**
             * The DBusExportIterator instance can delete itself once the 
             * iterator is closed. The default value is false, i.e. 
             * the creator has to delete the instance.
             *
             * \sa setDeleteOnClose, Iterator::close
             */
            bool deleteOnClose() const;
            
        public Q_SLOTS:
            /**
             * Set if the iterator should delete itself
             * once it has been closed. The default value is false,
             * i.e. the creator has to delete the instance.
             *
             * \warning This only works if the iterator is closed
             * from the outside, i.e. by the D-Bus client.
             *
             * \sa deleteOnClose
             */
            void setDeleteOnClose( bool deleteOnClose );

            /**
             * Register the iterator under the given D-Bus object path.
             * \sa QDBusConnection::registerObject
             *
             * \param dbusObjectPath The D-Bus object path to register the iterator under.
             * \param dbusClient The optional D-Bus client which uses the iterator, i.e. which called
             * the method that triggered the creation of the iterator. If set, the iterator
             * will be closed once the client dies. In combination with setDeleteOnClose
             * this class can thus be used in a fire-and-forget manner.
             */
            bool registerIterator( const QString& dbusObjectPath, const QString& dbusClient = QString() );

            /**
             * Unregister the iterator from D-Bus.
             * \sa QDBusConnection::unregisterObject
             */
            void unregisterIterator();

        private:
            class Private;
            Private* const d;

            Q_PRIVATE_SLOT( d, void slotServiceOwnerChanged( const QString& name, const QString&, const QString& ) )
        };
    }
}

#endif
