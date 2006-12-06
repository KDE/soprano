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

using namespace Soprano;

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
  QUrl subUri = QUrl("uri:soprano:test1");
  StatementIterator* it = m_model->listStatements( Statement( Node(subUri), Node(), Node() ) );
  QVERIFY( it != 0 );

  int cnt = 0;
  while( it->hasNext() ) {
    ++cnt;
    Statement st = it->next();

    qDebug() << st.subject().uri();

    QCOMPARE( st.subject().uri(), subUri );
  }

  QCOMPARE( cnt, 2 );

  delete it;
}

void SopranoTest::testRemoveStatement()
{
  Node subject = m_model->createResource( QUrl("uri:soprano:3") );
  Node predicate = m_model->createPredicate( "soprano", "predicate" );
  Node object = m_model->createResource( QUrl("uri:soprano:2") );
  
  Statement st(subject, predicate, object);
  m_model->add( st );

  QVERIFY( m_model->contains(st) );

  m_model->remove( st );

  QVERIFY( !m_model->contains(st) );
}

#include "SopranoTest.moc"
