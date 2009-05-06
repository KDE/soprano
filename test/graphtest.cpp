/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2009 Sebastian Trueg <trueg@k3b.org>
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

#include "graphtest.h"

#include "model.h"
#include "graph.h"
#include "statementiterator.h"
#include "nodeiterator.h"
#include "queryresultiterator.h"

using namespace Soprano;

namespace {
    class GraphModel : public Soprano::Model
    {
    public:
        GraphModel() {
        }

        ~GraphModel() {
        }

        Error::ErrorCode addStatement( const Statement& statement ) {
            if ( statement.isValid() ) {
                m_graph.addStatement( statement );
                clearError();
                return Error::ErrorNone;
            }
            else {
                setError( "invalid statement" );
                return Error::ErrorUnknown;
            }
        }

        Error::ErrorCode removeStatement( const Statement& statement ) {
            if ( statement.isValid() ) {
                m_graph.removeStatement( statement );
                clearError();
                return Error::ErrorNone;
            }
            else {
                setError( "invalid statement" );
                return Error::ErrorUnknown;
            }
        }

        Error::ErrorCode removeAllStatements( const Statement& statement ) {
            clearError();
            m_graph.removeAllStatements( statement );
            return Error::ErrorNone;
        }

        StatementIterator listStatements( const Statement& partial ) const {
            clearError();
            return m_graph.listStatements( partial );
        }

        NodeIterator listContexts() const {
            clearError();
            return m_graph.listContexts();
        }

        QueryResultIterator executeQuery( const QString&, Query::QueryLanguage, const QString& ) const {
            setError( "query not supported" );
            return QueryResultIterator();
        }

        bool containsAnyStatement( const Statement& statement ) const {
            clearError();
            return m_graph.containsAnyStatement( statement );
        }

        bool containsStatement( const Statement& statement ) const {
            if ( statement.isValid() ) {
                clearError();
                return m_graph.containsStatement( statement );
            }
            else {
                setError( "invalid statement" );
                return false;
            }
        }

        int statementCount() const {
            clearError();
            return m_graph.statementCount();
        }

        Node createBlankNode() {
            setError( "createBlankNode not supported" );
            return Node();
        }

        bool isEmpty() const {
            clearError();
            return m_graph.isEmpty();
        }

    private:
        Soprano::Graph m_graph;
    };
}


GraphTest::GraphTest()
{
    setTestSignals( false );
    setSupportedBackendFeatures( BackendFeatureAddStatement|
                                 BackendFeatureRemoveStatements|
                                 BackendFeatureListStatements|
                                 BackendFeatureContext|
                                 BackendFeatureStorageMemory );
}


Soprano::Model* GraphTest::createModel()
{
    return new GraphModel();
}


void GraphTest::testCloseStatementIteratorOnModelDelete()
{
    // nothing. the iterators have a copy of the graph
}

QTEST_MAIN( GraphTest )

#include "graphtest.moc"

