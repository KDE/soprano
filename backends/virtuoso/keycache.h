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

#ifndef _SOPRANO_ODBC_KEYCACHE_H_
#define _SOPRANO_ODBC_KEYCACHE_H_

#include <QtCore/QHash>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QDebug>


namespace Soprano {
    namespace ODBC {
        template<typename T> class KeyCache {
        public:
            bool contains( short key ) const {
                QMutexLocker lock( &m_mutex );
                return m_hash.contains( key );
            }
            void insert( short key, const T& value ) {
                QMutexLocker lock( &m_mutex );
                qDebug() << "inserting into KeyCache:" << key << value;
                m_hash.insert( key, value );
            }
            T value( short key ) const {
                QMutexLocker lock( &m_mutex );
                return m_hash[key];
            }

        private:
            QHash<short, T> m_hash;
            mutable QMutex m_mutex;
        };
    }
}

#endif


