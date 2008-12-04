/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2008 Sebastian Trueg <trueg@kde.org>
 *
 * Certain bits taken from iodbctest.c
 * Copyright (C) 1996-2006 by OpenLink Software <iodbc@openlinksw.com>
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

#ifndef _SOPRANO_IODBC_CONNECTION_H_
#define _SOPRANO_IODBC_CONNECTION_H_

#include "error.h"

namespace Soprano {

    class Node;
    class IODBCStatementHandler;

    class IODBCConnection : public Soprano::Error::ErrorCache
    {
    public:
        IODBCConnection();
        ~IODBCConnection();

        bool connect( const QString& cs );
        bool isConnected() const;
        void close();

        IODBCStatementHandler* execute( const QString& request );

    private:
        class Private;
        Private* const d;
    };
}

#endif