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

#ifndef _SOPRANO_SERVER_CONNECTION_H_
#define _SOPRANO_SERVER_CONNECTION_H_

#include <QtCore/QObject>
#include <QtCore/QThread>

class QIODevice;

namespace Soprano {

    class Backend;
    namespace Util {
        class AsyncResult;
    }

    namespace Server {

        class ServerCore;
        class ModelPool;

        class ServerConnection : public QThread
        {
            Q_OBJECT

        public:
            /**
             * Create a new ServerConnection.
             *
             * \param core The ServerCore that maintains all Models.
             * \param socket The connection socket.
             */
            ServerConnection( ModelPool* pool, ServerCore* core );
            virtual ~ServerConnection();

            void close();

        protected:
            void run();

            virtual QIODevice* createIODevice() = 0;

        private:
            class Private;
            Private* const d;

            Q_PRIVATE_SLOT( d, void _s_readNextCommand() )
        };
    }
}

#endif
