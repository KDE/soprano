/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2008 Sebastian Trueg <strueg@mandriva.com>
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

#include "asyncmodeltest.h"
#include "asyncresultwaiter.h"
#include "soprano.h"

#include <QtCore/QPointer>
#include <QtCore/QUuid>

#include <QtTest/QTest>

using namespace Soprano;
using namespace Soprano::Util;

Q_DECLARE_METATYPE( Soprano::Error::ErrorCode )
Q_DECLARE_METATYPE( Soprano::StatementIterator )
Q_DECLARE_METATYPE( Soprano::QueryResultIterator )


static QUrl createRandomUri()
{
    // FIXME: check if the uri already exists
    QString uid = QUuid::createUuid().toString();
    uid = uid.mid( 1, uid.length()-2 );
    return QUrl( "http://soprano.org/test#" + uid );
}


static QList<Statement> createTestData( const Statement& s, int num )
{
    QList<Statement> sl;
    for( int i = 0; i < num; ++i ) {
        sl.append( Statement( s.subject().isEmpty() ? Node( createRandomUri() ) : s.subject(),
                              s.predicate().isEmpty() ? Node( createRandomUri() ) : s.predicate(),
                              s.object().isEmpty() ? Node( createRandomUri() ) : s.object(),
                              s.context() ) );
    }
    return sl;
}


void AsyncModelTest::initTestCase()
{
    Soprano::Model* model = Soprano::createModel();
    QVERIFY( model );
    m_asyncModel = new Soprano::Util::AsyncModel( model );
    m_asyncModel->setMode( Soprano::Util::AsyncModel::MultiThreaded );


    Node subject1( QUrl("http://soprano.sf.net#init:test1") );
    Node subject2( QUrl("http://soprano.sf.net#init:test2") );

    Node predicate1( QUrl( "http://soprano.sf.net#predicate1" ) );
    Node predicate2( QUrl( "http://soprano.sf.net#predicate2" ) );

    Node object1( LiteralValue( "Literal value1" ) );
    Node object2( LiteralValue( "Literal value2" ) );

    m_s1 = Soprano::Statement( subject1, predicate1, object1 );
    m_s2 = Soprano::Statement( subject2, predicate2, object2 );
}


void AsyncModelTest::testAddStatement()
{
    QVariant result = AsyncResultWaiter::waitForResult( m_asyncModel->addStatementAsync( m_s1 ) );
    QVERIFY( result.value<Soprano::Error::ErrorCode>() == Soprano::Error::ErrorNone );
    QVERIFY( m_asyncModel->containsStatement( m_s1 ) );

    QVERIFY( AsyncResultWaiter::waitForResult( m_asyncModel->containsStatementAsync( m_s1 ) ).toBool() );
}


void AsyncModelTest::testCountStatements()
{
    QVariant result = AsyncResultWaiter::waitForResult( m_asyncModel->statementCountAsync() );
    int c = result.toInt();
    if ( c != -1 ) {
        QCOMPARE( c, 1 );
    }
}


void AsyncModelTest::testListStatements()
{
    m_asyncModel->removeAllStatements();
    m_asyncModel->addStatement( m_s1 );
    m_asyncModel->addStatement( m_s2 );

    QVariant result = AsyncResultWaiter::waitForResult( m_asyncModel->listStatementsAsync() );
    QVERIFY( result.userType() == qMetaTypeId<Soprano::StatementIterator>() );
    Soprano::StatementIterator it = result.value<Soprano::StatementIterator>();
    QVERIFY( it.next() );
    QVERIFY( *it == m_s1 || *it == m_s2 );
    QVERIFY( it.next() );
    QVERIFY( *it == m_s1 || *it == m_s2 );
    QVERIFY( !it.next() );
}


void AsyncModelTest::testSelectQuery()
{
    m_asyncModel->removeAllStatements();
    m_asyncModel->addStatement( m_s1 );
    m_asyncModel->addStatement( m_s2 );

    QVariant result = AsyncResultWaiter::waitForResult( m_asyncModel->executeQueryAsync( "select ?r ?p ?o where { ?r ?p ?o . }",  Query::QueryLanguageSparql ) );
    QVERIFY( result.userType() == qMetaTypeId<Soprano::QueryResultIterator>() );
    Soprano::QueryResultIterator it = result.value<Soprano::QueryResultIterator>();
    QVERIFY( !it.isGraph() );
    QVERIFY( !it.isBool() );
    QVERIFY( it.isBinding() );
    QVERIFY( it.next() );
    QCOMPARE( it.bindingCount(), 3 );
    QCOMPARE( it.bindingNames()[0], QString( "r" ) );
    QCOMPARE( it.bindingNames()[1], QString( "p" ) );
    QCOMPARE( it.bindingNames()[2], QString( "o" ) );
    QVERIFY( it.binding( 0 ) == m_s1.subject() || it.binding(0) == m_s2.subject() );
    QVERIFY( it.next() );
    QCOMPARE( it.bindingCount(), 3 );
    QVERIFY( it.binding(0) == m_s1.subject() || it.binding(0) == m_s2.subject() );
    QVERIFY( !it.next() );
}


void AsyncModelTest::testConstructQuery()
{
    m_asyncModel->removeAllStatements();
    m_asyncModel->addStatement( m_s1 );
    m_asyncModel->addStatement( m_s2 );

    QVariant result = AsyncResultWaiter::waitForResult( m_asyncModel->executeQueryAsync( "construct { ?r ?p ?o } where { ?r ?p ?o . }",  Query::QueryLanguageSparql ) );
    QVERIFY( result.userType() == qMetaTypeId<Soprano::QueryResultIterator>() );
    Soprano::QueryResultIterator it = result.value<Soprano::QueryResultIterator>();
    QVERIFY( it.isGraph() );
    QVERIFY( !it.isBool() );
    QVERIFY( !it.isBinding() );
    QVERIFY( it.next() );
    QVERIFY( it.currentStatement() == m_s1 || it.currentStatement() == m_s2 );
    QVERIFY( it.next() );
    QVERIFY( it.currentStatement() == m_s1 || it.currentStatement() == m_s2 );
    QVERIFY( !it.next() );
}


void AsyncModelTest::testAskQuery()
{
    m_asyncModel->removeAllStatements();
    m_asyncModel->addStatement( m_s1 );

    QVariant result = AsyncResultWaiter::waitForResult( m_asyncModel->executeQueryAsync( "ask where { ?r ?p ?o . }",  Query::QueryLanguageSparql ) );
    QVERIFY( result.userType() == qMetaTypeId<Soprano::QueryResultIterator>() );
    Soprano::QueryResultIterator it = result.value<Soprano::QueryResultIterator>();
    QVERIFY( it.isBool() );
    QVERIFY( !it.isGraph() );
    QVERIFY( !it.isBinding() );
    QVERIFY( it.boolValue() );
}


void AsyncModelTest::testListAndAdd()
{
    // perform a list and then an add. The add should be postponed until after the list
    Soprano::StatementIterator it = AsyncResultWaiter::waitForResult( m_asyncModel->listStatementsAsync() ).value<Soprano::StatementIterator>();
    QPointer<AsyncResult> addResult = m_asyncModel->addStatementAsync( m_s2 );
    QVERIFY( it.next() );
    it.close();
    QVERIFY( addResult );
    QVERIFY( AsyncResultWaiter::waitForResult( addResult ).value<Soprano::Error::ErrorCode>() == Soprano::Error::ErrorNone );
}


void AsyncModelTest::testMultiAdd()
{
    m_asyncModel->removeAllStatements();

    // start a bunch of commands...
    QList<QPointer<AsyncResult> > results;
    QList<Statement> data = createTestData( Statement(), 20 );
    foreach( const Statement& s, data ) {
        results.append( m_asyncModel->addStatementAsync( s ) );
    }

    // ...wait for them...
    foreach( QPointer<AsyncResult> r, results ) {
        qApp->processEvents();
        if ( r )
            AsyncResultWaiter::waitForResult( r );
    }

    // ...and then check for all the statements
    foreach( const Statement& s, data ) {
        QVERIFY( m_asyncModel->containsStatement( s ) );
    }


    Soprano::StatementIterator it = AsyncResultWaiter::waitForResult( m_asyncModel->listStatementsAsync() ).value<Soprano::StatementIterator>();
    int cnt = 0;
    while ( it.next() )
        ++cnt;
    QCOMPARE( cnt, data.count() );
}


QTEST_MAIN( AsyncModelTest )

#include "asyncmodeltest.moc"
