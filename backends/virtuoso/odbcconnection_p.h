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

#ifndef _SOPRANO_ODBC_CONNECTION_P_H_
#define _SOPRANO_ODBC_CONNECTION_P_H_

#include <sql.h>

#include <QtCore/QList>
#include <QtCore/QUrl>

#include "error.h"

namespace Soprano {
    namespace ODBC {

        class ConnectionPoolPrivate;
        class Environment;

        class ConnectionPrivate : public Error::ErrorCache
        {
        public:
            ConnectionPrivate()
                : m_env( 0 ),
                  m_hdbc( SQL_NULL_HANDLE ) {
            }

            Environment* m_env;
            HDBC m_hdbc;
            ConnectionPoolPrivate* m_pool;
            QList<QueryResult*> m_openResults;

            HSTMT execute( const QString& query );
            bool getCharData( HSTMT hstmt, int colNum, SQLCHAR** buffer, SQLLEN* length );
        };
    }
}

#endif
