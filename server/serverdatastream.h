/*
 * This file is part of Soprano Project.
 *
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

#ifndef _SOPRANO_SERVER_DATA_STREAM_H_
#define _SOPRANO_SERVER_DATA_STREAM_H_

#include "datastream.h"

class QIODevice;

namespace Soprano {

    namespace Server {
        /**
        * Provides streaming methods for all %Soprano
        * types. Compared to QDataStream's operators
        * it provides error handling and automatically
        * fills the buffer if more data needs to be
        * requested (QIODevice::waitForReadyRead)
        *
        * \author Sebastian Trueg <trueg@kde.org>
        */
        class DataStream : public Soprano::DataStream
        {
        public:
            DataStream( QIODevice* dev );
            ~DataStream();

        protected:
            virtual bool read( char* data, qint64 size );
            virtual bool write( const char* data, qint64 size );

        private:
            QIODevice* m_device;
        };
    }
}

#endif
