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

#ifndef _SOPRANO_UTIL_ASYNC_QUERY_RESULT_ITERATOR_BACKEND_H_
#define _SOPRANO_UTIL_ASYNC_QUERY_RESULT_ITERATOR_BACKEND_H_

#include "queryresultiteratorbackend.h"
#include "queryresultiterator.h"
#include "asyncmodel_p.h"
#include "asynciteratorbackend.h"

namespace Soprano {
    namespace Util {
        class AsyncQueryResultIteratorBackend : public QueryResultIteratorBackend, public AsyncIteratorBase
        {
        public:
            AsyncQueryResultIteratorBackend( AsyncModelPrivate* d, const QueryResultIterator& it )
                : AsyncIteratorBase( d ),
                  m_iterator( it ) {
            }

            bool next() {
                return m_iterator.next();
            }
            
            BindingSet current() const {
                return m_iterator.current();
            }
            
            Statement currentStatement() const {
                return m_iterator.currentStatement();
            }

            Node binding( const QString& name ) const {
                return m_iterator.binding( name );
            }

            Node binding( int offset ) const {
                return m_iterator.binding( offset );
            }

            int bindingCount() const {
                return m_iterator.bindingCount();
            }

            QStringList bindingNames() const {
                return m_iterator.bindingNames();
            }

            bool isGraph() const {
                return m_iterator.isGraph();
            }

            bool isBinding() const {
                return m_iterator.isBinding();
            }

            bool isBool() const {
                return m_iterator.isBool();
            }

            bool boolValue() const {
                return m_iterator.boolValue();
            }

            void close() {
                m_iterator.close();
                remove();
            }

            Error::Error lastError() const {
                return m_iterator.lastError();
            }

        private:
            QueryResultIterator m_iterator;
        };
    }
}

#endif
