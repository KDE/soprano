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

#include "SopranoTest.h"
#include <soprano/Soprano.h>

#include <QtCore>
#include <QList>

using namespace Soprano;

void SopranoTest::cleanup()
{
  delete m_st1;
  delete m_st2;
  delete m_st3;
  delete m_st4;

  QVERIFY( m_model->removeAll() == Model::SUCCESS_EXIT );
}

void SopranoTest::init()
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

void SopranoTest::testAddModel()
{
  Node subject1 = m_model->createResource( QUrl("uri:add:model") );

  Node predicate1 = m_model->createPredicate( "soprano", "predicate1" );
  Node predicate2 = m_model->createPredicate( "soprano", "predicate2" );
  Node predicate3 = m_model->createPredicate( "soprano", "predicate3" );

  Node object1 = m_model->createLiteral( "Literal value1" );

  Statement st1(subject1, predicate1, object1);
  Statement st2(subject1, predicate2, object1);
  Statement st3(subject1, predicate3, object1);

  Model *memory = m_factory->createMemoryModel( "add-model" );
  memory->add( st1 );
  memory->add( st2 );
 
  m_model->add( *memory );

  QVERIFY( m_model->contains( st1 ) );
  QVERIFY( m_model->contains( st2 ) );
  QVERIFY( !m_model->contains( st3 ) );
}

void SopranoTest::testAddListOfStatement()
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

void SopranoTest::testAddStatementIterator()
{
  Node subject1 = m_model->createResource( QUrl("uri:add:model") );

  Node predicate1 = m_model->createPredicate( "soprano", "predicate1" );
  Node predicate2 = m_model->createPredicate( "soprano", "predicate2" );
  Node predicate3 = m_model->createPredicate( "soprano", "predicate3" );

  Node object1 = m_model->createLiteral( "Literal value1" );

  Statement st1(subject1, predicate1, object1);
  Statement st2(subject1, predicate2, object1);
  Statement st3(subject1, predicate3, object1);

  Model *memory = m_factory->createMemoryModel( "add-model" );
  memory->add( st1 );
  memory->add( st2 );
 
  m_model->add( *memory->listStatements() );

  QVERIFY( m_model->contains( st1 ) );
  QVERIFY( m_model->contains( st2 ) );
  QVERIFY( !m_model->contains( st3 ) );
}

void SopranoTest::testAddStatements()
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

  QVERIFY( m_model->add( st1 ) == Soprano::Model::SUCCESS_EXIT );
  QVERIFY( m_model->add( st2 ) == Soprano::Model::SUCCESS_EXIT );
  QVERIFY( m_model->add( st3 ) == Soprano::Model::SUCCESS_EXIT );
  QVERIFY( m_model->add( st4 ) == Soprano::Model::SUCCESS_EXIT );
}

void SopranoTest::testListStatements()
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

  StatementIterator* it = m_model->listStatements( Statement( resource_1, Node(), Node() ) );
  QVERIFY( it != 0 );

  int cnt = 0;
  while( it->hasNext() ) {
    ++cnt;
    Statement st = it->next();

    QCOMPARE( st.subject(), resource_1 );
  }

  QCOMPARE( cnt, 50 );

  delete it;

  /* Resource 2 */

  it = m_model->listStatements( Statement( resource_2, Node(), Node() ) );
  QVERIFY( it != 0 );

  cnt = 0;
  while( it->hasNext() ) {
    ++cnt;
    Statement st = it->next();

    QCOMPARE( st.subject(), resource_2 );
  }

  QCOMPARE( cnt, 50 );

  delete it;

  /* Resource 3 */

  it = m_model->listStatements( Statement( resource_3, Node(), Node() ) );
  QVERIFY( it != 0 );

  cnt = 0;
  while( it->hasNext() ) {
    ++cnt;
    Statement st = it->next();

    QCOMPARE( st.subject(), resource_3 );
  }

  QCOMPARE( cnt, 20 );

  delete it;

  /* All */

  it = m_model->listStatements();
  QVERIFY( it != 0 );

  cnt = 0;
  while( it->hasNext() ) {
    ++cnt;
    Statement st = it->next();

    QVERIFY( statements.indexOf( st ) != -1 || st == *m_st1 || st == *m_st2 || st == *m_st3 || st == *m_st4 );
  }

  QCOMPARE( cnt, 124 );

  delete it;
}

void SopranoTest::testRemoveStatement()
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

void SopranoTest::testRemoveAllStatement()
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

void SopranoTest::testGraphQuery()
{
  Query query("DESCRIBE <uri:init:test1>", Query::SPARQL);

  QueryResult *qr = m_model->executeQuery( query );
  QVERIFY( qr != 0L);

  QVERIFY( qr->isGraph() );
  QVERIFY( !qr->isBinding() );
  QVERIFY( !qr->isBool() );
}

void SopranoTest::testBooleanQuery()
{
  Query query("ASK {?uri <soprano#predicate1> \"Literal value1\"}", Query::SPARQL);

  QueryResult *qr = m_model->executeQuery( query );
  QVERIFY( qr != 0L);

  QVERIFY( !qr->isGraph() );
  QVERIFY( !qr->isBinding() );
  QVERIFY( qr->isBool() );
}

void SopranoTest::testQuery()
{
  /* SPARQL */
  Query sparql("select ?b ?c where {<uri:init:test1> ?b ?c .}", Query::SPARQL);

  QueryResult *qr = m_model->executeQuery( sparql );
  QVERIFY( qr != 0L);

  QVERIFY( !qr->isGraph() );
  QVERIFY( qr->isBinding() );
  QVERIFY( !qr->isBool() );

  int cnt = 0;
  QueryResultIterator iter( qr );
  while ( iter.hasNext() )
  {
    ++cnt;
    iter.next();
  }

  QVERIFY( cnt == 2 );

  /* RDQL */

  Query rdql("select ?b ?c where (<uri:init:test1>, ?b, ?c)", Query::RDQL);

  QueryResult *qr2 = m_model->executeQuery( rdql );
  QVERIFY( qr2 != 0L);

  QVERIFY( !qr2->isGraph() );
  QVERIFY( qr2->isBinding() );
  QVERIFY( !qr2->isBool() );

  int cnt2 = 0;
  QueryResultIterator iter2( qr2 );
  while ( iter2.hasNext() )
  {
    ++cnt2;
    iter2.next();
  }

  QVERIFY( cnt2 == 2 );
}

#include "SopranoTest.moc"
