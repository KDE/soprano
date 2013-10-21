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
#include "odbcconnectionpool_p.h"
#include "odbcenvironment.h"
#include "odbcenvironment_p.h"
#include "odbcqueryresult.h"
#include "odbcqueryresult_p.h"

#include "virtuosotools.h"

#include <QtCore/QDebug>
#include <QtCore/QThread>


Soprano::ODBC::Connection::Connection()
    : QObject(),
      d( new ConnectionPrivate() )
{
    qDebug() << Q_FUNC_INFO << QThread::currentThread();
}


Soprano::ODBC::Connection::~Connection()
{
    qDebug() << Q_FUNC_INFO << QThread::currentThread();

    d->m_pool->m_connectionMutex.lock();
    d->m_pool->m_openConnections.remove( d->m_pool->m_openConnections.key( this ) );
    d->m_pool->m_connectionMutex.unlock();

    qDeleteAll( d->m_openResults );

    if ( d->m_hdbc ) {
        SQLDisconnect( d->m_hdbc );
        SQLFreeHandle( SQL_HANDLE_DBC, d->m_hdbc );
    }
    delete d->m_env;

    delete d;
}


void Soprano::ODBC::Connection::cleanup()
{
    delete this;
}


Soprano::Error::ErrorCode Soprano::ODBC::Connection::executeCommand( const QString& command, const QList<Soprano::Node>& params )
{
//    qDebug() << Q_FUNC_INFO << command;

    Error::ErrorCode result = Error::ErrorNone;

    HSTMT hstmt = execute( command, params );
    if ( hstmt ) {
        SQLCloseCursor( hstmt );
        SQLFreeHandle( SQL_HANDLE_STMT, hstmt );
    }
    return result;
}


Soprano::ODBC::QueryResult* Soprano::ODBC::Connection::executeQuery( const QString& request )
{
//    qDebug() << Q_FUNC_INFO << request;

    HSTMT hstmt = execute( request );
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


HSTMT Soprano::ODBC::Connection::execute( const QString& request, const QList<Soprano::Node>& params )
{
    HSTMT hstmt;
    if ( SQLAllocHandle( SQL_HANDLE_STMT, d->m_hdbc, &hstmt ) != SQL_SUCCESS ) {
        setError( Virtuoso::convertSqlError( SQL_HANDLE_DBC, d->m_hdbc ) );
        return 0;
    }
    else {
        // counter for the parameter index
        int i = 1;
        // counter for the node index
        int ni = 0;

        // each parameter requires its own variable which can be referenced by a pointer
        // since we never use more than 4 nodes for now (we only use parameters for adding statements)
        // we can stick to a fixed number of variables
        SQLSMALLINT mode[4];
        QVector<QByteArray> values(4);
        QVector<QByteArray> dtOrLangs(4);

        // the type vars can be shared as they do not change
        SQLLEN cbInt = 0;
        SQLLEN cbChar = SQL_NTS;

        // run through all nodes and create the parameters
        // We do *not* parameterize blank nodes
        foreach(const Soprano::Node& node, params) {
            if(node.isResource()) {
                mode[ni] = 1;
                values[ni] = node.uri().toEncoded();
                SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_SSHORT, SQL_SMALLINT, 0, 0, &(mode[ni]), 0, &cbInt);
                SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, values[ni].length(), 0, values[ni].data(), 0, &cbChar);
                SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, values[ni].length(), 0, values[ni].data(), 0, &cbChar); // ignored
            }
            else if(node.isLiteral()) {
                values[ni] = node.literal().toString().toUtf8();
                if(!node.literal().dataTypeUri().isEmpty()) {
                    mode[ni] = 4;
                    dtOrLangs[ni] = node.literal().dataTypeUri().toEncoded();
                }
                else if(node.literal().language().isValid()) {
                    mode[ni] = 5;
                    dtOrLangs[ni] = node.literal().language().toString().toUtf8();
                }
                else {
                    mode[ni] = 3;
                }
                SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_SSHORT, SQL_SMALLINT, 0, 0, &(mode[ni]), 0, &cbInt);
                SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, values[ni].length(), 0, values[ni].data(), 0, &cbChar);
                SQLBindParameter( hstmt, i++, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, dtOrLangs[ni].length(), 0, dtOrLangs[ni].data(), 0, &cbChar);
            }

            ++ni;
        }
        QByteArray utf8Request = request.toUtf8();
        if ( !SQL_SUCCEEDED( SQLExecDirect( hstmt, ( UCHAR* )utf8Request.data(), utf8Request.length() ) ) ) {
            setError( Virtuoso::convertSqlError( SQL_HANDLE_STMT, hstmt, QLatin1String( "SQLExecDirect failed on query '" ) + request + '\'' ) );
            SQLFreeHandle( SQL_HANDLE_STMT, hstmt );
            return 0;
        }
        else {
            clearError();
            return hstmt;
        }
    }
}

