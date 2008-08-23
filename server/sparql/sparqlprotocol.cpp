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

#include "synchonoussparqlprotocol.h"

#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtNetwork/QHttp>
#include <QtNetwork/QHttpResponseHeader>
#include <QtCore/QBuffer>
#include <QtCore/QDebug>
#include <QtCore/QEventLoop>


Soprano::Client::SynchronousSparqlProtocol::SynchronousSparqlProtocol( QObject* parent )
    : QHttp( parent )
{
}


Soprano::Client::SynchronousSparqlProtocol::~SynchronousSparqlProtocol()
{
}


void Soprano::Client::SynchronousSparqlProtocol::setHost( const QString& hostname, quint16 port )
{
    QHttp::setHost( hostname, port );
}


void Soprano::Client::SynchronousSparqlProtocol::setUser( const QString& userName, const QString& password )
{
    QHttp::setUser( userName, password );
}


QByteArray Soprano::Client::SynchronousSparqlProtocol::query( const QString& queryS )
{
    QUrl url = QUrl( "/sparql" );
    url.addQueryItem( "query", queryS );

    QBuffer buffer;
    get( url.toEncoded(), &buffer );
    wait();

    QHttpResponseHeader h = lastResponse();

    if( h.statusCode() != 200 ){
        setError( errorString() );
        return QByteArray();
    }

//    qDebug() << buffer.data();

    clearError();
    return buffer.data();
}


void Soprano::Client::SynchronousSparqlProtocol::wait()
{
    QEventLoop loop;
    loop.connect( this, SIGNAL(done(bool)), &loop, SLOT(quit()) );
    loop.exec( QEventLoop::ExcludeUserInputEvents );
}
