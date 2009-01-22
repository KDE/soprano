/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>
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

#ifndef _QUERYHIT_WRAPPER_RESULT_ITERATOR_BACKEND_H_
#define _QUERYHIT_WRAPPER_RESULT_ITERATOR_BACKEND_H_

#include "queryresultiteratorbackend.h"
#include "iterator.h"
#include "indexqueryhit.h"
#include "statement.h"

namespace Soprano {
    namespace Index {
        class QueryHitWrapperResultIteratorBackend : public QueryResultIteratorBackend
        {
        public:
            QueryHitWrapperResultIteratorBackend( const Iterator<QueryHit>& it );
            ~QueryHitWrapperResultIteratorBackend();

            bool next();
            void close();

            Statement currentStatement() const { return Statement(); }
            Node binding( const QString &name ) const;
            Node binding( int offset ) const;
            int bindingCount() const;
            QStringList bindingNames() const;
            bool isGraph() const { return false; }
            bool isBinding() const { return true; }
            bool isBool() const { return false; }
            bool boolValue() const { return false; }

        private:
            Iterator<QueryHit> m_it;
            QStringList m_bingingNameCache;
        };
    }
}

#endif
