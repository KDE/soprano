/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
 * Copyright (C) 2009 Sebastian Trueg <trueg@kde.org>
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

#include <QtTest/QtTest>
#include <QtCore/QtCore>

#include "node.h"
#include "../soprano/vocabulary/rdf.h"

#include "NodeTest.h"

Q_DECLARE_METATYPE( Soprano::Node )

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
    QVERIFY( !node.literal().isValid() );
}

void NodeTest::testCreateLiteralNode()
{
    QString literal("Literal value");
    LiteralValue lv( literal );
    Node node( lv );

    QCOMPARE( node.type(), Node::LiteralNode );
    QVERIFY( node.isValid() );
    QCOMPARE( node.literal().toString(), literal );
    QVERIFY( !node.literal().isPlain() );
    QVERIFY( node.uri().isEmpty() );

    Node germanNode( LiteralValue::createPlainLiteral( "Hallo Welt", "de" ) );
    QCOMPARE( germanNode.type(), Node::LiteralNode );
    QVERIFY( germanNode.isValid() );
    QVERIFY( germanNode.literal().isPlain() );
    QCOMPARE( germanNode.literal().toString(), QString( "Hallo Welt" )  );
    QVERIFY( germanNode.uri().isEmpty() );
    QCOMPARE( germanNode.language(), QString( "de" ) ); // Backwards compatibility

    germanNode = LiteralValue::createPlainLiteral( "Hallo Welt", "de" );
    QCOMPARE( germanNode.type(), Node::LiteralNode );
    QVERIFY( germanNode.isValid() );
    QVERIFY( germanNode.literal().isPlain() );
    QCOMPARE( germanNode.literal().toString(), QString( "Hallo Welt" )  );
    QVERIFY( germanNode.uri().isEmpty() );
    QCOMPARE( germanNode.language(), QString( "de" ) ); // Backwards compatibility
}

void NodeTest::testCreateBlankNode()
{
    QString id("uri:soprano:test");
    Node node( id );

    QCOMPARE( node.type(), Node::BlankNode );
    QVERIFY( node.isValid() );
    QCOMPARE( node.identifier(), id );
    QVERIFY( !node.literal().isValid() );
}


void NodeTest::testToN3_data()
{
    QTest::addColumn<Node>( "node" );
    QTest::addColumn<QString>( "n3" );

    QTest::newRow( "plain literal" )
        << Node( LiteralValue::createPlainLiteral("Hello World") )
        << QString("\"Hello World\"");

    QTest::newRow( "string literal" )
        << Node( LiteralValue("Hello World") )
        << QString("\"Hello World\"^^<http://www.w3.org/2001/XMLSchema#string>");

    QTest::newRow( "date literal" )
        << Node( LiteralValue( QDate( 2009, 10, 26 ) ) )
        << QString("\"2009-10-26\"^^<http://www.w3.org/2001/XMLSchema#date>");

    QTest::newRow( "time literal" )
        << Node( LiteralValue( QTime(1, 1, 1) ) )
        << QString("\"01:01:01Z\"^^<http://www.w3.org/2001/XMLSchema#time>");

    QTest::newRow( "datetime literal" )
        << Node( LiteralValue( QDateTime( QDate(2009, 1, 8), QTime(1, 1, 1), Qt::UTC ) ) )
        << QString("\"2009-01-08T01:01:01Z\"^^<http://www.w3.org/2001/XMLSchema#dateTime>");

    QTest::newRow( "datetime literal 24h" )
        << Node( LiteralValue( QDateTime( QDate(2009, 11, 8), QTime(14, 35, 12, 243), Qt::UTC ) ) )
        << QString("\"2009-11-08T14:35:12.243Z\"^^<http://www.w3.org/2001/XMLSchema#dateTime>");

    QTest::newRow( "simple uri" )
        << Node(Vocabulary::RDF::type())
        << QString( "<%1>" ).arg( Vocabulary::RDF::type().toString() );

    QTest::newRow( "uri full of junk" )
        << Node( QUrl::fromLocalFile( "/home/someone/hello (World), it's me.txt" ) )
        << QString( "<file:///home/someone/hello%20(World),%20it's%20me.txt>" );
}

void NodeTest::testToN3()
{
    QFETCH( Node, node );
    QFETCH( QString, n3 );

    QCOMPARE( node.toN3(), n3 );
}

QTEST_MAIN(NodeTest)

