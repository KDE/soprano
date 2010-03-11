/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2010 Sebastian Trueg <trueg@kde.org>
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

#ifndef _SOPRANO_SOCKET_SERVER_H_
#define _SOPRANO_SOCKET_SERVER_H_

#include <QtCore/QObject>

#include "error.h"
#include "socket.h"

class QString;

namespace Soprano {

    class Socket;
    class SocketNotifier;

    class SocketServer : public QObject, public Error::ErrorCache
    {
        Q_OBJECT

    public:
        SocketServer( QObject* parent = 0 );
        virtual ~SocketServer();

        virtual void close();

        QString socketName() const;

    Q_SIGNALS:
        void newConnection( SOCKET_HANDLE handle );

    protected:
        void handleOpenedServerSocket( SOCKET_HANDLE handle );

        Socket* m_socket;

    private Q_SLOTS:
        void slotServerSocketActivated();

    private:
        SocketNotifier* m_socketNotifier;
    };


    class LocalSocketServer : public SocketServer
    {
    public:
        LocalSocketServer( QObject* parent = 0 );
        ~LocalSocketServer();

        bool listen( const QString& socketName );
        void close();

    private:
        QString m_socketName;
    };
}

#endif
