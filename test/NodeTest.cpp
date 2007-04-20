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

#include <QtTest>
#include <QtCore>

#include <soprano/node.h>

#include "NodeTest.h"

using namespace Soprano;

void NodeTest::testCreateEmptyNode()
{
  Node node;
  QVERIFY( node.type() == Node::EmptyNode );
  QVERIFY( !node.isValid() );
}

void NodeTest::testCreateResourceNode()
{
  QUrl uri("uri:soprano:test");
  Node node( uri );

  QCOMPARE( node.type(), Node::ResourceNode );
  QVERIFY( node.isValid() );
  QCOMPARE( node.uri(), uri );
  QVERIFY( node.literal().isEmpty() );
}

void NodeTest::testCreateLiteralNode()
{
  QString literal("Literal value");
  Node node( literal );

  QCOMPARE( node.type(), Node::LiteralNode );
  QVERIFY( node.isValid() );
  QCOMPARE( node.literal(), literal );
  QVERIFY( node.uri().isEmpty() );
}

void NodeTest::testCreateBlankNode()
{
  QUrl uri("uri:soprano:test");
  Node node( uri, Node::BlankNode );

  QCOMPARE( node.type(), Node::BlankNode );
  QVERIFY( node.isValid() );
  QCOMPARE( node.uri(), uri );
  QVERIFY( node.literal().isNull() );
}

QTEST_MAIN(NodeTest)

#include "NodeTest.moc"
