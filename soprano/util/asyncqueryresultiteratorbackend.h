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
        class AsyncQueryResultIteratorBackend : public QueryResultIteratorBackend, public AsyncIteratorBase<Soprano::BindingSet>
        {
        public:
            AsyncQueryResultIteratorBackend( AsyncModelPrivate* d, const QueryResultIterator& it )
                : AsyncIteratorBase<BindingSet>( d, it ),
                m_iterator( it ),
                m_isGraph(false),
                m_isBinding(false),
                m_isBool(false),
                m_boolVal(false) {
            }

            // called in work thread
            void initWorkThread() {
                m_isGraph = m_iterator.isGraph();
                m_isBinding = m_iterator.isBinding();
                m_isBool = m_iterator.isBool();
                if( m_isBool ) {
                    m_boolVal = m_iterator.boolValue();
                    m_iterator.close();
                }
            }

            bool next() {
                return AsyncIteratorBase<BindingSet>::getNext();
            }
            
            BindingSet current() const {
                return AsyncIteratorBase<BindingSet>::getCurrent();
            }
            
            Statement currentStatement() const {
                if( AsyncIteratorHandle::modelPrivate() && AsyncIteratorHandle::modelPrivate()->mode == AsyncModel::SingleThreaded )
                    return m_iterator.currentStatement();
                else
                    return m_currentStatement;
            }

            Node binding( const QString& name ) const {
                if( AsyncIteratorHandle::modelPrivate() && AsyncIteratorHandle::modelPrivate()->mode == AsyncModel::SingleThreaded )
                    return m_iterator.binding( name );
                else
                    return current()[ name ];
            }

            Node binding( int offset ) const {
                if( AsyncIteratorHandle::modelPrivate() && AsyncIteratorHandle::modelPrivate()->mode == AsyncModel::SingleThreaded )
                    return m_iterator.binding( offset );
                else
                    return current()[ offset ];
            }

            int bindingCount() const {
                if( AsyncIteratorHandle::modelPrivate() && AsyncIteratorHandle::modelPrivate()->mode == AsyncModel::SingleThreaded )
                    return m_iterator.bindingCount();
                else
                    return current().count();
            }

            QStringList bindingNames() const {
                if( AsyncIteratorHandle::modelPrivate() && AsyncIteratorHandle::modelPrivate()->mode == AsyncModel::SingleThreaded )
                    return m_iterator.bindingNames();
                else
                    return current().bindingNames();
            }

            bool isGraph() const {
                if( AsyncIteratorHandle::modelPrivate() && AsyncIteratorHandle::modelPrivate()->mode == AsyncModel::SingleThreaded )
                    return m_iterator.isGraph();
                else
                    return m_isGraph;
            }

            bool isBinding() const {
                if( AsyncIteratorHandle::modelPrivate() && AsyncIteratorHandle::modelPrivate()->mode == AsyncModel::SingleThreaded )
                    return m_iterator.isBinding();
                else
                    return m_isBinding;
            }

            bool isBool() const {
                if( AsyncIteratorHandle::modelPrivate() && AsyncIteratorHandle::modelPrivate()->mode == AsyncModel::SingleThreaded )
                    return m_iterator.isBool();
                else
                    return m_isBool;
            }

            bool boolValue() const {
                if( AsyncIteratorHandle::modelPrivate() && AsyncIteratorHandle::modelPrivate()->mode == AsyncModel::SingleThreaded )
                    return m_iterator.boolValue();
                else
                    return m_boolVal;
            }

            void close() {
                AsyncIteratorBase<BindingSet>::closeIterator();
            }

            Error::Error lastError() const {
                if( AsyncIteratorHandle::modelPrivate() && AsyncIteratorHandle::modelPrivate()->mode == AsyncModel::SingleThreaded )
                    return m_iterator.lastError();
                else
                    return AsyncIteratorBase<BindingSet>::m_error;
            }

        private:
            void enqueueCurrent() {
                if( isGraph() )
                    m_statementCache.enqueue( m_iterator.currentStatement() );
                else if( isBinding() )
                    AsyncIteratorBase<BindingSet>::enqueueCurrent();
            }

            void dequeueFirst() {
                if( isGraph() )
                    m_currentStatement = m_statementCache.dequeue();
                else if( isBinding() )
                    AsyncIteratorBase<BindingSet>::dequeueFirst();
            }

            int cacheFillState() const {
                if( isGraph() )
                    return m_statementCache.size();
                else if( isBinding() )
                    return AsyncIteratorBase<BindingSet>::cacheFillState();
                else
                    return 0;
            }

            QueryResultIterator m_iterator;
            bool m_isGraph;
            bool m_isBinding;
            bool m_isBool;
            bool m_boolVal;

            QQueue<Statement> m_statementCache;
            Statement m_currentStatement;
        };
    }
}

#endif
