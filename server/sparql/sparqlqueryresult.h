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

#ifndef _SPARQL_QUERY_RESULT_H
#define _SPARQL_QUERY_RESULT_H

#include "queryresultiteratorbackend.h"
#include "sparqlxmlresultparser.h"

namespace Soprano {
    namespace Client {
        class SparqlQueryResult : public Soprano::QueryResultIteratorBackend
        {
        public:
            SparqlQueryResult( const SparqlParser::Sparql& result );
            ~SparqlQueryResult();

            bool next();
            Statement currentStatement() const;
            Node binding( const QString &name ) const;
            Node binding( int offset ) const;
            int bindingCount() const;
            QStringList bindingNames() const;
            bool isGraph() const;
            bool isBinding() const;
            bool isBool() const;
            bool boolValue() const;
            void close();

        private:
            SparqlParser::Sparql m_result;
            QStringList m_bindingNames;
            int m_currentResultIndex;
        };
    }
}

#endif

