/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2009 Sebastian Trueg <trueg@kde.org>
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

#ifndef _SOPRANO_SERVER_CLIENT_CONNECTION_P_H_
#define _SOPRANO_SERVER_CLIENT_CONNECTION_P_H_

#include <QtCore/QMutex>
#include <QtCore/QList>
#include <QtCore/QThreadStorage>
#include <QtCore/QIODevice>

class QThread;

namespace Soprano {
    namespace Client {
        class ClientConnectionPrivate;

        /**
         * A simple wrapper class which makes sure that
         * sockets are closed and deleted once their
         * spawning thread goes down.
         */
        class SocketHandler : public QObject
        {
            Q_OBJECT

        public:
            SocketHandler( ClientConnectionPrivate*, QIODevice* socket );
            ~SocketHandler();

            QIODevice* socket() const { return m_socket; }
            void close() { m_socket->close(); }

        private:
            ClientConnectionPrivate* m_client;
            QIODevice* m_socket;
        };

        class ClientConnectionPrivate
        {
        public:
            QList<QIODevice*> sockets;
            QMutex socketMutex;
            QThreadStorage<Soprano::Client::SocketHandler*> socketStorage;
        };
    }
}

#endif
