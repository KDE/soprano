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
#include <soprano/soprano.h>

#include <QtTest/QTest>
#include <QtCore/QList>
#include <QtCore/QDebug>


// FIXME: Use the QTest framework to create data tables

using namespace Soprano;

void SopranoModelTest::cleanup()
{
  delete m_st1;
  delete m_st2;
  delete m_st3;
  delete m_st4;

  QVERIFY( m_model->removeAll() == ERROR_NONE );
}

void SopranoModelTest::init()
{
  Node subject1( QUrl("uri:init:test1") );
  Node subject2( QUrl("uri:init:test2") );

  Node predicate1( QUrl( "soprano#predicate1" ) );
  Node predicate2( QUrl( "soprano#predicate2" ) );

  Node object1( "Literal value1" );
  Node object2( "Literal value2" );

  m_st1 = new Statement(subject1, predicate1, object1);
  m_st2 = new Statement(subject2, predicate1, object1);
  m_st3 = new Statement(subject1, predicate2, object2);
  m_st4 = new Statement(subject2, predicate2, object2);

  m_model->add( *m_st1 );
  m_model->add( *m_st2 );
  m_model->add( *m_st3 );
  m_model->add( *m_st4 );
}

void SopranoModelTest::testAddModel()
{
  Node subject1( QUrl("uri:add:model") );

  Node predicate1( QUrl( "soprano#predicate1" ) );
  Node predicate2( QUrl( "soprano#predicate2" ) );
  Node predicate3( QUrl( "soprano#predicate3" ) );

  Node object1( "Literal value1" );

  Statement st1(subject1, predicate1, object1);
  Statement st2(subject1, predicate2, object1);
  Statement st3(subject1, predicate3, object1);

  Model *memory = Soprano::createModel();
  memory->add( st1 );
  memory->add( st2 );

  m_model->add( *memory );

  QVERIFY( m_model->contains( st1 ) );
  QVERIFY( m_model->contains( st2 ) );
  QVERIFY( !m_model->contains( st3 ) );

  // partial statement match
  Statement stPartial1( subject1, Node(), object1 );
  Statement stPartial2( subject1, Node(), Node() );
  Statement stPartial3;
  Statement stPartial4( subject1, Node(), QUrl( "soprao#notInTheStore" ) );

  QVERIFY( m_model->contains( stPartial1 ) );
  QVERIFY( m_model->contains( stPartial2 ) );
  QVERIFY( m_model->contains( stPartial3 ) );
  QVERIFY( !m_model->contains( stPartial4 ) );

  delete memory;
}

void SopranoModelTest::testAddListOfStatement()
{
  Node subject1( QUrl("uri:add:model") );

  Node predicate1( QUrl( "soprano#predicate1" ) );
  Node predicate2( QUrl( "soprano#predicate2" ) );
  Node predicate3( QUrl( "soprano#predicate3" ) );

  Node object1( "Literal value1" );

  Statement st1(subject1, predicate1, object1);
  Statement st2(subject1, predicate2, object1);
  Statement st3(subject1, predicate3, object1);

  QList<Statement> statements;
  statements.append( st1 );
  statements.append( st2 );
  statements.append( st3 );

  m_model->add( statements );

  QVERIFY( m_model->contains( st1 ) );
  QVERIFY( m_model->contains( st2 ) );
  QVERIFY( m_model->contains( st3 ) );
}

void SopranoModelTest::testAddStatementIterator()
{
  Node subject1( QUrl("uri:add:model") );

  Node predicate1( QUrl( "soprano#predicate1" ) );
  Node predicate2( QUrl( "soprano#predicate2" ) );
  Node predicate3( QUrl( "soprano#predicate3" ) );

  Node object1( "Literal value1" );

  Statement st1(subject1, predicate1, object1);
  Statement st2(subject1, predicate2, object1);
  Statement st3(subject1, predicate3, object1);

  Model *memory = Soprano::createModel();
  memory->add( st1 );
  memory->add( st2 );

  m_model->add( memory->listStatements() );

  QVERIFY( m_model->contains( st1 ) );
  QVERIFY( m_model->contains( st2 ) );
  QVERIFY( !m_model->contains( st3 ) );

  delete memory;
}

void SopranoModelTest::testAddStatements()
{
  Node subject1( QUrl("uri:soprano:test1") );
  Node subject2( QUrl("uri:soprano:test2") );

  Node predicate1( QUrl( "soprano#predicate1" ) );
  Node predicate2( QUrl( "soprano#predicate2" ) );

  Node object1( "Literal value1" );
  Node object2( "Literal value2" );

  Statement st1(subject1, predicate1, object1);
  Statement st2(subject2, predicate1, object1);
  Statement st3(subject1, predicate2, object2);
  Statement st4(subject2, predicate2, object2);

  QVERIFY( m_model->add( st1 ) == Soprano::ERROR_NONE );
  QVERIFY( m_model->add( st2 ) == Soprano::ERROR_NONE );
  QVERIFY( m_model->add( st3 ) == Soprano::ERROR_NONE );
  QVERIFY( m_model->add( st4 ) == Soprano::ERROR_NONE );
}

void SopranoModelTest::testListStatements()
{
  QList<Statement> statements;
  Node resource_1( QUrl("uri:list:resource1") );
  Node resource_2( QUrl("uri:list:resource2") );
  Node resource_3( QUrl("uri:list:resource3") );

  for (int i=0; i<50; i++)
  {
    QString property = "predicate" + QString::number(i);
    QString literal = "Literal value" + QString::number(i);

    Node predicate( QUrl( "soprano#" + property) );
    Node object = LiteralValue( literal );

    Statement st(resource_1, predicate, object);
    statements.append( st );
  }

  for (int i=0; i<50; i++)
  {
    QString property = "predicate" + QString::number(i + 50);
    QString literal = "Literal value" + QString::number(i + 50);

    Node predicate( QUrl( "soprano#" + property) );
    Node object = LiteralValue( literal );

    Statement st(resource_2, predicate, object);
    statements.append( st );
  }

  for (int i=0; i<20; i++)
  {
    QString property = "predicate" + QString::number(i + 100);
    QString literal = "Literal value" + QString::number(i + 100);

    Node predicate( QUrl( "soprano#" + property) );
    Node object = LiteralValue( literal );

    Statement st(resource_3, predicate, object);
    statements.append( st );
  }

  m_model->add( statements );

  /* Resource 1 */

  StatementIterator it1 = m_model->listStatements( Statement( resource_1, Node(), Node() ) );

  int cnt = 0;
  while( it1.hasNext() ) {
    ++cnt;
    Statement st = it1.next();

    QCOMPARE( st.subject(), resource_1 );
  }

  QCOMPARE( cnt, 50 );

  /* Resource 2 */

  StatementIterator it2 = m_model->listStatements( Statement( resource_2, Node(), Node() ) );

  cnt = 0;
  while( it2.hasNext() ) {
    ++cnt;
    Statement st = it2.next();

    QCOMPARE( st.subject(), resource_2 );
  }

  QCOMPARE( cnt, 50 );

  /* Resource 3 */

  StatementIterator it3 = m_model->listStatements( Statement( resource_3, Node(), Node() ) );

  cnt = 0;
  while( it3.hasNext() ) {
    ++cnt;
    Statement st = it3.next();

    QCOMPARE( st.subject(), resource_3 );
  }

  QCOMPARE( cnt, 20 );

  /* All */

  StatementIterator it4 = m_model->listStatements();

  cnt = 0;
  while( it4.hasNext() ) {
    ++cnt;
    Statement st = it4.next();

    QVERIFY( statements.indexOf( st ) != -1 || st == *m_st1 || st == *m_st2 || st == *m_st3 || st == *m_st4 );
  }

  QCOMPARE( cnt, 124 );
}

void SopranoModelTest::testRemoveStatement()
{
  Node subject( QUrl("uri:remove:3") );
  Node predicate( QUrl( "soprano#predicate" ) );
  Node object( QUrl("uri:soprano:2") );

  Statement st(subject, predicate, object);
  m_model->add( st );

  QVERIFY( m_model->contains(st) );

  m_model->remove( st );

  QVERIFY( !m_model->contains(st) );
}

void SopranoModelTest::testRemoveAllStatement()
{
  m_model->removeAll( m_st1->subject(), Node(), Node() );

  QVERIFY( !m_model->contains( *m_st1 ) );
  QVERIFY( m_model->contains( *m_st2 ) );
  QVERIFY( !m_model->contains( *m_st3 ) );
  QVERIFY( m_model->contains( *m_st4 ) );

  m_model->removeAll( Node(), m_st3->predicate(), Node() );

  QVERIFY( !m_model->contains( *m_st1 ) );
  QVERIFY( m_model->contains( *m_st2 ) );
  QVERIFY( !m_model->contains( *m_st3 ) );
  QVERIFY( !m_model->contains( *m_st4 ) );

  m_model->removeAll( Node(), Node(), m_st2->object() );

  QVERIFY( !m_model->contains( *m_st1 ) );
  QVERIFY( !m_model->contains( *m_st2 ) );
  QVERIFY( !m_model->contains( *m_st3 ) );
  QVERIFY( !m_model->contains( *m_st4 ) );
}

void SopranoModelTest::testGraphQuery()
{
    Query query("CONSTRUCT { ?s ?p ?o } WHERE { ?s ?p ?o . }", Query::SPARQL);

    ResultSet rs = m_model->executeQuery( query );
    QVERIFY( rs.isGraph() );
    QVERIFY( !rs.isBinding() );
    QVERIFY( !rs.isBool() );

    // test the model creation
    Model *model = rs.model();
    QVERIFY( model != 0L);

    StatementIterator iter = model->listStatements();
    int cnt = 0;
    while ( iter.hasNext() ) {
        ++cnt;
        Statement st = iter.next();
    }
    QCOMPARE ( cnt, 4 );

    delete model;

    // test the iteration
    rs = m_model->executeQuery( query );
    cnt = 0;
    while ( rs.next() ) {
        ++cnt;
        Statement st = rs.currentStatement();
    }
    QCOMPARE( cnt, 4 );
}

void SopranoModelTest::testBooleanQuery()
{
  Query query("ASK {?a ?b ?c}", Query::SPARQL);

  ResultSet res = m_model->executeQuery( query );
  QVERIFY( !res.next() );

  QVERIFY( !res.isGraph() );
  QVERIFY( !res.isBinding() );
  QVERIFY( res.isBool() );

  QVERIFY( res.boolValue() );

  QVERIFY( !res.next() );
}

void SopranoModelTest::testQuery()
{
  /* SPARQL */
  Query sparql("select ?b ?c where {?a ?b ?c .}", Query::SPARQL);

  ResultSet rs1 = m_model->executeQuery( sparql );

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
  rs1 = m_model->executeQuery( sparql );
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

  Query rdql("select ?b ?c where (<uri:init:test1>, ?b, ?c)", Query::RDQL);

  ResultSet rs2 = m_model->executeQuery( rdql );

  int cnt2 = 0;
  while ( rs2.next() )
  {
    QVERIFY( !rs2.isGraph() );
    QVERIFY( rs2.isBinding() );
    QVERIFY( !rs2.isBool() );

    ++cnt2;
  }

  QVERIFY( cnt2 == 2 );
}

void SopranoModelTest::testCloseStatementIteratorOnModelDelete()
{
  Node subject1( QUrl("uri:add:model") );

  Node predicate1( QUrl( "soprano#predicate1" ) );
  Node predicate2( QUrl( "soprano#predicate2" ) );
  Node predicate3( QUrl( "soprano#predicate3" ) );

  Node object1( "Literal value1" );

  Statement st1(subject1, predicate1, object1);
  Statement st2(subject1, predicate2, object1);
  Statement st3(subject1, predicate3, object1);

  Model *model = Soprano::createModel();
  model->add( st1 );
  model->add( st2 );

  StatementIterator it = model->listStatements();
  QVERIFY( it.hasNext() );

  delete model;

  QVERIFY( !it.hasNext() );
  QVERIFY( !it.next().isValid() );
}

static bool checkSingleIt( StatementIterator it, const Statement& st )
{
    if ( it.hasNext() ) {
        if ( it.next() != st ) {
            return false;
        }
        else {
            return !it.hasNext();
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
    while ( it.hasNext() ) {
        Statement s = it.next();
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
    Node subject1( QUrl( "soprano#subject1" ) );
    Node subject2( QUrl( "soprano#subject2" ) );
    Node subject3( QUrl( "soprano#subject3" ) );

    Node predicate1( QUrl( "soprano#predicate1" ) );
    Node predicate2( QUrl( "soprano#predicate2" ) );
    Node predicate3( QUrl( "soprano#predicate3" ) );

    Node object1( "literal 1" );
    Node object2( "literal 2" );
    Node object3( "literal 3" );

    Node context1( QUrl( "soprano::context1" ) );
    Node context2( QUrl( "soprano::context2" ) );
    Node context3( QUrl( "soprano::context3" ) );

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
    QVERIFY( m_model->add( s1_c1 ) == ERROR_NONE );
    QVERIFY( m_model->add( s2_c1 ) == ERROR_NONE );
    QVERIFY( m_model->add( s3_c1 ) == ERROR_NONE );

    QVERIFY( m_model->add( s1_c2 ) == ERROR_NONE );
    QVERIFY( m_model->add( s2_c2 ) == ERROR_NONE );
    QVERIFY( m_model->add( s3_c2 ) == ERROR_NONE );

    QVERIFY( m_model->add( s1_c0 ) == ERROR_NONE );
    QVERIFY( m_model->add( s2_c0 ) == ERROR_NONE );
    QVERIFY( m_model->add( s3_c0 ) == ERROR_NONE );

    // check contains plain
    QVERIFY( m_model->contains( s1_c1 ) );
    QVERIFY( m_model->contains( s2_c1 ) );
    QVERIFY( m_model->contains( s3_c1 ) );

    QVERIFY( m_model->contains( s1_c2 ) );
    QVERIFY( m_model->contains( s2_c2 ) );
    QVERIFY( m_model->contains( s3_c2 ) );

    QVERIFY( m_model->contains( s1_c0 ) );
    QVERIFY( m_model->contains( s2_c0 ) );
    QVERIFY( m_model->contains( s3_c0 ) );

    // check contains with wildcard for context
    QVERIFY( m_model->contains( Statement( s1_c1.subject(), s1_c1.predicate(), s1_c1.object() ) ) );
    QVERIFY( m_model->contains( Statement( s2_c1.subject(), s2_c1.predicate(), s2_c1.object() ) ) );
    QVERIFY( m_model->contains( Statement( s3_c1.subject(), s3_c1.predicate(), s3_c1.object() ) ) );

    QVERIFY( m_model->contains( Statement( s1_c2.subject(), s1_c2.predicate(), s1_c2.object() ) ) );
    QVERIFY( m_model->contains( Statement( s2_c2.subject(), s2_c2.predicate(), s2_c2.object() ) ) );
    QVERIFY( m_model->contains( Statement( s3_c2.subject(), s3_c2.predicate(), s3_c2.object() ) ) );

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
    QVERIFY( m_model->remove( context1 ) == ERROR_NONE );
    QVERIFY( !m_model->contains( s1_c1 ) );
    QVERIFY( !m_model->contains( s2_c1 ) );
    QVERIFY( !m_model->contains( s3_c1 ) );

    QVERIFY( m_model->contains( s1_c2 ) );
    QVERIFY( m_model->contains( s2_c2 ) );
    QVERIFY( m_model->contains( s3_c2 ) );

    QVERIFY( m_model->contains( s1_c0 ) );
    QVERIFY( m_model->contains( s2_c0 ) );
    QVERIFY( m_model->contains( s3_c0 ) );

    // check remove with context
    QVERIFY( m_model->remove( s1_c2 ) == ERROR_NONE );
    QVERIFY( !m_model->contains( s1_c2 ) );
    QVERIFY( m_model->contains( s2_c2 ) );
    QVERIFY( m_model->contains( s3_c2 ) );

    QVERIFY( m_model->contains( s1_c0 ) );
    QVERIFY( m_model->contains( s2_c0 ) );
    QVERIFY( m_model->contains( s3_c0 ) );

    // check remove without context
    QVERIFY( m_model->add( s1_c3 ) == ERROR_NONE );
    QVERIFY( m_model->add( s2_c3 ) == ERROR_NONE );
    QVERIFY( m_model->add( s3_c3 ) == ERROR_NONE );

    QVERIFY( m_model->contains( s1_c3 ) );
    QVERIFY( m_model->contains( s2_c3 ) );
    QVERIFY( m_model->contains( s3_c3 ) );

    QVERIFY( m_model->removeAll( s1_c0 ) == ERROR_NONE );

    QVERIFY( !m_model->contains( s1_c0 ) );
    QVERIFY( !m_model->contains( s1_c3 ) );
}

#include "SopranoModelTest.moc"
