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

#include "iodbcconnection.h"
#include "iodbctools.h"
#include "iodbcstatementhandler.h"

#include <QtCore/QDebug>
#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QPointer>


class Soprano::IODBCConnection::Private
{
public:
    Private()
        : m_henv( SQL_NULL_HANDLE ),
          m_hdbc( SQL_NULL_HANDLE ),
          m_connected( false ) {
    }

    HENV m_henv;
    HDBC m_hdbc;

    bool m_connected;

    QList<QPointer<IODBCStatementHandler> > openHandlers;

    void rememberHandler( IODBCStatementHandler* hdl ) {
        // cleanup old handlers that have already been deleted
        QList<QPointer<IODBCStatementHandler> >::iterator it = openHandlers.begin();
        while ( it != openHandlers.end() ) {
            if ( !*it ) {
                it = openHandlers.erase( it );
            }
            else {
                ++it;
            }
        }
        openHandlers << hdl;
    }
};



Soprano::IODBCConnection::IODBCConnection()
    : d( new Private() )
{
}


Soprano::IODBCConnection::~IODBCConnection()
{
    close();
    delete d;
}


bool Soprano::IODBCConnection::connect( const QString& connectString )
{
    close();

    // allocate sql handle
    if ( SQLAllocHandle( SQL_HANDLE_ENV, NULL, &d->m_henv ) != SQL_SUCCESS ) {
        setError( "Failed to allocate SQL handle" );
        return false;
    }

    // set odbc version
    SQLSetEnvAttr( d->m_henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, SQL_IS_UINTEGER );

    // alloc hdbc handle
    if ( SQLAllocHandle (SQL_HANDLE_DBC, d->m_henv, &d->m_hdbc) != SQL_SUCCESS ) {
        close();
        setError( "Failed to allocate SQL handle" );
        return false;
    }

    // set the app name
    SQLSetConnectOption( d->m_hdbc, SQL_APPLICATION_NAME, (SQLULEN) TEXT ("Soprano") );

    SQLTCHAR outdsn[4097];
    outdsn[4096] = 0;
    short buflen = 0;
    int status = 0;
    wchar_t* connWString = new wchar_t[connectString.length()+1];
    connectString.toWCharArray( connWString );
    connWString[connectString.length()] = 0;
    status = SQLDriverConnectW ( d->m_hdbc,
                                 0,
                                 (SQLWCHAR*) connWString,
                                 SQL_NTS,
                                 (SQLWCHAR*) outdsn,
                                 4096,
                                 &buflen,
                                 SQL_DRIVER_COMPLETE );
    delete [] connWString;

    if ( status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO ) {
        setError( IODBC::convertSqlError( SQL_HANDLE_DBC, d->m_hdbc ) );
        close();
        return false;
    }

    clearError();
    d->m_connected = true;
    return true;
}


bool Soprano::IODBCConnection::isConnected() const
{
    return d->m_connected;
}


void Soprano::IODBCConnection::close()
{
    // close all open iterators, ie. IODBCStatementHandlers
    qDeleteAll( d->openHandlers );
    d->openHandlers.clear();

    if ( d->m_connected ) {
        SQLDisconnect( d->m_hdbc );
    }

    if ( d->m_hdbc ) {
        SQLFreeHandle( SQL_HANDLE_DBC, d->m_hdbc );
        d->m_hdbc = SQL_NULL_HANDLE;
    }

    if ( d->m_henv ) {
        SQLFreeHandle( SQL_HANDLE_ENV, d->m_henv );
        d->m_henv = SQL_NULL_HANDLE;
    }

    d->m_connected = false;
    clearError();
}


Soprano::IODBCStatementHandler* Soprano::IODBCConnection::execute( const QString& request )
{
    if ( !isConnected() ) {
        setError( "Not connected to iODBC" );
        return false;
    }

    HSTMT hstmt;
    if ( SQLAllocHandle( SQL_HANDLE_STMT, d->m_hdbc, &hstmt ) != SQL_SUCCESS ) {
        setError( IODBC::convertSqlError( SQL_HANDLE_DBC, d->m_hdbc ) );
        return 0;
    }
    else {
        // we create it already to reuse its desctructor on error
        IODBCStatementHandler* hdl = new IODBCStatementHandler( d->m_hdbc, hstmt );

        bool success = false;
        wchar_t* requestW = new wchar_t[request.length()+1];
        request.toWCharArray( requestW );
        requestW[request.length()] = 0;
        if ( !SQL_SUCCEEDED( SQLPrepare( hstmt, requestW, request.length() ) ) ) {
            setError( IODBC::convertSqlError( SQL_HANDLE_STMT, hstmt ) );
        }
        else {
            int r = SQLExecute( hstmt );
            if ( !SQL_SUCCEEDED( r ) ) {
                setError( IODBC::convertSqlError( SQL_HANDLE_STMT, hstmt ) );
            }
            else {
                clearError();
                success = true;
            }
        }

        delete [] requestW;
        if ( success ) {
            d->rememberHandler( hdl );
            return hdl;
        }
        else {
            delete hdl;
            return 0;
        }
    }
}
