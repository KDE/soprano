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
#include "virtuosotools.h"
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
    if ( SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &d->m_henv ) != SQL_SUCCESS ) {
        setError( "Failed to allocate SQL handle" );
        return false;
    }

    // set odbc version
    SQLSetEnvAttr( d->m_henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, SQL_IS_UINTEGER );

    // alloc hdbc handle
    if ( SQLAllocConnect( d->m_henv, &d->m_hdbc ) != SQL_SUCCESS ) {
        close();
        setError( "Failed to allocate SQL handle" );
        return false;
    }

    // set the app name
//    SQLSetConnectAttr( d->m_hdbc, SQL_ATTR_AUTOCOMMIT, SQL_AUTOCOMMIT_ON, 0 );
//    SQLSetConnectOption( d->m_hdbc, SQL_APPLICATION_NAME, "Soprano" );
    SQLSetConnectOption( d->m_hdbc, SQL_AUTOCOMMIT, SQL_AUTOCOMMIT_ON );

    SQLTCHAR outdsn[4097];
    outdsn[4096] = 0;
    short buflen = 0;
    int status = 0;
    status = SQLDriverConnect( d->m_hdbc,
                               0,
                               (UCHAR*) connectString.toUtf8().data(),
                               SQL_NTS,
                               outdsn,
                               4096,
                               &buflen,
                               SQL_DRIVER_COMPLETE );

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
        QByteArray utf8Request = request.toUtf8();
        if ( !SQL_SUCCEEDED( SQLExecDirect( hstmt, ( UCHAR* )utf8Request.data(), utf8Request.length() ) ) ) {
            setError( IODBC::convertSqlError( SQL_HANDLE_STMT, hstmt ) );
        }
        else {
            clearError();
            success = true;
        }

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


// FIXME: do not duplicate all the code - reuse the one from aboce without the overhead of creating the IODBCStatementHandler
Soprano::Error::ErrorCode Soprano::IODBCConnection::executeCommand( const QString& command )
{
    if ( !isConnected() ) {
        setError( "Not connected to iODBC" );
        return Error::ErrorUnknown;
    }

    HSTMT hstmt;
    if ( SQLAllocHandle( SQL_HANDLE_STMT, d->m_hdbc, &hstmt ) != SQL_SUCCESS ) {
        setError( IODBC::convertSqlError( SQL_HANDLE_DBC, d->m_hdbc ) );
        return Error::ErrorUnknown;
    }
    else {
        bool success = false;
        QByteArray utf8Request = command.toUtf8();
        if ( !SQL_SUCCEEDED( SQLExecDirect( hstmt, ( UCHAR* )utf8Request.data(), utf8Request.length() ) ) ) {
            setError( IODBC::convertSqlError( SQL_HANDLE_STMT, hstmt ) );
        }
        else {
            clearError();
            success = true;
        }

        SQLCloseCursor( hstmt );
        SQLFreeHandle( SQL_HANDLE_STMT, hstmt );

        return success ? Error::ErrorNone : Error::ErrorUnknown;
    }
}
