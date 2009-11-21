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

#include "odbcconnectionpool.h"
#include "odbcconnectionpool_p.h"
#include "odbcconnection.h"
#include "odbcconnection_p.h"
#include "odbcenvironment.h"
#include "odbcenvironment_p.h"
#include "virtuosotools.h"

#include <QtCore/QDebug>
#include <QtCore/QThread>
#include <QtCore/QMutexLocker>

#include <sql.h>


Soprano::ODBC::Connection* Soprano::ODBC::ConnectionPoolPrivate::createConnection()
{
    Environment* env = Environment::createEnvironment();
    if ( !env ) {
        setError( "Unable to create ODBC environment." );
        return 0;
    }

    HDBC hdbc;

    // alloc hdbc handle
    if ( SQLAllocConnect( env->henv(), &hdbc ) != SQL_SUCCESS ) {
        setError( "Failed to allocate SQL handle" );
        delete env;
        return 0;
    }

    // set the app name
//    SQLSetConnectAttr( m_hdbc, SQL_ATTR_AUTOCOMMIT, SQL_AUTOCOMMIT_ON, 0 );
//    SQLSetConnectOption( m_hdbc, SQL_APPLICATION_NAME, "Soprano" );
    SQLSetConnectOption( hdbc, SQL_AUTOCOMMIT, SQL_AUTOCOMMIT_ON );

    SQLTCHAR outdsn[4097];
    outdsn[4096] = 0;
    short buflen = 0;
    int status = 0;
    status = SQLDriverConnect( hdbc,
                               0,
                               (UCHAR*) m_odbcConnectString.toUtf8().data(),
                               SQL_NTS,
                               outdsn,
                               4096,
                               &buflen,
                               SQL_DRIVER_COMPLETE );

    if ( status != SQL_SUCCESS && status != SQL_SUCCESS_WITH_INFO ) {
        setError( Virtuoso::convertSqlError( SQL_HANDLE_DBC, hdbc ) );
        SQLFreeHandle( SQL_HANDLE_DBC, hdbc );
        delete env;
        return 0;
    }

    clearError();

    Connection* conn = new Connection();
    conn->d->m_env = env;
    conn->d->m_hdbc = hdbc;
    conn->d->m_pool = this;
    return conn;
}



Soprano::ODBC::ConnectionPool::ConnectionPool( const QString& odbcConnectString, QObject* parent )
    : QObject( parent ),
      d( new ConnectionPoolPrivate() )
{
    d->m_odbcConnectString = odbcConnectString;
}


Soprano::ODBC::ConnectionPool::~ConnectionPool()
{
    qDeleteAll( d->m_openConnections );
    delete d;
}


Soprano::ODBC::Connection* Soprano::ODBC::ConnectionPool::connection()
{
//    qDebug() << Q_FUNC_INFO << QThread::currentThread();

    QMutexLocker lock( &d->m_connectionMutex );

    QHash<QThread*, Connection*>::iterator it = d->m_openConnections.find( QThread::currentThread() );
    if ( it != d->m_openConnections.end() ) {
//        qDebug() << Q_FUNC_INFO << "recycling" << *it;
        return *it;
    }

    Connection* conn = d->createConnection();
    d->m_openConnections.insert( QThread::currentThread(), conn );
    // using the cleanup slot rather than deleteLater to not depend on any event loop
    connect( QThread::currentThread(), SIGNAL( finished() ),
             conn, SLOT( cleanup() ),
             Qt::DirectConnection );
    connect( QThread::currentThread(), SIGNAL( terminated() ),
             conn, SLOT( cleanup() ),
             Qt::DirectConnection );
    connect( QThread::currentThread(), SIGNAL( destroyed() ),
             conn, SLOT( cleanup() ),
             Qt::DirectConnection );
    return conn;
}

#include "odbcconnectionpool.moc"
