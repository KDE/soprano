/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007 Rajeev J Sebastian <rajeev.sebastian@gmail.com>
 * Copyright (C) 2008 Sebastian Trueg <trueg@kde.org>
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

#include "sparqlprotocol.h"

#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtNetwork/QHttp>
#include <QtCore/QBuffer>
#include <QtCore/QDebug>
#include <QtCore/QEventLoop>
#include <QtCore/QPointer>


Soprano::Client::SparqlProtocol::SparqlProtocol( QObject* parent )
    : QHttp( parent ),
      m_path( QLatin1String( "/sparql" ) )
{
    connect( this, SIGNAL( requestFinished( int, bool ) ),
             this, SLOT( slotRequestFinished( int, bool ) ) );
}


Soprano::Client::SparqlProtocol::~SparqlProtocol()
{
    cancel();
}


void Soprano::Client::SparqlProtocol::cancel()
{
    QHttp::abort();
    foreach( QEventLoop* loop, m_loops ) {
        loop->exit();
    }
}


void Soprano::Client::SparqlProtocol::setHost( const QString& hostname, quint16 port )
{
    QHttp::setHost( hostname, port );
}


void Soprano::Client::SparqlProtocol::setUser( const QString& userName, const QString& password )
{
    QHttp::setUser( userName, password );
}


void Soprano::Client::SparqlProtocol::setPath( const QString& path )
{
    m_path = path;
}


int Soprano::Client::SparqlProtocol::query( const QString& queryS )
{
    QUrl url = QUrl( m_path );
    url.addQueryItem( "query", queryS );

    QBuffer* buffer = new QBuffer();
    int id = get( url.toEncoded(), buffer );
    m_resultsData[id] = buffer;

    qDebug() << Q_FUNC_INFO << url << id;

    return id;
}


QByteArray Soprano::Client::SparqlProtocol::blockingQuery( const QString& queryString )
{
    int id = query( queryString );
    waitForRequest( id );

    QByteArray data;
    if ( !m_results[id] ) {
        data = m_resultsData[id]->data();
    }

    m_results.remove( id );
    delete m_resultsData[id];
    m_resultsData.remove( id );

    qDebug() << Q_FUNC_INFO << data;

    return data;
}


void Soprano::Client::SparqlProtocol::waitForRequest( int id )
{
    QEventLoop loop;
    m_loops[id] = &loop;
    loop.exec( QEventLoop::ExcludeUserInputEvents );
    m_loops.remove( id );
}


void Soprano::Client::SparqlProtocol::slotRequestFinished( int id, bool error )
{
    qDebug() << Q_FUNC_INFO << id << error;

    // we ignore all the other requests such as setting the user and so on
    if ( m_resultsData.contains( id ) ) {
        QHttpResponseHeader h = lastResponse();
        if( h.statusCode() != 200 ){
            setError( QString( "Server did respond with %2 (%3)" ).arg( h.statusCode() ).arg( errorString() ) );
            qDebug() << m_resultsData[id]->data();
            error = true;
        }
        else {
            clearError();
        }

        if ( m_loops.contains( id ) ) {
            m_loops[id]->quit();
            m_loops.remove( id );
            m_results[id] = error;
        }
        else {
            emit requestFinished( id, error, m_resultsData[id]->data() );
            m_results.remove( id );
            delete m_resultsData[id];
            m_resultsData.remove( id );
        }
    }
}

#include "sparqlprotocol.moc"
