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

#include "queryresultstatementiteratortest.h"

#include "soprano/queryresultiterator.h"
#include "soprano/queryresultiteratorbackend.h"
#include "soprano/statement.h"
#include "soprano/statementiterator.h"

#include <QtCore/QList>
#include <QtCore/QUrl>
#include <QtTest/QTest>

using namespace Soprano;


class DummyQueryResult : public Soprano::QueryResultIteratorBackend
{
public:
    DummyQueryResult( const QList<Statement>& sl )
        : QueryResultIteratorBackend(),
          m_first( true ),
          m_statements( sl ),
          m_iterator( sl.constBegin() ) {
    }

    bool next() {
        if ( !m_first &&
             m_iterator != m_statements.constEnd() ) {
            ++m_iterator;
        }

        m_first = false;
        return m_iterator != m_statements.constEnd();
    }

    Soprano::Statement currentStatement() const {
        if ( m_iterator != m_statements.constEnd() ) {
            return *m_iterator;
        }
        else {
            return Statement();
        }
    }

    Node binding( const QString &name ) const {
        Q_UNUSED( name );
        return Node();
    }

    Node binding( int offset ) const {
        Q_UNUSED( offset );
        return Node();
    }

    int bindingCount() const {
        return 0;
    }

    QStringList bindingNames() const {
        return QStringList();
    }

    bool isGraph() const {
        return true;
    }

    bool isBinding() const {
        return false;
    }

    bool isBool() const {
        return false;
    }

    bool boolValue() const {
        return false;
    }

    void close() {
    }

private:
    bool m_first;
    QList<Statement> m_statements;
    QList<Statement>::const_iterator m_iterator;
};


static QList<Statement> createStatements( int cnt )
{
    QList<Statement> sl;
    for ( int i = 0; i < cnt; ++i ) {
        sl.append( Statement( QUrl( QString( "test://subject%1" ).arg( i ) ),
                              QUrl( QString( "test://predicate%1" ).arg( i ) ),
                              QUrl( QString( "test://object%1" ).arg( i ) ),
                              QUrl( QString( "test://context%1" ).arg( i ) ) ) );
    }
    return sl;
}


void QueryResultStatementIteratorTest::testEmptyIterator()
{
    QueryResultIterator it;
    StatementIterator it2 = it.iterateStatements();
    QVERIFY( !it2.next() );
    QVERIFY( !it2.current().isValid() );
}


void QueryResultStatementIteratorTest::testIteration()
{
    QList<Statement> sl = createStatements( 5 );
    QueryResultIterator resultIt( new DummyQueryResult( sl ) );
    StatementIterator it = resultIt.iterateStatements();
    while ( it.next() ) {
        sl.removeAll( *it );
    }
    QCOMPARE( 0, sl.count() );
}

QTEST_MAIN(QueryResultStatementIteratorTest)

#include "queryresultstatementiteratortest.moc"
