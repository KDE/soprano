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

#include "simplestatementiteratortest.h"

#include "soprano/simplestatementiterator.h"
#include "soprano/statement.h"

#include <QtCore/QList>
#include <QtCore/QUrl>
#include <QtTest/QTest>

using namespace Soprano;


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


void SimpleStatementIteratorTest::testEmptyIterator()
{
    SimpleStatementIterator it;
    QVERIFY( !it.next() );
    QVERIFY( !it.current().isValid() );

    QList<Statement> sl;
    SimpleStatementIterator it2( sl );
    QVERIFY( !it2.next() );
    QVERIFY( !it2.current().isValid() );
}


void SimpleStatementIteratorTest::testIteration()
{
    QList<Statement> sl = createStatements( 5 );
    SimpleStatementIterator it( sl );
    while ( it.next() ) {
        sl.removeAll( *it );
    }
    QCOMPARE( 0, sl.count() );
}


void SimpleStatementIteratorTest::testAssignment()
{
    QList<Statement> sl = createStatements( 5 );
    SimpleStatementIterator it1( sl );
    for ( int i = 0; i < 2; ++i ) {
        QVERIFY( it1.next() );
    }

    SimpleStatementIterator it2 = it1;
    for ( int i = 0; i < 2; ++i ) {
        QVERIFY( it2.next() );
    }

    // there should be one left in the it
    QVERIFY( it1.next() );
    QVERIFY( !it1.next() );
    QVERIFY( !it2.next() );

    SimpleStatementIterator it3( it1 );
    QVERIFY( !it3.next() );

    it3 = sl;

    // the old its should still be the same
    QVERIFY( !it1.next() );
    QVERIFY( !it2.next() );

    // the new one however...
    for ( uint i = 0; i < sl.count(); ++i ) {
        QVERIFY( it3.next() );
    }
}

QTEST_MAIN(SimpleStatementIteratorTest)

#include "simplestatementiteratortest.moc"
