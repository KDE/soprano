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

#include <QtTest>
#include <QtCore/QUuid>
#include <QThread>


using namespace Soprano;


static QUrl createRandomUri()
{
    // FIXME: check if the uri already exists
    QString uid = QUuid::createUuid().toString();
    uid = uid.mid( 1, uid.length()-2 );
    return QUrl( "inference://localhost#" + uid );
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
    AddStatementTest()
        : TestingThread( "addStatement" ) {
    }

    bool performTest() {
        QList<Statement> data = createTestData( Statement(), 5 );
        if ( model()->addStatements( data ) != Error::ERROR_NONE )
            return false;

        // check if the statements have really been added
        Q_FOREACH( Statement s, data ) {
            if( !model()->containsStatement( s ) )
                return false;
        }

        return true;
    }
};


class RemoveStatementTest : public TestingThread
{
public:
    RemoveStatementTest()
        : TestingThread( "removeStatement" ) {
    }

    bool performTest() {
        if ( model()->removeAllStatements() != Error::ERROR_NONE )
            return false;

        return !model()->listStatements().next();
    }
};


class QueryTest : public TestingThread
{
public:
    QueryTest( QueryLegacy query )
        : TestingThread( "query" ),
          m_query( query ) {
    }

    bool performTest() {
        QueryResultIterator it = model()->executeQuery( m_query );

        return !it.lastError() && it.isValid() && it.next();
    }

private:
    QueryLegacy m_query;
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
        QVERIFY( dynamic_cast<TestingThread*>( t )->verifyResult() );
    }
}


void MultiThreadingTest::initTestCase()
{
    m_testThreads.append( new AddStatementTest() );
    m_testThreads.append( new RemoveStatementTest() );
    m_testThreads.append( new QueryTest( QueryLegacy( "select * where { ?s ?p ?o . }", QueryLegacy::SPARQL ) ) );
}


void MultiThreadingTest::cleanupTestCase()
{
    Q_FOREACH( QThread* t, m_testThreads ) {
        delete t;
    }
}


Q_DECLARE_METATYPE( TestingThread* );

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
    QUrl context1 = createRandomUri();
    QUrl context2 = createRandomUri();
    model->addStatements( createTestData( Statement( Node(), Node(), Node(), context1 ), 5 ) );
    model->addStatements( createTestData( Statement( Node(), Node(), Node(), context2 ), 5 ) );

    NodeIterator it = model->listContexts();

    // we start the thread with an open iterator
    thread->start( model );

    // now wait little to give the threads some time to mess things up (in case we have a bug)
    QTime t;
    t.start();
    while ( t.elapsed() < 200 );

    // now check the iterator, it should contain exactly those contexts that were in the
    // model when we called listContexts
    QList<Node> allContexts = it.allNodes();
    it.close();
    StatementIterator it2 = model->listStatements();
    it2.close();
    QCOMPARE( allContexts.count(), 2 );
    QVERIFY( allContexts.contains( context1 ) );
    QVERIFY( allContexts.contains( context2 ) );

    QVERIFY( thread->verifyResult() );

    delete model;
}

#include "multithreadingtest.moc"
