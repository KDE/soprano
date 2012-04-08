/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2009-2012 Sebastian Trueg <trueg@kde.org>
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

#ifndef _SOPRANO_IODBC_MODEL_P_H_
#define _SOPRANO_IODBC_MODEL_P_H_

#include <QtCore/QMutex>
#include <QtCore/QRegExp>

#include "virtuosoqueryresultiteratorbackend.h"


namespace Soprano {
    namespace ODBC {
        class ConnectionPool;
    }

    class VirtuosoModel;

    class VirtuosoModelPrivate
    {
    public:
        VirtuosoModelPrivate()
            : connectionPool( 0 ),
              m_noStatementSignals( false ),
              m_fakeBooleanRegExp( QLatin1String("([\"'])(true|false)\\1\\^\\^(<http\\://www\\.w3\\.org/2001/XMLSchema#boolean>|\\w+\\:boolean)"),
                                   Qt::CaseInsensitive,
                                   QRegExp::RegExp2 ),
              m_openIteratorMutex( QMutex::Recursive ) {
        }

        inline void addIterator( Virtuoso::QueryResultIteratorBackend* iterator ) {
            m_openIteratorMutex.lock();
            m_openIterators.append(iterator);
            m_openIteratorMutex.unlock();
        }

        inline void removeIterator( Virtuoso::QueryResultIteratorBackend* iterator ) {
            m_openIteratorMutex.lock();
            m_openIterators.removeAll(iterator);
            m_openIteratorMutex.unlock();
        }

        void closeAllIterators() {
            m_openIteratorMutex.lock();
            while ( !m_openIterators.isEmpty() )
                m_openIterators.last()->close();
            m_openIteratorMutex.unlock();
        }

        QueryResultIterator sqlQuery( const QString& query );
        QueryResultIterator sparqlQuery( const QString& query );

        QString replaceFakeTypesInQuery( const QString& query );

        ODBC::ConnectionPool* connectionPool;
        QList<Virtuoso::QueryResultIteratorBackend*> m_openIterators;

        QString m_virtuosoVersion;

        bool m_noStatementSignals;

        VirtuosoModel* q;

    private:
        QRegExp m_fakeBooleanRegExp;
        QMutex m_fakeBooleanRegExpMutex;

        QMutex m_openIteratorMutex;
    };
}

#endif
