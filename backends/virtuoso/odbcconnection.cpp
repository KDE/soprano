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


bool Soprano::ODBC::ConnectionPrivate::getCharData( HSTMT hstmt, int colNum, SQLCHAR** buffer, SQLLEN* length )
{
    SQLCHAR dummyBuffer[1]; // dummy buffer only used to determine length

    int r = SQLGetData( hstmt, colNum, SQL_C_CHAR, dummyBuffer, 0, length );

    if ( SQL_SUCCEEDED( r ) ) {
        //
        // Treat a 0 length and null data as an empty node
        //
        if ( *length == SQL_NULL_DATA || *length == 0 ) {
            *buffer = 0;
            *length = 0;
            return true;
        }

        //
        // again with real length buffer
        //
        else {
            *buffer = new SQLCHAR[*length+4]; // FIXME: why +4 (I got this from the redland plugin)
            r = SQLGetData ( hstmt, colNum, SQL_C_CHAR, *buffer, *length+4, length );
            if ( SQL_SUCCEEDED( r ) ) {
                return true;
            }
            else {
                delete [] *buffer;
                *buffer = 0;
                *length = 0;
                setError( Virtuoso::convertSqlError( SQL_HANDLE_STMT, hstmt, QLatin1String( "SQLGetData failed" ) ) );
                return false;
            }
        }
    }
    else {
        setError( Virtuoso::convertSqlError( SQL_HANDLE_STMT, hstmt, QLatin1String( "SQLGetData for data lenght failed" ) ) );
        return false;
    }
}


QString Soprano::ODBC::ConnectionPrivate::getLang( short key )
{
    // FIXME: error handling
    if ( m_langCache.contains( key ) ) {
        return m_langCache.value( key );
    }

    QString lang;

    HSTMT hstmt;
    int rc = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &hstmt );
    if ( !SQL_SUCCEEDED(rc) ) {
        qDebug() << "Failed to allocate HSTMT";
        return QString();
    }

    SQLLEN ind = 0;
    rc = SQLBindParameter( hstmt, 1, SQL_PARAM_INPUT, SQL_C_SSHORT, SQL_SMALLINT, 0, 0, &key, 0, &ind );
    if ( SQL_SUCCEEDED(rc) ) {
        rc = SQLExecDirect( hstmt, ( SQLCHAR* )"select RL_ID from DB.DBA.RDF_LANGUAGE where RL_TWOBYTE=?", SQL_NTS );
        if ( SQL_SUCCEEDED(rc) ) {
            rc = SQLFetch( hstmt );
            if ( SQL_SUCCEEDED(rc) ) {
                SQLCHAR* data = 0;
                SQLLEN length = 0;
                if ( getCharData( hstmt, 1, &data, &length ) ) {
                    lang = QString::fromLatin1( reinterpret_cast<const char*>( data ), length );
                    m_langCache.insert( key, lang );
                }
            }
        }
    }

    SQLCloseCursor( hstmt );
    SQLFreeHandle( SQL_HANDLE_STMT, hstmt );

    return lang;
}


QUrl Soprano::ODBC::ConnectionPrivate::getType( short key )
{
    // FIXME: error handling
    if ( m_typeCache.contains( key ) ) {
        return m_typeCache.value( key );
    }

    QUrl type;

    HSTMT hstmt;

    int rc = SQLAllocHandle( SQL_HANDLE_STMT, m_hdbc, &hstmt );
    if (!SQL_SUCCEEDED(rc)) {
        qDebug() << "Failed to allocate HSTMT";
        return QUrl();
    }

    SQLLEN ind;
    rc = SQLBindParameter( hstmt, 1, SQL_PARAM_INPUT, SQL_C_SSHORT, SQL_SMALLINT, 0, 0, &key, 0, &ind);
    if ( SQL_SUCCEEDED(rc) ) {
        rc = SQLExecDirect( hstmt, ( SQLCHAR* )"select RDT_QNAME from DB.DBA.RDF_DATATYPE where RDT_TWOBYTE=?", SQL_NTS );
        if ( SQL_SUCCEEDED(rc) ) {
            rc = SQLFetch( hstmt );
            if ( SQL_SUCCEEDED(rc) ) {
                SQLCHAR* data = 0;
                SQLLEN length = 0;
                if ( getCharData( hstmt, 1, &data, &length ) ) {
                    type = QUrl::fromEncoded( reinterpret_cast<const char*>( data ), QUrl::StrictMode );
                    m_typeCache.insert( key, type );
                }
            }
        }
    }

    SQLCloseCursor( hstmt );
    SQLFreeHandle( SQL_HANDLE_STMT, hstmt );

    return type;
}



Soprano::ODBC::Connection::Connection()
    : d( new ConnectionPrivate() )
{
    qDebug() << Q_FUNC_INFO << QThread::currentThread();
}


Soprano::ODBC::Connection::~Connection()
{
    qDebug() << Q_FUNC_INFO << QThread::currentThread();

    qDeleteAll( d->m_openResults );

    if ( d->m_hdbc ) {
        SQLFreeHandle( SQL_HANDLE_DBC, d->m_hdbc );
    }
    delete d->m_env;

    delete d;
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
