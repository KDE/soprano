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

#ifndef _SOPRANO_ODBC_QUERY_RESULT_H_
#define _SOPRANO_ODBC_QUERY_RESULT_H_

#include <QtCore/qglobal.h>
#include "error.h"
#ifdef Q_OS_WIN
#include <windows.h>
#endif
#include <sql.h>

namespace Soprano {

    class Node;

    namespace ODBC {

        class QueryResultPrivate;
        class Connection;

        class QueryResult : public Error::ErrorCache
        {
        public:
            ~QueryResult();

            QStringList resultColumns();
            bool fetchRow();
            Node getData( int colNum );

            /// Check if the data in column colNum is a BLOB. used in Virtuoso::QueryResultIteratorBackend
            bool isBlob( int colNum );

        private:
            QueryResult();

            bool getCharData( int colNum, SQLCHAR** buffer, SQLLEN* length );

            QueryResultPrivate* const d;

            friend class Connection;
        };
    }
}

#endif

