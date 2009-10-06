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

#include "odbcconnection.h"
#include "odbcconnection_p.h"
#include "odbcenvironment_p.h"
#include "odbcqueryresult.h"
#include "odbcqueryresult_p.h"

#include "virtuosotools.h"

#include <QtCore/QDebug>

HSTMT Soprano::ODBC::ConnectionPrivate::execute( const QString& request )
{
    HSTMT hstmt;
    if ( SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &hstmt ) != SQL_SUCCESS ) {
        setError( Virtuoso::convertSqlError( SQL_HANDLE_DBC, m_hdbc ) );
        return 0;
    }
    else {
        QByteArray utf8Request = request.toUtf8();
        if ( !SQL_SUCCEEDED( SQLExecDirect( hstmt, ( UCHAR* )utf8Request.data(), utf8Request.length() ) ) ) {
            setError( Virtuoso::convertSqlError( SQL_HANDLE_STMT, hstmt ) );
            SQLFreeHandle( SQL_HANDLE_STMT, hstmt );
            return 0;
        }
        else {
            clearError();
            return hstmt;
        }
    }
}


Soprano::ODBC::Connection::Connection()
    : d( new ConnectionPrivate() )
{
}


Soprano::ODBC::Connection::~Connection()
{
    d->m_env->m_openConnections.removeAll( this );

    qDeleteAll( d->m_openResults );

    if ( d->m_hdbc ) {
        SQLFreeHandle( SQL_HANDLE_DBC, d->m_hdbc );
    }

    delete d;
}


Soprano::ODBC::Environment* Soprano::ODBC::Connection::environment() const
{
    return d->m_env->m_env;
}


QString Soprano::ODBC::Connection::odbcConnectString() const
{
    return d->m_connectString;
}


Soprano::Error::ErrorCode Soprano::ODBC::Connection::executeCommand( const QString& command )
{
//    qDebug() << Q_FUNC_INFO << command;

    Error::ErrorCode result = Error::ErrorNone;

    HSTMT hstmt = d->execute( command );
    setError( d->lastError() );
    if ( hstmt ) {
        SQLCloseCursor( hstmt );
        SQLFreeHandle( SQL_HANDLE_STMT, hstmt );
    }
    return result;
}


Soprano::ODBC::QueryResult* Soprano::ODBC::Connection::executeQuery( const QString& request )
{
//    qDebug() << Q_FUNC_INFO << request;

    HSTMT hstmt = d->execute( request );
    setError( d->lastError() );
    if ( hstmt ) {
        QueryResult* result = new QueryResult();
        result->d->m_conn = d;
        result->d->m_hstmt = hstmt;
        d->m_openResults.append( result );
        return result;
    }
    else {
        return 0;
    }
}
