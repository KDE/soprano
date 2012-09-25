/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2008-2012 Sebastian Trueg <trueg@kde.org>
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

#ifndef _SOPRANO_SOCKET_STREAM_H_
#define _SOPRANO_SOCKET_STREAM_H_

#include "socket.h"
#include "datastream.h"

namespace Soprano {


    /**
     * Provides streaming methods for all %Soprano
     * types. Compared to QDataStream's operators
     * it provides error handling and automatically
     * fills the buffer if more data needs to be
     * requested (QIODevice::waitForReadyRead)
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class SocketStream : public DataStream
    {
    public:
        SocketStream( Socket* dev );
        ~SocketStream();

    protected:
        virtual bool read( char* data, qint64 size );
        virtual bool write( const char* data, qint64 size );

    private:
        Socket* m_device;
    };
}

#endif
