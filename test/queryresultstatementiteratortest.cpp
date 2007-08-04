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

#include "soprano/queryresultstatementiterator.h"
#include "soprano/queryresultiterator.h"
#include "soprano/queryresultiteratorbackend.h"
#include "soprano/statement.h"

#include <QtCore/QList>
#include <QtCore/QUrl>
#include <QtTest/QTest>

using namespace Soprano;


class DummyQueryResult : public Soprano::QueryResultIteratorBackend
{
public:
    DummyQueryResult( const QList<Statement>& sl )
        : QueryResultIteratorBackend(),
          m_statements( sl ),
          m_iterator( sl.constBegin() ),
          m_first( true ) {
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
    QueryResultStatementIterator it;
    QVERIFY( !it.next() );
    QVERIFY( !it.current().isValid() );
}


void QueryResultStatementIteratorTest::testIteration()
{
    QList<Statement> sl = createStatements( 5 );
    QueryResultIterator resultIt( new DummyQueryResult( sl ) );
    QueryResultStatementIterator it( resultIt );
    while ( it.next() ) {
        sl.removeAll( *it );
    }
    QCOMPARE( 0, sl.count() );
}


void QueryResultStatementIteratorTest::testAssignment()
{
    QList<Statement> sl = createStatements( 5 );
    QueryResultIterator resultIt( new DummyQueryResult( sl ) );
    QueryResultStatementIterator it1( resultIt );
    for ( int i = 0; i < 2; ++i ) {
        QVERIFY( it1.next() );
    }

    QueryResultStatementIterator it2 = it1;
    for ( int i = 0; i < 2; ++i ) {
        QVERIFY( it2.next() );
    }

    // there should be one left in the it
    QVERIFY( it1.next() );
    QVERIFY( !it1.next() );
    QVERIFY( !it2.next() );
}

QTEST_MAIN(QueryResultStatementIteratorTest)

#include "queryresultstatementiteratortest.moc"
