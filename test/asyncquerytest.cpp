/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2010 Sebastian Trueg <trueg@k3b.org>
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

#include "asyncquerytest.h"
#include "asyncquery.h"
#include "soprano.h"

#include <QEventLoop>
#include <QTimer>

using namespace Soprano;
using namespace Soprano::Util;

AsyncQueryHandler::AsyncQueryHandler( Soprano::Util::AsyncQuery* query )
    : m_query( query ),
      m_nextLoop( 0 ),
      m_finishedLoop( 0 ),
      m_nextReady( false ),
      m_finished( false )
{
    connect( query, SIGNAL(nextReady(Soprano::Util::AsyncQuery*)),
             this, SLOT(slotNextReady()) );
    connect( query, SIGNAL(finished(Soprano::Util::AsyncQuery*)),
             this, SLOT(slotFinished()) );
}

bool AsyncQueryHandler::waitForNext()
{
    if ( m_finished ) {
        return false;
    }
    m_nextReady = false;
    QEventLoop loop;
    m_nextLoop = &loop;
    QTimer::singleShot( 20000, &loop, SLOT(quit()) );
    loop.exec();
    m_nextLoop = 0;
    return m_nextReady;
}

bool AsyncQueryHandler::waitForFinished()
{
    if ( m_finished ) {
        return true;
    }
    m_finished = false;
    QEventLoop loop;
    m_finishedLoop = &loop;
    QTimer::singleShot( 20000, &loop, SLOT(quit()) );
    loop.exec();
    m_finishedLoop = 0;
    return m_finished;
}

void AsyncQueryHandler::slotNextReady()
{
    m_nextReady = true;
    if ( m_nextLoop ) {
        m_nextLoop->exit();
    }
}

void AsyncQueryHandler::slotFinished()
{
    setError(m_query->lastError());
    m_finished = true;
    if ( m_finishedLoop ) {
        m_finishedLoop->exit();
    }
    if ( m_nextLoop ) {
        m_nextLoop->exit();
        m_nextReady = false;
    }
}



void AsyncQueryTest::testBindingNames()
{
    Model* model = Soprano::createModel();
    QVERIFY( model != 0 );

    model->addStatement( QUrl( "test:/A" ), QUrl( "test:/B" ), QUrl( "test:/C" ) );

    QString query = QString::fromLatin1( "select ?r ?p ?o where { ?r ?p ?o . } limit 1" );
    AsyncQuery* q = AsyncQuery::executeQuery( model, query, Query::QueryLanguageSparql );
    QVERIFY( q );
    AsyncQueryHandler h( q );
    QVERIFY( h.waitForNext() );
    QStringList bindings = q->bindingNames();
    QCOMPARE( q->bindingCount(), 3 );
    QCOMPARE( bindings.count(), 3 );
    QVERIFY( bindings.contains( QLatin1String( "r" ) ) );
    QVERIFY( bindings.contains( QLatin1String( "p" ) ) );
    QVERIFY( bindings.contains( QLatin1String( "o" ) ) );

    delete model;
}


void AsyncQueryTest::testQueryType()
{
    Model* model = Soprano::createModel();
    QVERIFY( model != 0 );
    model->addStatement( QUrl( "test:/A" ), QUrl( "test:/B" ), QUrl( "test:/C" ) );

    QString query = QString::fromLatin1( "select ?r ?p ?o where { ?r ?p ?o . }" );
    AsyncQuery* q = AsyncQuery::executeQuery( model, query, Query::QueryLanguageSparql );
    QVERIFY( q );
    AsyncQueryHandler h( q );
    QVERIFY( h.waitForNext() );
    QVERIFY( q->isBinding() );
    QVERIFY( !q->isGraph() );
    QVERIFY( !q->isBool() );
    q->close();
    h.waitForFinished();

    query = QString::fromLatin1( "construct { ?r ?p ?o } where { ?r ?p ?o . }" );
    q = AsyncQuery::executeQuery( model, query, Query::QueryLanguageSparql );
    QVERIFY( q );
    AsyncQueryHandler h2( q );
    QVERIFY( h2.waitForNext() );
    QVERIFY( !q->isBinding() );
    QVERIFY( q->isGraph() );
    QVERIFY( !q->isBool() );
    q->close();
    h2.waitForFinished();

    query = QString::fromLatin1( "ask where { ?r ?p ?o . }" );
    q = AsyncQuery::executeQuery( model, query, Query::QueryLanguageSparql );
    QVERIFY( q );
    AsyncQueryHandler h3( q );
    QVERIFY( h3.waitForFinished() );
    QVERIFY( !q->isBinding() );
    QVERIFY( !q->isGraph() );
    QVERIFY( q->isBool() );

    delete model;
}


void AsyncQueryTest::testBindingsQuery()
{
    Model* model = Soprano::createModel();
    QVERIFY( model != 0 );
    model->addStatement( QUrl( "test:/A" ), QUrl( "test:/B" ), QUrl( "test:/C" ) );
    model->addStatement( QUrl( "test:/AA" ), QUrl( "test:/B" ), QUrl( "test:/C" ) );
    model->addStatement( QUrl( "test:/AAA" ), QUrl( "test:/B" ), QUrl( "test:/C" ) );
    model->addStatement( QUrl( "test:/AAAA" ), QUrl( "test:/B" ), QUrl( "test:/C" ) );

    QString query = QString::fromLatin1( "select ?r where { ?r ?p ?o . }" );
    AsyncQuery* q = AsyncQuery::executeQuery( model, query, Query::QueryLanguageSparql );
    QVERIFY( q );
    AsyncQueryHandler h( q );

    QList<Node> nodes;

    QVERIFY( h.waitForNext() );
    nodes << q->binding( 0 );
    q->next();
    QVERIFY( h.waitForNext() );
    nodes << q->binding( 0 );
    q->next();
    QVERIFY( h.waitForNext() );
    nodes << q->binding( 0 );
    q->next();
    QVERIFY( h.waitForNext() );
    nodes << q->binding( 0 );
    q->next();
    QVERIFY( h.waitForFinished() );

    QueryResultIterator it = model->executeQuery( query, Query::QueryLanguageSparql );
    int i = 0;
    while ( it.next() ) {
        QCOMPARE( nodes[i++], it[0] );
    }
    QCOMPARE( i, 4 );

    delete model;
}


void AsyncQueryTest::testError()
{
    Model* model = Soprano::createModel();
    QVERIFY( model != 0 );

    // use an invalid query
    const QString query = QLatin1String("I am an invalid query - I will eat your cat.");
    AsyncQuery* q = AsyncQuery::executeQuery( model, query, Query::QueryLanguageSparql );
    QVERIFY( q );
    AsyncQueryHandler h( q );
    QVERIFY( h.waitForFinished() );
    QVERIFY(h.lastError());
}

QTEST_MAIN( AsyncQueryTest )

