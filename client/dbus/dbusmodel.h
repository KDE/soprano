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

#ifndef _SOPRANO_SERVER_DBUS_CLIENT_MODEL_H_
#define _SOPRANO_SERVER_DBUS_CLIENT_MODEL_H_

#include "storagemodel.h"
#include "soprano_export.h"

namespace Soprano {
    namespace Client {

        /**
         * \class DBusModel dbusmodel.h Soprano/Client/DBusModel
         *
         * \brief Interface to a DBus server model.
         *
         * The DBusModel provides a fully functional interface
         * to a Model provided through a %Soprano DBus server. It can thus,
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
        class SOPRANO_CLIENT_EXPORT DBusModel : public Soprano::StorageModel
        {
            Q_OBJECT

        public:
            /**
             * Create a new DBus Model interface.
             *
             * \param serviceName The name of the DBus service that provides the Model.
             * \param dbusObject The path to the Model object in the DBus service.
             * \param backend The backend that created this model. Should always be set to 0.
             */
            DBusModel( const QString& serviceName, const QString& dbusObject, const Backend* backend = 0 );
            ~DBusModel();

            /**
             * Configure DBusModel to use asyncronous calls over DBus.
             * With asyncronous calls DBusModel will enter local event
             * loops while waiting for the reply. This way multiple calls
             * can be performed interweaved. However, the application does 
             * not block, i.e. events (such as signals and slots) are handled
             * which might not be wanted.
             * If that is the case it is better to use Util::MutexModel
             * instead.
             *
             * \param b If true asyncronous calls are enabled.
             *
             * Per default asyncronous calls are disabled.
             *
             * \warning Asyncronous calls only work in combination with a QCoreApplication
             * instance.
             *
             * \sa QDBus::BlockWithGui
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

            using StorageModel::addStatement;
            using StorageModel::removeStatement;
            using StorageModel::removeAllStatements;
            using StorageModel::listStatements;
            using StorageModel::containsStatement;
            using StorageModel::containsAnyStatement;

        private:
            class Private;
            Private* const d;
        };
    }
}

#endif
