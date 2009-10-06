/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2009 Sebastian Trueg <trueg@k3b.org>
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

#include "datetimetest.h"
#include "datetime.h"

void DateTimeTest::testFromTimeString_data()
{
    QTest::addColumn<QString>( "string" );
    QTest::addColumn<QTime>( "time" );

    // Virtuoso skips the 'Z', thus we need to be able to parse those, too
    QTest::newRow("01:01:01.000Z") << QString( "01:01:01.000Z" ) << QTime(1, 1, 1);
    QTest::newRow("01:01:01.000") << QString( "01:01:01.000" ) << QTime(1, 1, 1);
    QTest::newRow("01:01:01Z") << QString( "01:01:01Z" ) << QTime(1, 1, 1);
    QTest::newRow("01:01:01") << QString( "01:01:01" ) << QTime(1, 1, 1);

    QTest::newRow("14:35:12.243Z") << QString( "14:35:12.243Z" ) << QTime(14, 35, 12, 243);
    QTest::newRow("14:35:12.243") << QString( "14:35:12.243" ) << QTime(14, 35, 12, 243);
    QTest::newRow("14:35:12Z") << QString( "14:35:12Z" ) << QTime(14, 35, 12, 0);
    QTest::newRow("14:35:12") << QString( "14:35:12" ) << QTime(14, 35, 12, 0);
}


void DateTimeTest::testFromTimeString()
{
    QFETCH( QString, string );
    QFETCH( QTime, time );

    QTime parsedTime = Soprano::DateTime::fromTimeString( string );
    QCOMPARE( parsedTime, time );
}


void DateTimeTest::testFromDateString_data()
{
    QTest::addColumn<QString>( "string" );
    QTest::addColumn<QDate>( "date" );

    QTest::newRow("2009-01-08") << QString( "2009-01-08" ) << QDate(2009, 1, 8);
    QTest::newRow("2009-11-08") << QString( "2009-11-08" ) << QDate(2009, 11, 8);
}


void DateTimeTest::testFromDateString()
{
    QFETCH( QString, string );
    QFETCH( QDate, date );

    QDate parsedDate = Soprano::DateTime::fromDateString( string );
    QCOMPARE( parsedDate, date );
}


void DateTimeTest::testFromDateTimeString_data()
{
    QTest::addColumn<QString>( "string" );
    QTest::addColumn<QDateTime>( "datetime" );

    QTest::newRow("2009-01-08T01:01:01.000Z") << QString( "2009-01-08T01:01:01.000Z" ) << QDateTime( QDate(2009, 1, 8), QTime(1, 1, 1), Qt::UTC );
    QTest::newRow("2009-01-08T01:01:01.000") << QString( "2009-01-08T01:01:01.000" ) << QDateTime( QDate(2009, 1, 8), QTime(1, 1, 1), Qt::UTC );
    QTest::newRow("2009-01-08T01:01:01Z") << QString( "2009-01-08T01:01:01Z" ) << QDateTime( QDate(2009, 1, 8), QTime(1, 1, 1), Qt::UTC );
    QTest::newRow("2009-01-08T01:01:01") << QString( "2009-01-08T01:01:01" ) << QDateTime( QDate(2009, 1, 8), QTime(1, 1, 1), Qt::UTC );

    QTest::newRow("2009-11-08T14:35:12.243Z") << QString( "2009-11-08T14:35:12.243Z" ) << QDateTime( QDate(2009, 11, 8), QTime(14, 35, 12, 243), Qt::UTC );
    QTest::newRow("2009-11-08T14:35:12.243") << QString( "2009-11-08T14:35:12.243" ) << QDateTime( QDate(2009, 11, 8), QTime(14, 35, 12, 243), Qt::UTC );
    QTest::newRow("2009-11-08T14:35:12Z") << QString( "2009-11-08T14:35:12Z" ) << QDateTime( QDate(2009, 11, 8), QTime(14, 35, 12, 0), Qt::UTC );
    QTest::newRow("2009-11-08T14:35:12") << QString( "2009-11-08T14:35:12" ) << QDateTime( QDate(2009, 11, 8), QTime(14, 35, 12, 0), Qt::UTC );
}



void DateTimeTest::testFromDateTimeString()
{
    QFETCH( QString, string );
    QFETCH( QDateTime, datetime );

    QDateTime parsedDateTime = Soprano::DateTime::fromDateTimeString( string );
    QCOMPARE( parsedDateTime, datetime );
}


void DateTimeTest::testTimeToString_data()
{
    QTest::addColumn<QTime>( "time" );
    QTest::addColumn<QString>( "string" );

    QTest::newRow("01:01:01") << QTime(1, 1, 1) << QString( "01:01:01Z" );
    QTest::newRow("14:35:12.243Z") << QTime(14, 35, 12, 243) << QString( "14:35:12.243Z" );
    QTest::newRow("14:35:12Z") << QTime(14, 35, 12, 0) << QString( "14:35:12Z" );
}


void DateTimeTest::testTimeToString()
{
    QFETCH( QTime, time );
    QFETCH( QString, string );

    QString serializedString = Soprano::DateTime::toString( time );
    QCOMPARE( serializedString, string );
}


void DateTimeTest::testDateToString_data()
{
    QTest::addColumn<QDate>( "date" );
    QTest::addColumn<QString>( "string" );

    QTest::newRow("2009-01-08") << QDate(2009, 1, 8) << QString( "2009-01-08" );
    QTest::newRow("2009-11-08") << QDate(2009, 11, 8) << QString( "2009-11-08" );
}


void DateTimeTest::testDateToString()
{
    QFETCH( QDate, date );
    QFETCH( QString, string );

    QString serializedString = Soprano::DateTime::toString( date );
    QCOMPARE( serializedString, string );
}


void DateTimeTest::testDateTimeToString_data()
{
    QTest::addColumn<QDateTime>( "datetime" );
    QTest::addColumn<QString>( "string" );

    QTest::newRow("2009-01-08T01:01:01Z") << QDateTime( QDate(2009, 1, 8), QTime(1, 1, 1), Qt::UTC ) << QString( "2009-01-08T01:01:01Z" );
    QTest::newRow("2009-11-08T14:35:12.243Z") << QDateTime( QDate(2009, 11, 8), QTime(14, 35, 12, 243), Qt::UTC ) << QString( "2009-11-08T14:35:12.243Z" );
    QTest::newRow("2009-11-08T14:35:12Z") << QDateTime( QDate(2009, 11, 8), QTime(14, 35, 12, 0), Qt::UTC ) << QString( "2009-11-08T14:35:12Z" );
}


void DateTimeTest::testDateTimeToString()
{
    QFETCH( QDateTime, datetime );
    QFETCH( QString, string );

    QString serializedString = Soprano::DateTime::toString( datetime );
    QCOMPARE( serializedString, string );
}

QTEST_MAIN( DateTimeTest )

#include "datetimetest.moc"
