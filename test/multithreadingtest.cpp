/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007 Sebastian Trueg <strueg@mandriva.com>
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

#include "multithreadingtest.h"
#include "../soprano/soprano.h"

#include <QtTest/QtTest>
#include <QtCore/QUuid>
#include <QThread>


using namespace Soprano;


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


class TestingThread : public QThread
{
public:
    TestingThread( const QString& name )
        : m_model( 0 ),
          m_name( name ),
          m_success( true ) {
    }

    void start( Model* m ) {
        m_model = m;
        QThread::start();
    }

    bool verifyResult() {
        // wait max 2 seconds, all our operations should be done by then
        if ( !wait( 2000 ) ) {
            qDebug() << m_name << ": threading timeout: Model is blocked!";
            terminate();
            return false;
        }

        return m_success;
    }

    Model* model() const {
        return m_model;
    }

    QString name() const {
        return m_name;
    }

protected:
    void run() {
        m_success = performTest();
    }

    virtual bool performTest() = 0;

private:
    Model* m_model;
    QString m_name;
    bool m_success;
};


class AddStatementTest : public TestingThread
{
public:
    AddStatementTest( const Statement& partial = Statement(), int cnt = 5 )
        : TestingThread( "addStatement" ),
          m_partial( partial ),
          m_cnt( cnt ) {
    }

    bool performTest() {
        QList<Statement> data = createTestData( m_partial, m_cnt );
        if ( model()->addStatements( data ) != Error::ErrorNone ) {
            qDebug() << "Adding statements failed:" << model()->lastError();
            return false;
        }

        return true;
    }

private:
    Statement m_partial;
    int m_cnt;
};


class RemoveStatementTest : public TestingThread
{
public:
    RemoveStatementTest()
        : TestingThread( "removeStatement" ) {
    }

    bool performTest() {
        if ( model()->removeAllStatements() != Error::ErrorNone )
            return false;

        return !model()->listStatements().next();
    }
};


class QueryTest : public TestingThread
{
public:
    QueryTest( const QString& query )
        : TestingThread( "query" ),
          m_query( query ) {
    }

    bool performTest() {
        QueryResultIterator it = model()->executeQuery( m_query, Query::QueryLanguageSparql );

        return !it.lastError() && it.isValid() && it.next();
    }

private:
    QString m_query;
};


class ListStatementsTest : public TestingThread
{
public:
    ListStatementsTest( const QList<Statement>& sl )
        : TestingThread( "listStatements" ),
          m_statements( sl ) {
    }

    bool performTest() {
        StatementIterator it = model()->listStatements();
        int cnt = 0;
        while ( it.next() ) {
            ++cnt;
            if ( !m_statements.contains( *it ) ) {
                qDebug() << "invalid statement: " << *it;
                return false;
            }
        }
        if ( cnt == m_statements.count() ) {
            return true;
        }
        else {
            qDebug() << "Invalid count:" << cnt << "Expecting:" << m_statements.count();
            return false;
        }
    }

private:
    QList<Statement> m_statements;
};


void MultiThreadingTest::startAllTests( Model* m )
{
    Q_FOREACH( QThread* t, m_testThreads ) {
        dynamic_cast<TestingThread*>( t )->start( m );
    }
}


void MultiThreadingTest::verifyAllTests()
{
    Q_FOREACH( QThread* t, m_testThreads ) {
        if ( !dynamic_cast<TestingThread*>( t )->verifyResult() ) {
            qDebug() << dynamic_cast<TestingThread*>( t )->name() << "failed.";
        }
        QVERIFY( dynamic_cast<TestingThread*>( t )->verifyResult() );
    }
}


void MultiThreadingTest::initTestCase()
{
    m_testContext1 = createRandomUri();
    m_testContext2 = createRandomUri();
    m_testStatements += createTestData( Statement( Node(), Node(), Node(), m_testContext1 ), 5 );
    m_testStatements += createTestData( Statement( Node(), Node(), Node(), m_testContext2 ), 5 );

    m_testThreads.append( new ListStatementsTest( m_testStatements ) );
    m_testThreads.append( new QueryTest( "select * where { ?s ?p ?o . }" ) );
    m_testThreads.append( new QueryTest( "select * where { ?s ?p ?o . }" ) );
    m_testThreads.append( new AddStatementTest() );
    m_testThreads.append( new RemoveStatementTest() );
}


void MultiThreadingTest::cleanupTestCase()
{
    Q_FOREACH( QThread* t, m_testThreads ) {
        delete t;
    }
}


Q_DECLARE_METATYPE( TestingThread* )

void MultiThreadingTest::testNodeIterator_data()
{
    QTest::addColumn<TestingThread*>( "thread" );

    Q_FOREACH( QThread* t, m_testThreads ) {
        TestingThread* tt = dynamic_cast<TestingThread*>( t );
        QTest::newRow( tt->name().toLatin1().data() ) << tt;
    }
}


void MultiThreadingTest::testNodeIterator()
{
    QFETCH( TestingThread*, thread );

    Model* model = createModel();
    QVERIFY( model != 0 );

    // add some testdata with the same context
    QVERIFY( model->addStatements( m_testStatements ) == Error::ErrorNone );

    NodeIterator it = model->listContexts();

    // we start the thread with an open iterator
    thread->start( model );

    // now wait little to give the threads some time to mess things up (in case we have a bug)
    QTime t;
    t.start();
    while ( t.elapsed() < 200 );

    // now check the iterators, it should contain exactly those contexts that were in the
    // model when we called listContexts
    QList<Node> allContexts = it.allNodes();
    it.close();
    QCOMPARE( allContexts.count(), 2 );
    QVERIFY( allContexts.contains( m_testContext1 ) );
    QVERIFY( allContexts.contains( m_testContext2 ) );

    StatementIterator it2 = model->listStatements();
    while ( it2.next() ) {
        QCOMPARE( *it2, it2.current() );
    }
    QVERIFY( thread->verifyResult() );

    deleteModel( model );
}


void MultiThreadingTest::testAddStatement()
{
    Model* model = createModel();
    QVERIFY( model != 0 );

    AddStatementTest* t1 = new AddStatementTest( Statement( createRandomUri(), Node(), Node() ), 100 );
    AddStatementTest* t2 = new AddStatementTest( Statement( createRandomUri(), Node(), Node() ), 100 );
    AddStatementTest* t3 = new AddStatementTest( Statement( createRandomUri(), Node(), Node() ), 100 );
    AddStatementTest* t4 = new AddStatementTest( Statement( createRandomUri(), Node(), Node() ), 100 );

    t1->start( model );
    t2->start( model );
    t3->start( model );
    t4->start( model );

    t1->wait();
    t2->wait();
    t3->wait();
    t4->wait();

    QVERIFY( t1->verifyResult() );
    QVERIFY( t2->verifyResult() );
    QVERIFY( t3->verifyResult() );
    QVERIFY( t4->verifyResult() );

    delete t1;
    delete t2;
    delete t3;
    delete t4;

    deleteModel( model );
}


void MultiThreadingTest::deleteModel( Model* m )
{
    delete m;
}

#include "multithreadingtest.moc"
