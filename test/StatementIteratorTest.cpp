/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
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

#include "StatementIteratorTest.h"
#include "soprano.h"

#include <QtTest/QTest>
#include <QtCore/QList>

using namespace Soprano;

void StatementIteratorTest::cleanup()
{
    delete m_model;
    m_model = 0;
}

void StatementIteratorTest::init()
{
    m_model = createModel();
    QVERIFY( m_model != 0 );

    m_model->addStatements( m_statements );
}

void StatementIteratorTest::initTestCase()
{
    Node resource_1( QUrl("uri:list:resource1") );
    Node resource_2( QUrl("uri:list:resource2") );
    Node resource_3( QUrl("uri:list:resource3") );

    for (int i=0; i<50; i++)
    {
        QString property = "predicate" + QString::number(i);
        QString literal = "Literal value" + QString::number(i);

        Node predicate( QUrl( "soprano#" + property ) );
        Node object = LiteralValue( literal );

        Statement st(resource_1, predicate, object);
        m_statements.append( st );
    }

    for (int i=0; i<50; i++)
    {
        QString property = "predicate" + QString::number(i + 50);
        QString literal = "Literal value" + QString::number(i + 50);

        Node predicate( QUrl( "soprano#" + property ) );
        Node object = LiteralValue( literal );

        Statement st(resource_2, predicate, object);
        m_statements.append( st );
    }

    for (int i=0; i<20; i++)
    {
        QString property = "predicate" + QString::number(i + 100);
        QString literal = "Literal value" + QString::number(i + 100);

        Node predicate( QUrl( "soprano#" + property ) );
        Node object = LiteralValue( literal );

        Statement st(resource_3, predicate, object);
        m_statements.append( st );
    }
}

void StatementIteratorTest::testIterator()
{
    Node resource_1( QUrl("uri:list:resource1") );
    Node resource_2( QUrl("uri:list:resource2") );
    Node resource_3( QUrl("uri:list:resource3") );

    StatementIterator sti = m_model->listStatements();

    int cnt = 0;
    while ( sti.next() ) {
        ++cnt;
    }

    QVERIFY( cnt == m_statements.size() );

    /* Resource 1 */

    StatementIterator sti2 = m_model->listStatements( Statement( resource_1, Node(), Node() ) );

    cnt = 0;
    while ( sti2.next() ) {
        ++cnt;
    }

    QVERIFY( cnt == 50 );
}

void StatementIteratorTest::testSharedStuffs()
{
    StatementIterator iter1 = m_model->listStatements();

    int cnt = 0;
    if ( iter1.next() ) {
        ++cnt;
    }

    StatementIterator iter2 = iter1;
    while ( iter2.next() ) {
        ++cnt;
    }

    QCOMPARE( cnt, m_statements.size() );
}


void StatementIteratorTest::testConsistency()
{
    StatementIterator it = m_model->listStatements();

    while ( it.next() ) {
        QCOMPARE( *it, it.current() );
    }
}

#include "StatementIteratorTest.moc"
