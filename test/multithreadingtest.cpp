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
    TestingThread( Model* m, const QString& name )
        : m_model( m ),
          m_name( name ),
          m_success( true ) {
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
    AddStatementTest( Model* m )
        : TestingThread( m, "addStatement" ) {
    }

    bool performTest() {
        QList<Statement> data = createTestData( Statement(), 5 );
        if ( model()->addStatements( data ) != ERROR_NONE )
            return false;

        // check if the statements have really been added
        Q_FOREACH( Statement s, data ) {
            if( !model()->containsStatements( s ) )
                return false;
        }

        return true;
    }
};


void MultiThreadingTest::testNodeIterator()
{
    Model* model = createModel();
    QVERIFY( model != 0 );

    // add some testdata with the same context
    QUrl context = createRandomUri();
    QList<Statement> data = createTestData( Statement( Node(), Node(), Node(), context ), 5 );
    model->addStatements( data );

    NodeIterator it = model->listContexts();

    // we start the thread with an open iterator
    AddStatementTest t1( model );
    t1.start();

    // now check the iterator, it should contain exactly those contexts that were in the
    // model when we called listContexts
    QList<Node> allContexts = it.allNodes();
    QCOMPARE( 1, allContexts.count() );
    QVERIFY( allContexts[0] == context );

    QVERIFY( t1.verifyResult() );

    delete model;
}


#include "multithreadingtest.moc"
