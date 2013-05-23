/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2010 Sebastian Trueg <trueg@kde.org>
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

#include "n3nodeparsertest.h"
#include "node.h"
#include "literalvalue.h"

#include <QtTest/QtTest>

Q_DECLARE_METATYPE( Soprano::Node )

using namespace Soprano;

void N3NodeParserTest::testToFromN3_data()
{
    QTest::addColumn<Soprano::Node>( "node" );

    QTest::newRow( "empty" ) << Node();
    QTest::newRow( "resource" ) << Node( QUrl( "http://soprano.test.org#A" ) );
    QTest::newRow( "blank" ) << Node::createBlankNode( "blank" );
    QTest::newRow( "plain literal" ) << Node( LiteralValue::createPlainLiteral( "Hello World" ) );
    QTest::newRow( "plain literal de" ) << Node( LiteralValue::createPlainLiteral( "Hallo Welt", "de" ) );
    QTest::newRow( "types literal string" ) << Node( LiteralValue( "Hello World" ) );
    QTest::newRow( "types literal int" ) << Node( LiteralValue( 42 ) );
    QTest::newRow( "types literal double" ) << Node( LiteralValue( 42.2 ) );
    QTest::newRow( "types literal string with escaped" ) << Node( LiteralValue( "Hello \"World\"" ) );
}


void N3NodeParserTest::testToFromN3()
{
    QFETCH( Node, node );

    QCOMPARE( node, Node::fromN3( node.toN3() ) );
}


QTEST_MAIN( N3NodeParserTest )

