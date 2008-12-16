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


#ifndef _SPARQL_UTIL_H_
#define _SPARQL_UTIL_H_

#include <soprano/error.h>
#include <QtNetwork/QHttp>

class QEventLoop;

namespace Soprano {
    namespace Client {
        class SynchronousSparqlProtocol : protected QHttp, public Soprano::Error::ErrorCache
        {
        public:
            SynchronousSparqlProtocol( QObject* parent = 0 );
            ~SynchronousSparqlProtocol();

            void setHost( const QString& hostname, quint16 port = 8889 );
            void setUser( const QString& userName, const QString& password = QString() );

            /**
             * \returns the response data. An emtpy QByteArray
             * on error. Check lastError() for details.
             */
            QByteArray query(const QString& query);

            void cancel();

        private:
            void wait();

            QEventLoop* m_loop;
        };
    }
}

#endif
