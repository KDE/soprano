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

#include "soprano/Node.h"

#include "NodeTest.h"

using namespace Soprano;

void NodeTest::testCreateEmptyNode()
{
  Node node;
  QVERIFY( node.type() == Node::Unknown );
  QVERIFY( !node.isValid() );
}

void NodeTest::testCreateResourceNode()
{
  QUrl uri("uri:soprano:test");
  Node node( uri );
  
  QVERIFY( node.type() == Node::Resource );
  QVERIFY( node.isValid() );
  QVERIFY( node.uri() == uri );
  QVERIFY( node.literal().isNull() );
  QVERIFY( node.blank().isNull() );
}

void NodeTest::testCreateLiteralNode()
{
  QString literal("Literal value");
  Node node( literal, Node::Literal );
  
  QVERIFY( node.type() == Node::Literal );
  QVERIFY( node.isValid() );
  QVERIFY( node.literal() == literal );
  QVERIFY( node.blank().isNull() );
  QVERIFY( !node.uri().isValid() );
}

void NodeTest::testCreateBlankNode()
{
  QUrl uri("uri:soprano:test");
  Node node(uri.toString(), Node::Blank);
  
  QVERIFY( node.type() == Node::Blank );
  QVERIFY( node.isValid() );
  QVERIFY( node.blank() == uri.toString() );
  QVERIFY( node.literal().isNull() );  
  QVERIFY( !node.uri().isValid() );
}

QTEST_MAIN(NodeTest)

#include "NodeTest.moc"
