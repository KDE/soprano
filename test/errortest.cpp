/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>
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

#include "errortest.h"
#include "../soprano/error.h"
#include "../soprano/locator.h"

#include <QtTest/QTest>
#include <QtCore/QDebug>

using namespace Soprano::Error;

void ErrorTest::testErrorCopy()
{
    Error e1( "e1", 2 );
    Error e2( e1 );

    QCOMPARE( e1.message(), QString( "e1" ) );
    QCOMPARE( e1.code(), 2 );

    QCOMPARE( e1.message(), e2.message() );
    QCOMPARE( e1.code(), e2.code() );
}

void ErrorTest::testOperator()
{
    Error e1( "e1", 2 );
    Error e2 = e1;

    QCOMPARE( e1.message(), e2.message() );
    QCOMPARE( e1.code(), e2.code() );

    Error error;
    error = Error( "test error" );

    QCOMPARE( error.message(), QString( "test error" ) );
    QCOMPARE( error.code(), ( int )error.code() );
}


static const int line = 4;
static const int col = 9;

void ErrorTest::testParserErrorCopy()
{
    ParserError p1( Locator( line, col ), "p1" );
    ParserError p2( p1 );

    QCOMPARE( p1.message(), QString( "p1" ) );
    QCOMPARE( p1.code(), ( int )Soprano::Error::ERROR_PARSING_FAILED );
    QCOMPARE( p1.locator().line(), line );
    QCOMPARE( p1.locator().column(), col );

    QCOMPARE( p1.message(), p2.message() );
    QCOMPARE( p1.code(), p2.code() );
    QCOMPARE( p1.locator().line(), p2.locator().line() );
    QCOMPARE( p1.locator().column(), p2.locator().column() );
}

void ErrorTest::testParserErrorOperator()
{
    ParserError p1( Locator( line, col ), "p1" );
    ParserError p2 = p1;

    QCOMPARE( p1.message(), QString( "p1" ) );
    QCOMPARE( p1.code(), ( int )Soprano::Error::ERROR_PARSING_FAILED );
    QCOMPARE( p1.locator().line(), line );
    QCOMPARE( p1.locator().column(), col );

    QCOMPARE( p1.message(), p2.message() );
    QCOMPARE( p1.code(), p2.code() );
    QCOMPARE( p1.locator().line(), p2.locator().line() );
    QCOMPARE( p1.locator().column(), p2.locator().column() );
}

void ErrorTest::testParserErrorConversion()
{
    ParserError p1( Locator( line, col ), "p1" );
    Error ep1 = p1;

    QCOMPARE( p1.message(), ep1.message() );
    QCOMPARE( p1.code(), ep1.code() );

    ParserError pep1 = ep1;

    QCOMPARE( p1.message(), pep1.message() );
    QCOMPARE( p1.code(), pep1.code() );
    QCOMPARE( p1.locator().line(), pep1.locator().line() );
    QCOMPARE( p1.locator().column(), pep1.locator().column() );
}

QTEST_MAIN( ErrorTest );

#include "errortest.moc"
