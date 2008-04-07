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

#ifndef _SOPRANO_UTIL_ASYNC_ITERATOR_BACKEND_H_
#define _SOPRANO_UTIL_ASYNC_ITERATOR_BACKEND_H_

#include "iteratorbackend.h"
#include "iterator.h"


namespace Soprano {
    namespace Util {

        class AsyncModelPrivate;

        class AsyncIteratorBase
        {
        public:
            AsyncIteratorBase( AsyncModelPrivate* d );
            virtual ~AsyncIteratorBase();
            void setModelGone();

        protected:
            void remove();

        private:
            AsyncModelPrivate* m_asyncModelPrivate;
        };


        template<typename T> class AsyncIteratorBackend : public IteratorBackend<T>, public AsyncIteratorBase
        {
        public:
            AsyncIteratorBackend( AsyncModelPrivate* d, const Iterator<T>& it )
                : AsyncIteratorBase( d ),
                  m_iterator( it ) {
            }

            bool next() {
                return m_iterator.next();
            }
            
            T current() const {
                return m_iterator.current();
            }
            
            void close() {
                m_iterator.close();
                remove();
            }

            Error::Error lastError() const {
                return m_iterator.lastError();
            }

        private:
            Iterator<T> m_iterator;
        };
    }
}

#endif
