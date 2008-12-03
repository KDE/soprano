/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2008 Sebastian Trueg <trueg@kde.org>
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

#ifndef _SOPRANO_SERVER_DBUS_CLIENT_TRANSACTION_H_
#define _SOPRANO_SERVER_DBUS_CLIENT_TRANSACTION_H_

#include "transaction.h"

namespace Soprano {
    namespace Client {

        class DBusModel;

        /**
         * \class DBusTransaction dbustransaction.h Soprano/Client/DBusTransaction
         *
         * \brief Interface to a DBus server transaction.
         *
         * The DBusTransaction provides a fully functional interface
         * to a Transaction provided through a %Soprano DBus server. It can thus,
         * be seen as the counterpart to Server::DBusExportModel.
         *
         * Given the DBus service name and the object path of the Model
         * in question it handles all DBus communication transparently.
         *
         * Normally there is no need to use this class directly as 
         * DBusClient does create instances of it on request.
         *
         * \warning DBusModel is not thread-safe by default
         * and has to be protected if is is to be used from different
         * threads (This can be done via Soprano::Util::MutexModel
         * in Soprano::Util::MutexModel::ReadWriteMultiThreading mode
         * or via setting the model to async mode via setAsyncCalls()).
         *
         * \author Sebastian Trueg <trueg@kde.org>
         *
         * \sa \ref soprano_server_dbus
         */
        class DBusTransaction : public Soprano::Transaction
        {
            Q_OBJECT

        public:
            /**
             * Create a new DBus Transaction interface.
             *
             * \param serviceName The name of the DBus service that provides the Transaction.
             * \param dbusObject The path to the Transaction object in the DBus service.
             */
            DBusTransaction( DBusModel* model, const QString& serviceName, const QString& dbusObject );
            ~DBusTransaction();

            /**
             * \sa DBusModel::setAsyncCalls
             */
            void setAsyncCalls( bool b );

            /**
             * \return \p true if asyncronous calls are enabled.
             */
            bool asyncCalls() const;

            Error::ErrorCode addStatement( const Statement &statement );
            NodeIterator listContexts() const;
            QueryResultIterator executeQuery( const QString& query, Query::QueryLanguage language, const QString& userQueryLanguage = QString() ) const;
            StatementIterator listStatements( const Statement &partial ) const;
            Error::ErrorCode removeStatement( const Statement &statement );
            Error::ErrorCode removeAllStatements( const Statement &statement );
            int statementCount() const;
            bool isEmpty() const;
            bool containsStatement( const Statement &statement ) const;
            bool containsAnyStatement( const Statement &statement ) const;
            Node createBlankNode();

        protected:
            Error::ErrorCode doRollback();
            Error::ErrorCode doCommit();

        private:
            class Private;
            Private* const d;
        };
    }
}

#endif
