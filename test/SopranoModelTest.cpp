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
  Node subject1 = m_model->createResource( QUrl("uri:init:test1") );
  Node subject2 = m_model->createResource( QUrl("uri:init:test2") );

  Node predicate1 = m_model->createPredicate( "soprano", "predicate1" );
  Node predicate2 = m_model->createPredicate( "soprano", "predicate2" );

  Node object1 = m_model->createLiteral( "Literal value1" );
  Node object2 = m_model->createLiteral( "Literal value2" );

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
  Node subject1 = m_model->createResource( QUrl("uri:add:model") );

  Node predicate1 = m_model->createPredicate( "soprano", "predicate1" );
  Node predicate2 = m_model->createPredicate( "soprano", "predicate2" );
  Node predicate3 = m_model->createPredicate( "soprano", "predicate3" );

  Node object1 = m_model->createLiteral( "Literal value1" );

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

  delete memory;
}

void SopranoModelTest::testAddListOfStatement()
{
  Node subject1 = m_model->createResource( QUrl("uri:add:model") );

  Node predicate1 = m_model->createPredicate( "soprano", "predicate1" );
  Node predicate2 = m_model->createPredicate( "soprano", "predicate2" );
  Node predicate3 = m_model->createPredicate( "soprano", "predicate3" );

  Node object1 = m_model->createLiteral( "Literal value1" );

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
  Node subject1 = m_model->createResource( QUrl("uri:add:model") );

  Node predicate1 = m_model->createPredicate( "soprano", "predicate1" );
  Node predicate2 = m_model->createPredicate( "soprano", "predicate2" );
  Node predicate3 = m_model->createPredicate( "soprano", "predicate3" );

  Node object1 = m_model->createLiteral( "Literal value1" );

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
  Node subject1 = m_model->createResource( QUrl("uri:soprano:test1") );
  Node subject2 = m_model->createResource( QUrl("uri:soprano:test2") );

  Node predicate1 = m_model->createPredicate( "soprano", "predicate1" );
  Node predicate2 = m_model->createPredicate( "soprano", "predicate2" );

  Node object1 = m_model->createLiteral( "Literal value1" );
  Node object2 = m_model->createLiteral( "Literal value2" );

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
  Node resource_1 = m_model->createResource( QUrl("uri:list:resource1") );
  Node resource_2 = m_model->createResource( QUrl("uri:list:resource2") );
  Node resource_3 = m_model->createResource( QUrl("uri:list:resource3") );

  for (int i=0; i<50; i++)
  {
    QString property = "predicate" + QString::number(i);
    QString literal = "Literal value" + QString::number(i);

    Node predicate = m_model->createPredicate( "soprano", property);
    Node object = m_model->createLiteral( literal );

    Statement st(resource_1, predicate, object);
    statements.append( st );
  }

  for (int i=0; i<50; i++)
  {
    QString property = "predicate" + QString::number(i + 50);
    QString literal = "Literal value" + QString::number(i + 50);

    Node predicate = m_model->createPredicate( "soprano", property);
    Node object = m_model->createLiteral( literal );

    Statement st(resource_2, predicate, object);
    statements.append( st );
  }

  for (int i=0; i<20; i++)
  {
    QString property = "predicate" + QString::number(i + 100);
    QString literal = "Literal value" + QString::number(i + 100);

    Node predicate = m_model->createPredicate( "soprano", property);
    Node object = m_model->createLiteral( literal );

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
  Node subject = m_model->createResource( QUrl("uri:remove:3") );
  Node predicate = m_model->createPredicate( "soprano", "predicate" );
  Node object = m_model->createResource( QUrl("uri:soprano:2") );

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
  /*Query query("DESCRIBE ?a", Query::SPARQL);

  ResultSet rs = m_model->executeQuery( query );
  QVERIFY( !rs.next() );

  QVERIFY( rs.isGraph() );
  QVERIFY( !rs.isBinding() );
  QVERIFY( !rs.isBool() );

  Model *model = rs.model();
  QVERIFY( model != 0L);

  StatementIterator *iter = model->listStatements();
  int cnt = 0;
  while ( iter->hasNext() )
  {
    ++cnt;
    Statement st = iter->next();
  }
  QVERIFY ( cnt == 2 );

  delete iter;
  delete model;*/
}

void SopranoModelTest::testBooleanQuery()
{
  Query query("ASK {?a ?b ?c}", Query::SPARQL);

  ResultSet res = m_model->executeQuery( query );
  QVERIFY( res.next() );

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
  Node subject1 = m_model->createResource( QUrl("uri:add:model") );

  Node predicate1 = m_model->createPredicate( "soprano", "predicate1" );
  Node predicate2 = m_model->createPredicate( "soprano", "predicate2" );
  Node predicate3 = m_model->createPredicate( "soprano", "predicate3" );

  Node object1 = m_model->createLiteral( "Literal value1" );

  Statement st1(subject1, predicate1, object1);
  Statement st2(subject1, predicate2, object1);
  Statement st3(subject1, predicate3, object1);

  Model *model = Soprano::createModel();
  model->add( st1 );
  model->add( st2 );

  StatementIterator it = model->listStatements();
  int cnt = 0;
  while( it.hasNext() )
  {
    it.next();
    cnt++;
  }

  delete model;

 QVERIFY( cnt == 2 );

  QVERIFY( !it.hasNext() );
  QVERIFY( !it.next().isValid() );
}

#include "SopranoModelTest.moc"
