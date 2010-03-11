/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2010 Sebastian Trueg <trueg@kde.org>
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

#ifndef _SOPRANO_SOCKET_H_
#define _SOPRANO_SOCKET_H_

#include "error.h"

#include <QtCore/QMutex>

#ifndef Q_OS_WIN
#define SOCKET_HANDLE int
#endif

namespace Soprano {
    /**
     * A thread-safe socket without the QObject overhead of Qt's own socket
     * implementations.
     */
    class Socket : public Error::ErrorCache
    {
    public:
        /**
         * Create a new socket.
         * \param fd Allows to create a wrapper around the fd to
         * read and write from and to it.
         */
        Socket( SOCKET_HANDLE fd = -1 );
        virtual ~Socket();

        SOCKET_HANDLE handle() const { return m_handle; }

        /**
         * Returns true of the socket has successfully been opened.
         * Does not actually check the connection. Thus, if the
         * connection is closed remotely this method does not notice it.
         */
        virtual bool isConnected() const;

        virtual void close();

        virtual bool waitForReadyRead( int timeout = -1 );

        virtual qint64 read( char* buffer, qint64 max );
        virtual qint64 write( const char* buffer, qint64 max );

        /// lock the socket (no other thread can use it)
        void lock();

        /// unlock the socket
        void unlock();

    protected:
        /**
         * Open the socket with the last config. Used
         * for automatic reconnect.
         */
        virtual bool open() {
            return false;
        }

        SOCKET_HANDLE m_handle;

    private:
        bool m_autoReConnect;
        QMutex m_mutex;
    };

    class LocalSocket : public Socket
    {
    public:
        LocalSocket();
        ~LocalSocket();

        bool open( const QString& path );

    private:
        bool open();

        QString m_path;
    };
}

#endif
