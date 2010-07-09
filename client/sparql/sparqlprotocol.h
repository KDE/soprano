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
#include <QtCore/QHash>

class QEventLoop;
class QBuffer;

namespace Soprano {
    namespace Client {
        class SparqlProtocol : public QHttp, public Soprano::Error::ErrorCache
        {
            Q_OBJECT

        public:
            SparqlProtocol( QObject* parent = 0 );
            ~SparqlProtocol();

            void setHost( const QString& hostname, quint16 port = 8889 );
            void setUser( const QString& userName, const QString& password = QString() );

            /**
             * For historical reasons the default path is set to "/sparql".
             * This method allows to change that.
             */
            void setPath( const QString& path );

            /**
             * \returns the response data. An emtpy QByteArray
             * on error. Check lastError() for details.
             */
            QByteArray blockingQuery( const QString& query );

            int query( const QString& query );

        Q_SIGNALS:
            void requestFinished( int id, bool error, const QByteArray& data );

        public Q_SLOTS:
            void cancel();

        private Q_SLOTS:
            void slotRequestFinished( int id, bool error );

        private:
            void waitForRequest( int id );

            QHash<int, QEventLoop*> m_loops;
            QHash<int, bool> m_results;
            QHash<int, QBuffer*> m_resultsData;
            QString m_path;
        };
    }
}

#endif
