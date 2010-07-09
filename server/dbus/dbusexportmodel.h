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


#ifndef _SOPRANO_SERVER_DBUS_EXPORT_MODEL_H_
#define _SOPRANO_SERVER_DBUS_EXPORT_MODEL_H_

#include "filtermodel.h"
#include "soprano_export.h"

namespace Soprano {
    namespace Server {
        /**
         * \class DBusExportModel dbusexportmodel.h Soprano/Server/DBusExportModel
         *
         * \brief Exports a %Soprano Model via D-Bus.
         *
         * DBusExportModel is a FilterModel like any other. As such,
         * it can occure anywhere in a stack of models. However,
         * the model exported via D-Bus is actually the FilterModel::parentModel,
         * not the DBusExportModel itself. Thus, subclassing DBusExportModel
         * to modify the behaviour of methods called via D-Bus does not 
         * make sense. Instead stack the DBusExportModel on top of
         * your own custom FilterModel.
         *
         * For creating a simple %Soprano D-Bus server see 
         * ServerCore::registerAsDBusObject.
         *
         * The interface exported can be accessed via Client::DBusModel.
         *
         * DBusExportModel automatically makes use of a Util::AsyncModel as
         * parent model to create delayed D-Bus replies. If the parent model
         * is not a Util::AsyncModel all calls will be performed syncroneously.
         *
         * \author Sebastian Trueg <trueg@kde.org>
         *
         * \sa \ref soprano_server_dbus
         *
         * \since 2.1
         */
        class SOPRANO_SERVER_EXPORT DBusExportModel : public FilterModel
        {
            Q_OBJECT

        public:
            /**
             * Create a new D-Bus export model.
             * \param model The parent model which should be exported.
             */
            DBusExportModel( Model* model = 0 );

            /**
             * Destructor.
             */
            ~DBusExportModel();

            /**
             * Register the model under the given D-Bus object path.
             * \sa QDBusConnection::registerObject
             */
            bool registerModel( const QString& dbusObjectPath );

            /**
             * Unregister the model from D-Bus.
             * \sa QDBusConnection::unregisterObject
             */
            void unregisterModel();

            /**
             * The path this model is exported on.
             * This is an empty string if the model is not exported.
             */
            QString dbusObjectPath() const;

        private:
            class Private;
            Private* const d;
        };
    }
}

#endif
