/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2006 Sebastian Trueg <strueg@mandriva.com>
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

#include "SopranoModelTest.h"
#include "stringpool.h"

#include "soprano.h"

#include <QtTest/QTest>
#include <QtCore/QList>
#include <QtCore/QDebug>
#include <QtCore/QTime>
#include <QtCore/QUrl>
#include <QtCore/QUuid>
#include <QtCore/QCoreApplication>
#include <QtTest/QSignalSpy>


// FIXME: Use the QTest framework to create data tables

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


SopranoModelTest::SopranoModelTest()
    : m_model( 0 ),
      m_features( BackendFeatureAll ),
      m_testSignals( true )
{
}

void SopranoModelTest::cleanup()
{
    deleteModel( m_model );
    m_model = 0;
}


void SopranoModelTest::deleteModel( Model* m )
{
    delete m;
}


void SopranoModelTest::init()
{
    m_model = createModel();
    QVERIFY( m_model != 0 );

    Node subject1( QUrl("http://soprano.sf.net#init:test1") );
    Node subject2( QUrl("http://soprano.sf.net#init:test2") );

    Node predicate1( QUrl( "http://soprano.sf.net#predicate1" ) );
    Node predicate2( QUrl( "http://soprano.sf.net#predicate2" ) );

    Node object1( LiteralValue( "Literal value1" ) );
    Node object2( LiteralValue( "Literal value2" ) );

    m_st1 = Statement(subject1, predicate1, object1);
    m_st2 = Statement(subject2, predicate1, object1);
    m_st3 = Statement(subject1, predicate2, object2);
    m_st4 = Statement(subject2, predicate2, object2);

    m_model->addStatement( m_st1 );
    m_model->addStatement( m_st2 );
    m_model->addStatement( m_st3 );
    m_model->addStatement( m_st4 );
}

void SopranoModelTest::testAddListOfStatement()
{
    QVERIFY( m_model != 0 );

    Node subject1( QUrl("http://soprano.sf.net#add:model") );

    Node predicate1( QUrl( "http://soprano.sf.net#predicate1" ) );
    Node predicate2( QUrl( "http://soprano.sf.net#predicate2" ) );
    Node predicate3( QUrl( "http://soprano.sf.net#predicate3" ) );

    Node object1( LiteralValue( "Literal value1" ) );

    Statement st1(subject1, predicate1, object1);
    Statement st2(subject1, predicate2, object1);
    Statement st3(subject1, predicate3, object1);

    QList<Statement> statements;
    statements.append( st1 );
    statements.append( st2 );
    statements.append( st3 );

    m_model->addStatements( statements );

    QVERIFY( m_model->containsStatement( st1 ) );
    QVERIFY( m_model->containsStatement( st2 ) );
    QVERIFY( m_model->containsStatement( st3 ) );
}


void SopranoModelTest::testAddStatements()
{
    QVERIFY( m_model != 0 );

    Node subject1( QUrl("http://soprano.sf.net#soprano:test1") );
    Node subject2( QUrl("http://soprano.sf.net#soprano:test2") );

    Node predicate1( QUrl( "http://soprano.sf.net#predicate1" ) );
    Node predicate2( QUrl( "http://soprano.sf.net#predicate2" ) );

    Node object1( LiteralValue( "Literal value1" ) );
    Node object2( LiteralValue( "Literal value2" ) );

    Statement st1(subject1, predicate1, object1);
    Statement st2(subject2, predicate1, object1);
    Statement st3(subject1, predicate2, object2);
    Statement st4(subject2, predicate2, object2);

    QVERIFY( m_model->addStatement( st1 ) == Soprano::Error::ErrorNone );
    QVERIFY( m_model->addStatement( st2 ) == Soprano::Error::ErrorNone );
    QVERIFY( m_model->addStatement( st3 ) == Soprano::Error::ErrorNone );
    QVERIFY( m_model->addStatement( st4 ) == Soprano::Error::ErrorNone );

    QVERIFY( m_model->addStatement( Statement() ) != Soprano::Error::ErrorNone );
}

void SopranoModelTest::testListStatements()
{
    QVERIFY( m_model != 0 );

    QList<Statement> statements;
    Node resource_1( QUrl("http://soprano.sf.net#list:resource1") );
    Node resource_2( QUrl("http://soprano.sf.net#list:resource2") );
    Node resource_3( QUrl("http://soprano.sf.net#list:resource3") );

    for (int i=0; i<50; i++)
    {
        QString property = "predicate" + QString::number(i);
        QString literal = "Literal value" + QString::number(i);

        Node predicate( QUrl( "http://soprano.sf.net#" + property) );
        Node object = LiteralValue( literal );

        Statement st(resource_1, predicate, object);
        statements.append( st );
    }

    for (int i=0; i<50; i++)
    {
        QString property = "predicate" + QString::number(i + 50);
        QString literal = "Literal value" + QString::number(i + 50);

        Node predicate( QUrl( "http://soprano.sf.net#" + property) );
        Node object = LiteralValue( literal );

        Statement st(resource_2, predicate, object);
        statements.append( st );
    }

    for (int i=0; i<20; i++)
    {
        QString property = "predicate" + QString::number(i + 100);
        QString literal = "Literal value" + QString::number(i + 100);

        Node predicate( QUrl( "http://soprano.sf.net#" + property) );
        Node object = LiteralValue( literal );

        Statement st(resource_3, predicate, object);
        statements.append( st );
    }

    QVERIFY( m_model->addStatements( statements ) == Error::ErrorNone );

    /* Resource 1 */

    StatementIterator it1 = m_model->listStatements( Statement( resource_1, Node(), Node() ) );

    int cnt = 0;
    while( it1.next() ) {
        ++cnt;
        Statement st = *it1;

        if ( st.subject() != resource_1 ) {
            qDebug() << st.subject() << "vs." << resource_1;
        }
        QCOMPARE( st.subject(), resource_1 );
    }

    QCOMPARE( cnt, 50 );

    /* Resource 2 */

    StatementIterator it2 = m_model->listStatements( Statement( resource_2, Node(), Node() ) );

    cnt = 0;
    while( it2.next() ) {
        ++cnt;
        Statement st = *it2;

        QCOMPARE( st.subject(), resource_2 );
    }

    QCOMPARE( cnt, 50 );

    /* Resource 3 */

    StatementIterator it3 = m_model->listStatements( Statement( resource_3, Node(), Node() ) );

    cnt = 0;
    while( it3.next() ) {
        ++cnt;
        Statement st = *it3;

        QCOMPARE( st.subject(), resource_3 );
    }

    QCOMPARE( cnt, 20 );

    /* All */

    StatementIterator it4 = m_model->listStatements();

    cnt = 0;
    while( it4.next() ) {
        ++cnt;
        Statement st = *it4;
        QVERIFY( statements.contains( st ) || st == m_st1 || st == m_st2 || st == m_st3 || st == m_st4 );
    }

    QCOMPARE( cnt, 124 );
}

void SopranoModelTest::testListStatementsWithContext()
{
    QVERIFY( m_model != 0 );

    // we do not want the normal test statements
    m_model->removeAllStatements();

    QList<Statement> statements;
    Node context1( QUrl("http://soprano.sf.net#list:resource1") );
    Node context2( QUrl("http://soprano.sf.net#list:resource2") );
    Node context3( QUrl("http://soprano.sf.net#list:resource3") );

    for (int i=0; i<10; i++)
    {
        QUrl subject = "http://soprano.sf.net#subject" + QString::number(i);
        QUrl predicate = "http://soprano.sf.net#predicate" + QString::number(i);
        LiteralValue object = "Literal value" + QString::number(i);

        statements.append( Statement( subject, predicate, object, context1 ) );
        statements.append( Statement( subject, predicate, object, context2 ) );
        statements.append( Statement( subject, predicate, object, context3 ) );
    }

    QVERIFY( m_model->addStatements( statements ) == Error::ErrorNone );

    // with context as wildcard
    StatementIterator it = m_model->listStatements( Statement( Node(), Node(), Node(), context1 ) );
    int cnt = 0;
    while( it.next() ) {
        ++cnt;
        Statement st = *it;
        QCOMPARE( st.context(), context1 );
    }

    QCOMPARE( cnt, 10 );


    // and the full context
    it = m_model->listStatementsInContext( context2 );
    cnt = 0;
    while( it.next() ) {
        ++cnt;
        Statement st = *it;
        QCOMPARE( st.context(), context2 );
    }

    QCOMPARE( cnt, 10 );
}

void SopranoModelTest::testRemoveStatement()
{
    QVERIFY( m_model != 0 );

    // test removing a single statement
    QVERIFY( m_model->removeStatement( m_st1 ) == Error::ErrorNone );
    QVERIFY( !m_model->lastError() );
    QVERIFY( !m_model->containsStatement( m_st1 ) );

    // make sure empty contexts are not used as wildcards
    Statement s1( m_st1 );
    Statement s2( s1 );
    s1.setContext( Node() );
    s2.setContext( QUrl( "http://soprano.org/test#context" ) );

    QVERIFY( m_model->addStatement( s1 ) == Error::ErrorNone );
    QVERIFY( m_model->addStatement( s2 ) == Error::ErrorNone );

    QVERIFY( m_model->removeStatement( s1 ) == Error::ErrorNone );
    QVERIFY( !m_model->lastError() );
    QVERIFY( !m_model->containsStatement( s1 ) );
    QVERIFY( !m_model->lastError() );
    QVERIFY( m_model->containsStatement( s2 ) );
    QVERIFY( !m_model->lastError() );

    // make sure invalid statements throw an error
    s1.setObject( Node() );
    QVERIFY( m_model->removeStatement( Statement( s1 ) ) != Error::ErrorNone );
    QVERIFY( m_model->lastError() );
}

void SopranoModelTest::testRemoveStatements()
{
    QVERIFY( m_model != 0 );

    // make sure empty contexts are used as wildcards
    Statement s1( m_st1 );
    Statement s2( s1 );
    Statement s3( s1 );
    s1.setContext( Node() );
    s2.setContext( QUrl( "http://soprano.org/test#context" ) );
    s3.setObject( LiteralValue( "Hello World" ) );

    QVERIFY( m_model->addStatement( s1 ) == Error::ErrorNone );
    QVERIFY( m_model->addStatement( s2 ) == Error::ErrorNone );
    QVERIFY( m_model->addStatement( s3 ) == Error::ErrorNone );

    QVERIFY( m_model->removeAllStatements( s1 ) == Error::ErrorNone );
    QVERIFY( !m_model->lastError() );
    QVERIFY( !m_model->containsStatement( s1 ) );
    QVERIFY( !m_model->lastError() );
    QVERIFY( !m_model->containsStatement( s2 ) );
    QVERIFY( !m_model->lastError() );

    QVERIFY( m_model->containsStatement( s3 ) );
    QVERIFY( !m_model->lastError() );

    // other wildcard nodes
    QVERIFY( m_model->removeAllStatements( Statement( s3.subject(), Node(), s3.object() ) ) == Error::ErrorNone );
    QVERIFY( !m_model->lastError() );
    QVERIFY( !m_model->containsStatement( s3 ) );
    QVERIFY( !m_model->lastError() );
}

void SopranoModelTest::testRemoveAllStatement()
{
    QVERIFY( m_model != 0 );

    m_model->removeAllStatements( Statement( m_st1.subject(), Node(), Node() ) );

    QVERIFY( !m_model->containsAnyStatement( m_st1 ) );
    QVERIFY( m_model->containsAnyStatement( m_st2 ) );
    QVERIFY( !m_model->containsAnyStatement( m_st3 ) );
    QVERIFY( m_model->containsAnyStatement( m_st4 ) );

    m_model->removeAllStatements( Statement( Node(), m_st3.predicate(), Node() ) );

    QVERIFY( !m_model->containsAnyStatement( m_st1 ) );
    QVERIFY( m_model->containsAnyStatement( m_st2 ) );
    QVERIFY( !m_model->containsAnyStatement( m_st3 ) );
    QVERIFY( !m_model->containsAnyStatement( m_st4 ) );

    m_model->removeAllStatements( Statement( Node(), Node(), m_st2.object() ) );

    QVERIFY( !m_model->containsAnyStatement( m_st1 ) );
    QVERIFY( !m_model->containsAnyStatement( m_st2 ) );
    QVERIFY( !m_model->containsAnyStatement( m_st3 ) );
    QVERIFY( !m_model->containsAnyStatement( m_st4 ) );
}


void SopranoModelTest::testContainsStatement()
{
    QVERIFY( m_model != 0 );

    Node res1( QUrl( "http://soprano.org/test#A" ) );
    Node res2( QUrl( "http://soprano.org/test#B" ) );
    Node res3( QUrl( "http://soprano.org/test#C" ) );
    Node res4( QUrl( "http://soprano.org/test#D" ) );

    Node pre( QUrl( "http://soprano.org/test#isRelated" ) );

    Node context1( QUrl( "http://soprano.org/test#contextA" ) );
    Node context2( QUrl( "http://soprano.org/test#contextB" ) );

    QVERIFY( m_model->addStatement( Statement( res1, pre, res2 ) ) == Error::ErrorNone );
    QVERIFY( m_model->addStatement( Statement( res1, pre, res3, context1 ) ) == Error::ErrorNone );
    QVERIFY( m_model->addStatement( Statement( res1, pre, res4, context2 ) ) == Error::ErrorNone );

    // check simple contains statement
    QVERIFY( m_model->containsStatement( Statement( res1, pre, res2 ) ) );
    QVERIFY( !m_model->lastError() );
    QVERIFY( m_model->containsStatement( Statement( res1, pre, res3, context1 ) ) );
    QVERIFY( !m_model->lastError() );
    QVERIFY( m_model->containsStatement( Statement( res1, pre, res4, context2 ) ) );
    QVERIFY( !m_model->lastError() );

    QVERIFY( !m_model->containsStatement( Statement( context2, res1, pre, res3 ) ) );
    QVERIFY( !m_model->lastError() );
    QVERIFY( !m_model->containsStatement( Statement( res1, pre, res4, context1 ) ) );
    QVERIFY( !m_model->lastError() );
    QVERIFY( !m_model->containsStatement( Statement( res1, pre, res3, context2 ) ) );
    QVERIFY( !m_model->lastError() );

    // make sure the context is not used as wildcard
    QVERIFY( !m_model->containsStatement( Statement( res1, pre, res3 ) ) );
    QVERIFY( !m_model->lastError() );
    QVERIFY( !m_model->containsStatement( Statement( res1, pre, res4 ) ) );
    QVERIFY( !m_model->lastError() );

    // make sure a wildcard statement throws an error.
    QVERIFY( !m_model->containsStatement( Statement( res1, pre, Node() ) ) );
    QVERIFY( m_model->lastError() );
}


void SopranoModelTest::testContainsStatements()
{
    QVERIFY( m_model != 0 );

    // check all wildcard combinations
    QVERIFY( m_model->containsAnyStatement( Statement( m_st1.subject(), Node(), Node() ) ) );
    QVERIFY( m_model->containsAnyStatement( Statement( Node(), m_st1.predicate(), Node() ) ) );
    QVERIFY( m_model->containsAnyStatement( Statement( Node(), Node(), m_st1.object() ) ) );

    // check context wildcards
    Statement s( QUrl( "http://soprano.org/test#A" ),
                 QUrl( "http://soprano.org/test#isRelated" ),
                 LiteralValue( "Hello World" ),
                 QUrl( "http://soprano.org/test#context" ) );
    QVERIFY( m_model->addStatement( s ) == Error::ErrorNone );

    QVERIFY( m_model->containsAnyStatement( Statement( s.subject(), Node(), Node() ) ) );
    QVERIFY( m_model->containsAnyStatement( Statement( Node(), s.predicate(), Node() ) ) );
    QVERIFY( m_model->containsAnyStatement( Statement( Node(), Node(), s.object() ) ) );
    QVERIFY( m_model->containsAnyStatement( Statement( s.subject(), Node(), Node(), s.context() ) ) );
    QVERIFY( m_model->containsAnyStatement( Statement( Node(), s.predicate(), Node(), s.context() ) ) );
    QVERIFY( m_model->containsAnyStatement( Statement( Node(), Node(), s.object(), s.context() ) ) );
    QVERIFY( m_model->containsAnyStatement( Statement( Node(), Node(), Node(), s.context() ) ) );

    QVERIFY( !m_model->containsAnyStatement( Statement( Node(), Node(), Node(), QUrl( "http://soprano.org/test#SomeOtherContext" ) ) ) );
}

void SopranoModelTest::testGraphQuery()
{
    if ( !( m_features&BackendFeatureQuery ) )
        return;

    QVERIFY( m_model != 0 );

    QString query( "CONSTRUCT { ?s ?p ?o } WHERE { ?s ?p ?o . }" );

    QueryResultIterator rs = m_model->executeQuery( query, Query::QueryLanguageSparql );
    QVERIFY( rs.isGraph() );
    QVERIFY( !rs.isBinding() );
    QVERIFY( !rs.isBool() );

    int cnt = 0;
    while ( rs.next() ) {
        ++cnt;
        Statement st = rs.currentStatement();
    }
    QCOMPARE ( cnt, 4 );
}

void SopranoModelTest::testBooleanQuery()
{
    if ( !( m_features&BackendFeatureQuery ) )
        return;

    QVERIFY( m_model != 0 );

    QString query( "ASK where {?a ?b ?c}" );

    QueryResultIterator res = m_model->executeQuery( query, Query::QueryLanguageSparql );
    QVERIFY( res.next() );
    QVERIFY( res.next() );

    QVERIFY( !res.isGraph() );
    QVERIFY( !res.isBinding() );
    QVERIFY( res.isBool() );

    QVERIFY( res.boolValue() );

    QVERIFY( res.next() );

    res.close();

    QVERIFY( !res.next() );
}


void SopranoModelTest::testInvalidQuery()
{
    if ( !( m_features&BackendFeatureQuery ) )
        return;

    QVERIFY( m_model != 0 );

    QueryResultIterator res = m_model->executeQuery( "INVALID query", Query::QueryLanguageSparql );
    QVERIFY( !res.isValid() );
}

void SopranoModelTest::testQuery()
{
    if ( !( m_features&BackendFeatureQuery ) )
        return;

    QVERIFY( m_model != 0 );

    /* SPARQL */
    QString sparql("select ?b ?c where {?a ?b ?c .}");

    QueryResultIterator rs1 = m_model->executeQuery( sparql, Query::QueryLanguageSparql );

    int cnt = 0;
    while ( rs1.next() )
    {
        QVERIFY( !rs1.isGraph() );
        QVERIFY( rs1.isBinding() );
        QVERIFY( !rs1.isBool() );

        ++cnt;
    }

    QVERIFY( cnt == 4 );

    // test bindings
    rs1 = m_model->executeQuery( sparql, Query::QueryLanguageSparql );
    qDebug() << rs1.bindingNames();
    while ( rs1.next() ) {
        QCOMPARE( rs1.bindingCount(), 2 );
        QCOMPARE( rs1.binding( 0 ), rs1.binding( "b" ) );
        QCOMPARE( rs1.binding( 1 ), rs1.binding( "c" ) );
        QCOMPARE( rs1.bindingNames().count(), 2 );
        QCOMPARE( rs1.bindingNames()[0], QString( "b" ) );
        QCOMPARE( rs1.bindingNames()[1], QString( "c" ) );
    }

    /* RDQL */

//     QueryLegacy rdql("select ?b ?c where (<http://soprano.sf.net#init:test1>, ?b, ?c)", QueryLegacy::RDQL);

//     QueryResultIterator rs2 = m_model->executeQuery( rdql );

//     int cnt2 = 0;
//     while ( rs2.next() )
//     {
//         QVERIFY( !rs2.isGraph() );
//         QVERIFY( rs2.isBinding() );
//         QVERIFY( !rs2.isBool() );

//         ++cnt2;
//     }

//     QVERIFY( cnt2 == 2 );
}

void SopranoModelTest::testCloseStatementIteratorOnModelDelete()
{
    QVERIFY( m_model != 0 );

    Node subject1( QUrl("http://soprano.sf.net#add:model") );

    Node predicate1( QUrl( "http://soprano.sf.net#predicate1" ) );
    Node predicate2( QUrl( "http://soprano.sf.net#predicate2" ) );
    Node predicate3( QUrl( "http://soprano.sf.net#predicate3" ) );

    Node object1( LiteralValue( "Literal value1" ) );

    Statement st1(subject1, predicate1, object1);
    Statement st2(subject1, predicate2, object1);
    Statement st3(subject1, predicate3, object1);

    Model *model = createModel();
    QVERIFY( model );

    model->addStatement( st1 );
    model->addStatement( st2 );

    StatementIterator it = model->listStatements();
    QVERIFY( it.next() );

    deleteModel( model );

    QVERIFY( !it.next() );
}

static bool checkSingleIt( StatementIterator it, const Statement& st )
{
    if ( it.next() ) {
        if ( *it != st ) {
            return false;
        }
        else {
            return !it.next();
        }
    }
    else {
        return false;
    }
}

static bool check3It( StatementIterator it, const Statement& s1, const Statement& s2, const Statement& s3 )
{
    int cnt = 0;
    bool haveS1 = false;
    bool haveS2 = false;
    bool haveS3 = false;
    while ( it.next() ) {
        Statement s = *it;
        if ( s == s1 )
            haveS1 = true;
        else if ( s == s2 )
            haveS2 = true;
        else if ( s == s3 )
            haveS3 = true;
        ++cnt;
    }

    return ( cnt == 3 && haveS1 && haveS2 && haveS3 );
}


void SopranoModelTest::testContexts()
{
    QVERIFY( m_model != 0 );

    Node subject1( QUrl( "http://soprano.sf.net#subject1" ) );
    Node subject2( QUrl( "http://soprano.sf.net#subject2" ) );
    Node subject3( QUrl( "http://soprano.sf.net#subject3" ) );

    Node predicate1( QUrl( "http://soprano.sf.net#predicate1" ) );
    Node predicate2( QUrl( "http://soprano.sf.net#predicate2" ) );
    Node predicate3( QUrl( "http://soprano.sf.net#predicate3" ) );

    LiteralValue object1( "literal 1" );
    LiteralValue object2( "literal 2" );
    LiteralValue object3( "literal 3" );

    Node context1( QUrl( "http://soprano.sf.net/contexts/context1" ) );
    Node context2( QUrl( "http://soprano.sf.net/contexts/context2" ) );
    Node context3( QUrl( "http://soprano.sf.net/contexts/context3" ) );

    Statement s1_c1( subject1, predicate1, object1, context1 );
    Statement s2_c1( subject1, predicate2, object1, context1 );
    Statement s3_c1( subject1, predicate3, object1, context1 );

    Statement s1_c2( subject1, predicate1, object2, context2 );
    Statement s2_c2( subject1, predicate2, object2, context2 );
    Statement s3_c2( subject1, predicate3, object2, context2 );

    Statement s1_c3( subject1, predicate1, object3, context3 );
    Statement s2_c3( subject1, predicate2, object3, context3 );
    Statement s3_c3( subject1, predicate3, object3, context3 );

    Statement s1_c0( subject1, predicate1, object3 );
    Statement s2_c0( subject1, predicate2, object3 );
    Statement s3_c0( subject1, predicate3, object3 );

    // add all the statements (do not add context3 yet, it is used below)
    QVERIFY( m_model->addStatement( s1_c1 ) == Error::ErrorNone );
    QVERIFY( m_model->addStatement( s2_c1 ) == Error::ErrorNone );
    QVERIFY( m_model->addStatement( s3_c1 ) == Error::ErrorNone );

    QVERIFY( m_model->addStatement( s1_c2 ) == Error::ErrorNone );
    QVERIFY( m_model->addStatement( s2_c2 ) == Error::ErrorNone );
    QVERIFY( m_model->addStatement( s3_c2 ) == Error::ErrorNone );

    QVERIFY( m_model->addStatement( s1_c0 ) == Error::ErrorNone );
    QVERIFY( m_model->addStatement( s2_c0 ) == Error::ErrorNone );
    QVERIFY( m_model->addStatement( s3_c0 ) == Error::ErrorNone );

    // check containsAnyStatement plain
    QVERIFY( m_model->containsAnyStatement( s1_c1 ) );
    QVERIFY( m_model->containsAnyStatement( s2_c1 ) );
    QVERIFY( m_model->containsAnyStatement( s3_c1 ) );

    QVERIFY( m_model->containsAnyStatement( s1_c2 ) );
    QVERIFY( m_model->containsAnyStatement( s2_c2 ) );
    QVERIFY( m_model->containsAnyStatement( s3_c2 ) );

    QVERIFY( m_model->containsAnyStatement( s1_c0 ) );
    QVERIFY( m_model->containsAnyStatement( s2_c0 ) );
    QVERIFY( m_model->containsAnyStatement( s3_c0 ) );

    // check containsAnyStatement with wildcard for context
    QVERIFY( m_model->containsAnyStatement( Statement( s1_c1.subject(), s1_c1.predicate(), s1_c1.object() ) ) );
    QVERIFY( m_model->containsAnyStatement( Statement( s2_c1.subject(), s2_c1.predicate(), s2_c1.object() ) ) );
    QVERIFY( m_model->containsAnyStatement( Statement( s3_c1.subject(), s3_c1.predicate(), s3_c1.object() ) ) );

    QVERIFY( m_model->containsAnyStatement( Statement( s1_c2.subject(), s1_c2.predicate(), s1_c2.object() ) ) );
    QVERIFY( m_model->containsAnyStatement( Statement( s2_c2.subject(), s2_c2.predicate(), s2_c2.object() ) ) );
    QVERIFY( m_model->containsAnyStatement( Statement( s3_c2.subject(), s3_c2.predicate(), s3_c2.object() ) ) );

    // check listStatements single
    checkSingleIt( m_model->listStatements( s1_c1 ), s1_c1 );
    checkSingleIt( m_model->listStatements( s2_c1 ), s2_c1 );
    checkSingleIt( m_model->listStatements( s3_c1 ), s3_c1 );

    checkSingleIt( m_model->listStatements( s1_c2 ), s1_c2 );
    checkSingleIt( m_model->listStatements( s2_c2 ), s2_c2 );
    checkSingleIt( m_model->listStatements( s3_c2 ), s3_c2 );

    checkSingleIt( m_model->listStatements( s1_c0 ), s1_c0 );
    checkSingleIt( m_model->listStatements( s2_c0 ), s2_c0 );
    checkSingleIt( m_model->listStatements( s3_c0 ), s3_c0 );

    // check listStatements with wildcard for object (one context)
    checkSingleIt( m_model->listStatements( Statement( s1_c1.subject(), s1_c1.predicate(), Node(), s1_c1.context() ) ), s1_c1 );
    checkSingleIt( m_model->listStatements( Statement( s2_c1.subject(), s2_c1.predicate(), Node(), s2_c1.context() ) ), s2_c1 );
    checkSingleIt( m_model->listStatements( Statement( s3_c1.subject(), s3_c1.predicate(), Node(), s3_c1.context() ) ), s3_c1 );

    checkSingleIt( m_model->listStatements( Statement( s1_c2.subject(), s1_c2.predicate(), Node(), s1_c2.context() ) ), s1_c2 );
    checkSingleIt( m_model->listStatements( Statement( s2_c2.subject(), s2_c2.predicate(), Node(), s2_c2.context() ) ), s2_c2 );
    checkSingleIt( m_model->listStatements( Statement( s3_c2.subject(), s3_c2.predicate(), Node(), s3_c2.context() ) ), s3_c2 );

    // the one without the context should return all three variants, i.e. 3 statements (different contexts)
    check3It( m_model->listStatements( Statement( s1_c0.subject(), s1_c0.predicate(), Node(), s1_c0.context() ) ),
              s1_c1, s1_c2, s1_c0 );
    check3It( m_model->listStatements( Statement( s2_c0.subject(), s2_c0.predicate(), Node(), s2_c0.context() ) ),
              s2_c1, s2_c2, s2_c0 );
    check3It( m_model->listStatements( Statement( s3_c0.subject(), s3_c0.predicate(), Node(), s3_c0.context() ) ),
              s1_c0, s3_c2, s3_c0 );

    // check remove context
    QVERIFY( m_model->removeContext( context1 ) == Error::ErrorNone );
    QVERIFY( !m_model->containsAnyStatement( s1_c1 ) );
    QVERIFY( !m_model->containsAnyStatement( s2_c1 ) );
    QVERIFY( !m_model->containsAnyStatement( s3_c1 ) );

    QVERIFY( m_model->containsAnyStatement( s1_c2 ) );
    QVERIFY( m_model->containsAnyStatement( s2_c2 ) );
    QVERIFY( m_model->containsAnyStatement( s3_c2 ) );

    QVERIFY( m_model->containsAnyStatement( s1_c0 ) );
    QVERIFY( m_model->containsAnyStatement( s2_c0 ) );
    QVERIFY( m_model->containsAnyStatement( s3_c0 ) );

    // check remove with context
    QVERIFY( m_model->removeAllStatements( s1_c2 ) == Error::ErrorNone );
    QVERIFY( !m_model->containsAnyStatement( s1_c2 ) );
    QVERIFY( m_model->containsAnyStatement( s2_c2 ) );
    QVERIFY( m_model->containsAnyStatement( s3_c2 ) );

    QVERIFY( m_model->containsAnyStatement( s1_c0 ) );
    QVERIFY( m_model->containsAnyStatement( s2_c0 ) );
    QVERIFY( m_model->containsAnyStatement( s3_c0 ) );

    // check remove without context
    QVERIFY( m_model->addStatement( s1_c3 ) == Error::ErrorNone );
    QVERIFY( m_model->addStatement( s2_c3 ) == Error::ErrorNone );
    QVERIFY( m_model->addStatement( s3_c3 ) == Error::ErrorNone );

    QVERIFY( m_model->containsAnyStatement( s1_c3 ) );
    QVERIFY( m_model->containsAnyStatement( s2_c3 ) );
    QVERIFY( m_model->containsAnyStatement( s3_c3 ) );

    QVERIFY( m_model->removeAllStatements( s1_c0 ) == Error::ErrorNone );

    QVERIFY( !m_model->containsAnyStatement( s1_c0 ) );
    QVERIFY( !m_model->containsAnyStatement( s1_c3 ) );
}


void SopranoModelTest::testListContexts()
{
    QVERIFY( m_model != 0 );

    // add some statements with contexts

    QList<Statement> statements;
    Node context1( QUrl("http://soprano.sf.net#list:resource1") );
    Node context2( QUrl("http://soprano.sf.net#list:resource2") );
    Node context3( QUrl("http://soprano.sf.net#list:resource3") );

    for (int i=0; i<10; i++)
    {
        QUrl subject = "http://soprano.sf.net#subject" + QString::number(i);
        QUrl predicate = "http://soprano.sf.net#predicate" + QString::number(i);
        LiteralValue object = "Literal value" + QString::number(i);

        statements.append( Statement( subject, predicate, object, context1 ) );
        statements.append( Statement( subject, predicate, object, context2 ) );
        statements.append( Statement( subject, predicate, object, context3 ) );
    }

    QVERIFY( m_model->addStatements( statements ) == Error::ErrorNone );

    NodeIterator it = m_model->listContexts();

    QList<Node> allContexts = it.allNodes();
    QCOMPARE( allContexts.count(), 3 );

    QVERIFY( allContexts.contains( context1 ) );
    QVERIFY( allContexts.contains( context2 ) );
    QVERIFY( allContexts.contains( context3 ) );
}


Q_DECLARE_METATYPE( Soprano::LiteralValue )

void SopranoModelTest::testLiteralTypes_data()
{
    QTest::addColumn<LiteralValue>( "literal" );
    QTest::addColumn<QUrl>( "predicate" );

    QString ns = "http://soprano.org/literalTest#";

    // plain literals
    QTest::newRow("plain") << LiteralValue::createPlainLiteral( "Hello World" ) << QUrl(ns + "plain");
    QTest::newRow("plainDe") << LiteralValue::createPlainLiteral( "Hallo Welt", "de" ) << QUrl(ns + "plainDe");

    // all the standard literal types
    QTest::newRow("intValue") << LiteralValue( 42 ) << QUrl(ns + "intValue");
    QTest::newRow("boolFalseValue") << LiteralValue( false ) << QUrl(ns + "boolFalseValue");
    QTest::newRow("boolTrueValue") << LiteralValue( true ) << QUrl(ns + "boolTrueValue");
    QTest::newRow("doubleValue") << LiteralValue( 42.3 ) << QUrl(ns + "doubleValue");
    QTest::newRow("dateValue") << LiteralValue( QDate::currentDate() ) << QUrl(ns + "dateValue");
    QTest::newRow("timeValue") << LiteralValue( QTime::currentTime() ) << QUrl(ns + "timeValue");
    QTest::newRow("dateTimeValue") << LiteralValue( QDateTime::currentDateTime() ) << QUrl(ns + "dateTimeValue");
    QTest::newRow("stringValue") << LiteralValue( "Hello World" ) << QUrl(ns + "stringValue");
    QTest::newRow("binaryValue") << LiteralValue( QByteArray( "Hello World" ) ) << QUrl( ns + "binaryValue" );

    // and now some utf8 encoding tests
    QTest::newRow("stringValueWithGermanUmlauts") << LiteralValue( StringPool::germanUmlauts() ) << QUrl(ns + "stringValueWithGermanUmlauts");
    QTest::newRow("stringValueWithFrenchAccents") << LiteralValue( StringPool::frenchAccents() ) << QUrl(ns + "stringValueWithFrenchAccents");
    QTest::newRow("stringValueWithRussianChars") << LiteralValue( StringPool::russianChars() ) << QUrl(ns + "stringValueWithRussianChars");

    // and some others
    QTest::newRow("stringWithSingleQuote" ) << LiteralValue( QString( "c'est" ) ) << QUrl( ns + "stringWithSingleQuote" );
    QTest::newRow("stringWith2SingleQuote" ) << LiteralValue( QString( "\'\'real" ) ) << QUrl( ns + "stringWith2SingleQuote" );
    QTest::newRow("stringWith3SingleQuote" ) << LiteralValue( QString( "\'\'\'real" ) ) << QUrl( ns + "stringWith3SingleQuote" );
    QTest::newRow("stringWithDoubleQuote" ) << LiteralValue( QString( "\"real" ) ) << QUrl( ns + "stringWithDoubleQuote" );
    QTest::newRow("stringWith2DoubleQuote" ) << LiteralValue( QString( "\"\"real" ) ) << QUrl( ns + "stringWith2DoubleQuote" );
    QTest::newRow("stringWith3DoubleQuote" ) << LiteralValue( QString( "\"\"\"real" ) ) << QUrl( ns + "stringWith3DoubleQuote" );
    QTest::newRow("stringWithTab" ) << LiteralValue( QString( "foo\tbar" ) ) << QUrl( ns + "stringWithTab" );
    QTest::newRow("stringWithNewLine" ) << LiteralValue( QString( "foo\nbar" ) ) << QUrl( ns + "stringWithNewLine" );
}


void SopranoModelTest::testLiteralTypes()
{
    QVERIFY( m_model != 0 );

    QFETCH( LiteralValue, literal );
    QFETCH( QUrl, predicate );

//    qDebug() << "testing literal: " << literal;

    QUrl sub( "http://soprano.org/literalTest#X" );

    QVERIFY( m_model->addStatement( Statement( sub, predicate, literal ) ) == Error::ErrorNone );

    StatementIterator it = m_model->listStatements( Statement( sub, predicate, Node() ) );
    QVERIFY( it.next() );
    QCOMPARE( it.current().object().literal(), literal );
    it.close();

    it = m_model->listStatements( Statement( Node(), Node(), literal ) );
    QVERIFY( it.next() );
    QCOMPARE( it.current().object().literal(), literal );
}


void SopranoModelTest::testUriEncoding_data()
{
    QTest::addColumn<QUrl>( "uri" );
    QTest::addColumn<QUrl>( "predicate" );

    QString ns = "http://soprano.org/uriEncodinglTest#";

    // a simple URI
    QTest::newRow("plain") << QUrl(ns + "simple") << QUrl(ns + "plain");

    // with some white space
    QTest::newRow("withSpace") << QUrl(ns + "URI with space") << QUrl(ns + "withSpace");

    // unicode
    QTest::newRow("germanUmlauts") << QUrl( ns + StringPool::germanUmlauts() ) << QUrl(ns + "germanUmlauts");
    QTest::newRow("frenchAccents") << QUrl( ns + StringPool::frenchAccents() ) << QUrl(ns + "frenchAccents");
    QTest::newRow("russianChars") << QUrl( ns + StringPool::russianChars() ) << QUrl(ns + "russianChars");
    QTest::newRow("withSingleQuote" ) << QUrl( ns + "c'est" ) << QUrl( ns + "singleQuote" );
    QTest::newRow("withDoubleQuote" ) << QUrl( ns + "\"real" ) << QUrl( ns + "doubleQuote" );
}


void SopranoModelTest::testUriEncoding()
{
    QVERIFY( m_model != 0 );

    QFETCH( QUrl, uri );
    QFETCH( QUrl, predicate );

//    qDebug() << "testing literal: " << literal;

    QUrl sub( "http://soprano.org/literalTest#X" );

    QVERIFY( m_model->addStatement( Statement( sub, predicate, uri ) ) == Error::ErrorNone );

    StatementIterator it = m_model->listStatements( Statement( sub, predicate, Node() ) );
    QVERIFY( it.next() );
    QCOMPARE( it.current().object().uri(), uri );
    it.close();

    it = m_model->listStatements( Statement( Node(), predicate, uri ) );
    QVERIFY( it.next() );
    QCOMPARE( it.current().subject().uri(), sub );
    it.close();
}


void SopranoModelTest::testIteratorNesting()
{
    QVERIFY( m_model );

    m_model->removeAllStatements();

    Node c1 = createRandomUri();
    Node c2 = createRandomUri();
    Node c3 = createRandomUri();
    Node c4 = createRandomUri();
    QList<Statement> sl1 = createTestData( Statement( Node(), Node(), Node(), c1 ), 5 );
    QList<Statement> sl2 = createTestData( Statement( Node(), Node(), Node(), c2 ), 5 );
    QList<Statement> sl3 = createTestData( Statement( Node(), Node(), Node(), c3 ), 5 );
    QList<Statement> sl4 = createTestData( Statement( Node(), Node(), Node(), c4 ), 5 );

    QVERIFY( m_model->addStatements( sl1 ) == Error::ErrorNone );
    QVERIFY( m_model->addStatements( sl2 ) == Error::ErrorNone );
    QVERIFY( m_model->addStatements( sl3 ) == Error::ErrorNone );
    QVERIFY( m_model->addStatements( sl4 ) == Error::ErrorNone );

    // test context + statement listing
    NodeIterator it1 = m_model->listContexts();
    int cnt1 = 0, cnt2 = 0, cnt3 = 0, cnt4 = 0;
    while ( it1.next() ) {
        StatementIterator it2 = m_model->listStatementsInContext( *it1 );
        while ( it2.next() ) {
            if ( ( *it2 ).context() == c1 ) {
                ++cnt1;
            }
            if ( ( *it2 ).context() == c2 ) {
                ++cnt2;
            }
            if ( ( *it2 ).context() == c3 ) {
                ++cnt3;
            }
            if ( ( *it2 ).context() == c4 ) {
                ++cnt4;
            }
        }
    }

    QCOMPARE( cnt1, sl1.count() );
    QCOMPARE( cnt2, sl2.count() );
    QCOMPARE( cnt3, sl3.count() );
    QCOMPARE( cnt4, sl4.count() );

    // test statement iterator nesting
    StatementIterator it3 = m_model->listStatements();
    while ( it3.next() ) {
        cnt1 = 0;
        if ( !it3.current().context().isEmpty() ) {
            StatementIterator it4 = m_model->listStatementsInContext( it3.current().context() );
            while ( it4.next() ) {
                ++cnt1;
            }

            QCOMPARE( cnt1, 5 );
        }
    }


    if ( !( m_features&BackendFeatureQuery ) )
        return;

    // test query iterator nesting
    cnt1 = 0;
    QueryResultIterator it4 = m_model->executeQuery( "select * where { graph ?g { ?r ?p ?o . } . }",
                                                     Query::QueryLanguageSparql );
    while ( it4.next() ) {
        ++cnt1;
        QUrl g = it4["g"].uri();
        QueryResultIterator it5 = m_model->executeQuery( QString( "select * where { graph %1 { ?r ?p ?o . } . }" ).arg( Node::resourceToN3( g ) ),
                                                         Query::QueryLanguageSparql );
        cnt2 = 0;
        while ( it5.next() )
            ++cnt2;
        QCOMPARE( cnt2, 5 );
    }
    QCOMPARE( cnt1, 20 );
}


Q_DECLARE_METATYPE( Soprano::Statement )

namespace {
    /**
     * Give it a bit of time for the DBus client to deliver the signals.
     */
    void waitForSignals() {
        for ( int i = 0; i < 10000; ++i ) {
            QCoreApplication::processEvents();
        }
    }
}

void SopranoModelTest::testStatementsAddedSignal()
{
    if ( !m_testSignals )
        return;

    QVERIFY( m_model );

    m_model->removeAllStatements();

    // clear out any dangling signals from previous calls
    waitForSignals();

    QSignalSpy spy( m_model, SIGNAL( statementsAdded() ) );

    m_model->addStatement( m_st1 );

    // let's give the dbus client model a chance to actually transport the signals
    waitForSignals();

    QCOMPARE( spy.count(), 1 );
}


void SopranoModelTest::testStatementAddedSignal()
{
    if ( !m_testSignals )
        return;

    QVERIFY( m_model );

    m_model->removeAllStatements();

    waitForSignals();

    qRegisterMetaType<Soprano::Statement>( "Soprano::Statement" );
    QSignalSpy spy( m_model, SIGNAL( statementAdded( const Soprano::Statement& ) ) );

    m_model->addStatement( m_st1 );
    m_model->addStatement( m_st2 );

    // let's give the dbus client model a chance to actually transport the signals
    waitForSignals();

    QCOMPARE( spy.count(), 2 );

    QList<QVariant> args = spy.takeFirst();
    QVERIFY( args.at( 0 ).value<Soprano::Statement>() == m_st1 );

    args = spy.takeFirst();
    QVERIFY( args.at( 0 ).value<Soprano::Statement>() == m_st2 );
}


void SopranoModelTest::testStatementsRemovedSignal()
{
    if ( !m_testSignals )
        return;

    QVERIFY( m_model );

    m_model->removeAllStatements();

    waitForSignals();

    QSignalSpy spy( m_model, SIGNAL( statementsRemoved() ) );

    m_model->addStatement( m_st1 );
    m_model->removeStatement( m_st1 );

    // let's give the dbus client model a chance to actually transport the signals
    waitForSignals();

    QCOMPARE( spy.count(), 1 );
    spy.clear();

    m_model->addStatement( m_st1 );
    m_model->removeAllStatements( m_st1.subject(), Node(), Node() );

    // let's give the dbus client model a chance to actually transport the signals
    waitForSignals();

    QCOMPARE( spy.count(), 1 );
}


void SopranoModelTest::testStatementRemovedSignal()
{
    if ( !m_testSignals )
        return;

    QVERIFY( m_model );

    m_model->removeAllStatements();

    waitForSignals();

    qRegisterMetaType<Soprano::Statement>( "Soprano::Statement" );
    QSignalSpy spy( m_model, SIGNAL( statementRemoved( const Soprano::Statement& ) ) );

    QVERIFY( m_model->addStatement( m_st1 ) == Error::ErrorNone );
    QVERIFY( m_model->addStatement( m_st2 ) == Error::ErrorNone );
    QVERIFY( m_model->removeStatement( m_st1 ) == Error::ErrorNone );
    QVERIFY( m_model->removeStatement( m_st2 ) == Error::ErrorNone );

    // let's give the dbus client model a chance to actually transport the signals
    waitForSignals();

    QCOMPARE( spy.count(), 2 );

    QList<QVariant> args = spy.takeFirst();
    QVERIFY( args.at( 0 ).value<Soprano::Statement>() == m_st1 );

    args = spy.takeFirst();
    QVERIFY( args.at( 0 ).value<Soprano::Statement>() == m_st2 );


    spy.clear();

    m_model->addStatement( m_st1 );
    m_model->removeAllStatements( m_st1.subject(), Node(), Node() );

    waitForSignals();

    QCOMPARE( spy.count(), 1 );

    args = spy.takeFirst();
    QVERIFY( m_st1.matches( args.at( 0 ).value<Soprano::Statement>() ) );
}


void SopranoModelTest::testPerformance()
{
    QVERIFY( m_model );

    m_model->removeAllStatements();

    QTime time;
    time.start();

    m_model->addStatement( m_st1 );
    int e = time.elapsed();
    qDebug() << "Time for adding one statement: " << e;

    QList<Statement> sl;
    for ( int i = 0; i < 1000; ++i ) {
        sl.append( Statement( Node::createResourceNode( QString( "http://soprano.org/test/resource%1" ).arg( i ) ),
                              Node::createResourceNode( QString( "http://soprano.org/test/property%1" ).arg( i ) ),
                              Node::createResourceNode( QString( "http://soprano.org/test/object%1" ).arg( i ) ) ) );
    }


    time.start();
    m_model->addStatements( sl );
    e = time.elapsed();
    qDebug() << "Time for adding a list of " << sl.count() << " statements: " << e;

    m_model->removeAllStatements();

    time.start();
    for ( QList<Statement>::const_iterator it = sl.constBegin(); it != sl.constEnd(); ++it ) {
        m_model->addStatement( *it );
    }
    e = time.elapsed();
    qDebug() << "Time for adding " << sl.count() << " statements one after the other: " << e;
}

#include "SopranoModelTest.moc"
