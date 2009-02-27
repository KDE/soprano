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

#include "odbcenvironment.h"
#include "odbcenvironment_p.h"
#include "odbcconnection.h"
#include "odbcconnection_p.h"
#include "virtuosotools.h"

#include <sql.h>


Soprano::ODBC::Environment::Environment()
    : d( new EnvironmentPrivate(this) )
{
}


Soprano::ODBC::Environment::~Environment()
{
    qDeleteAll( d->m_openConnections );

    if ( d->m_henv ) {
        SQLFreeHandle( SQL_HANDLE_ENV, d->m_henv );
    }
    delete d;
}


Soprano::ODBC::Connection* Soprano::ODBC::Environment::createConnection( const QString& connectString )
{
    HDBC hdbc;

    // alloc hdbc handle
    if ( SQLAllocConnect( d->m_henv, &hdbc ) != SQL_SUCCESS ) {
        setError( "Failed to allocate SQL handle" );
        return 0;
    }

    // set the app name
//    SQLSetConnectAttr( d->m_hdbc, SQL_ATTR_AUTOCOMMIT, SQL_AUTOCOMMIT_ON, 0 );
//    SQLSetConnectOption( d->m_hdbc, SQL_APPLICATION_NAME, "Soprano" );
    SQLSetConnectOption( hdbc, SQL_AUTOCOMMIT, SQL_AUTOCOMMIT_ON );

    SQLTCHAR outdsn[4097];
    outdsn[4096] = 0;
    short buflen = 0;
    int status = 0;
    status = SQLDriverConnect( hdbc,
                               0,
                               (UCHAR*) connectString.toUtf8().data(),
                               SQL_NTS,
                               outdsn,
                               4096,
                               &buflen,
                               SQL_DRIVER_COMPLETE );

    if ( status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO ) {
        setError( Virtuoso::convertSqlError( SQL_HANDLE_DBC, hdbc ) );
        SQLFreeHandle( SQL_HANDLE_DBC, hdbc );
        return 0;
    }

    clearError();

    Connection* conn = new Connection();
    conn->d->m_hdbc = hdbc;
    conn->d->m_env = d;
    conn->d->m_connectString = connectString;
    d->m_openConnections.append( conn );
    return conn;
}


// static
Soprano::ODBC::Environment* Soprano::ODBC::Environment::createEnvironment()
{
    HENV henv;

    // allocate sql handle
    if ( SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv ) != SQL_SUCCESS ) {
        return 0;
    }

    // set odbc version
    SQLSetEnvAttr( henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, SQL_IS_UINTEGER );

    Environment* env = new Environment();
    env->d->m_henv = henv;
    return env;
}
