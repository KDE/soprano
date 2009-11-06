/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2009 Sebastian Trueg <trueg@kde.org>
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

#include "virtuosoqueryresultiteratorbackend.h"


namespace Soprano {
    namespace ODBC {
        class ConnectionPool;
    }

    class VirtuosoModelPrivate
    {
    public:
        VirtuosoModelPrivate()
            : connectionPool( 0 ) {
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

        ODBC::ConnectionPool* connectionPool;
        QList<Virtuoso::QueryResultIteratorBackend*> m_openIterators;
        
    private:
        QMutex m_openIteratorMutex;
    };
}

#endif
