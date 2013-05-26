/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>
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

#include "tcpclient.h"
#include "clientconnection.h"
#include "clientmodel.h"

#include <QtNetwork/QTcpSocket>
#include <QDebug>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_DECLARE_METATYPE( QAbstractSocket::SocketError )
#endif

const quint16 Soprano::Client::TcpClient::DEFAULT_PORT = 5000;


Soprano::Client::TcpClient::TcpClient( QObject* parent )
    : QObject( parent ),
      d( 0 )
{
    qWarning() << "This class is deprecated, and the implementation is only a stub now. DO NOT USE IT IN YOUR APPLICATION.";
}


Soprano::Client::TcpClient::~TcpClient()
{
    //delete d;
}


bool Soprano::Client::TcpClient::connect( const QHostAddress& address, int port )
{
    Q_UNUSED(address)
    Q_UNUSED(port)
    return false;
}


bool Soprano::Client::TcpClient::isConnected()
{
    return false;
}


void Soprano::Client::TcpClient::disconnect()
{
}


Soprano::Model* Soprano::Client::TcpClient::createModel( const QString& name, const QList<BackendSetting>& settings )
{
    Q_UNUSED(name)
    Q_UNUSED(settings)
    setError( "Not connected" );
    return 0;
}


void Soprano::Client::TcpClient::removeModel( const QString& name )
{
    Q_UNUSED(name)
    setError( "Not connected" );
}


void Soprano::Client::TcpClient::slotError( QAbstractSocket::SocketError error )
{
    Q_UNUSED(error)
    qDebug() << "Error: " << error;
}


// QStringList Soprano::Client::TcpClient::allModels() const
// {

// }

