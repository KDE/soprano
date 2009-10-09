/*
 * This file is part of Soprano Project
 *
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

#ifndef _SOPRANO_ODBC_CONNECTION_POOL_H_
#define _SOPRANO_ODBC_CONNECTION_POOL_H_

#include <QtCore/QObject>

#include "error.h"

namespace Soprano {
    namespace ODBC {

        class Connection;
        class ConnectionPoolPrivate;

        class ConnectionPool : public QObject, public Soprano::Error::ErrorCache
        {
            Q_OBJECT

        public:
            ConnectionPool( const QString& odbcConnectString, QObject* parent = 0 );
            ~ConnectionPool();

            /**
             * Get the connection for the current thread.
             */
            Connection* connection();

        private Q_SLOTS:
            void slotThreadFinished();

        private:
            ConnectionPoolPrivate* const d;
        };
    }
}

#endif
