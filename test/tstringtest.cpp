/*
 *
 * $Id: sourceheader 511311 2006-02-19 14:51:05Z trueg $
 *
 * This file is part of the Nepomuk KDE project.
 * Copyright (C) 2006-2007 Sebastian Trueg <trueg@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */

#include "tstringtest.h"
#include "../index/tstring.h"
#include "stringpool.h"

#include <QtTest/QTest>
#include <QtCore/QDebug>


static void printWChars( const wchar_t* data, int len = -1 )
{
    if ( len < 0 )
        len = wcslen( data );
    for ( int i = 0; i < len; ++i ) {
        printf( "%.2x%.2x%.2x%.2x ", ( char )( data[i]>>24 )&0xff, ( char )( data[i]>>16 )&0xff, ( char )( data[i]>>8 )&0xff, ( char )data[i]&0xff );
    }
    printf( "\n" );
}

static void printUtf16( const unsigned short* data, int len = -1 )
{
    for ( int i = 0; ( len > 0 && i < len ) || data[i]; ++i ) {
        printf( "%.2x%.2x ", ( char )( data[i]>>8 )&0xff, ( char )data[i]&0xff );
    }
    printf( "\n" );
}


Q_DECLARE_METATYPE( TString )

void TStringTest::testConversion_data()
{
    QTest::addColumn<QString>( "qstring" );
//    QTest::addColumn<TString>( "tstring" );

    QTest::newRow( "plain" ) << QString( QLatin1String( "Hello World" ) );
// #ifdef _UCS2
//                              << TString( L"Hello World" );
// #else
//                              << TString( "Hello World" );
// #endif

#ifdef _UCS2
        QTest::newRow( "germanUmlauts" ) << StringPool::germanUmlauts();// << TString( ( const wchar_t* )StringPool::germanUmlauts().utf16() );
    QTest::newRow( "frenchAccents" ) << StringPool::frenchAccents();// << TString( ( const wchar_t* )StringPool::frenchAccents().utf16() );
    QTest::newRow( "russianChars" ) << StringPool::russianChars();// << TString( ( const wchar_t* )StringPool::russianChars().utf16() );
#endif
}


void TStringTest::testConversion()
{
    QFETCH( QString, qstring );
//    QFETCH( TString, tstring );

    printf( "Testing string: %s\n", qstring.toUtf8().data() );
    printf( "  -> utf16 data: " );
    printUtf16( qstring.utf16(), qstring.length() );
    printf( "Converted string: %ls\n", TString( qstring ).data() );
    printf( "  -> ucs4 data: " );
    printWChars( TString( qstring ).data() );

    QString ttoq = TString( qstring ).toQString();

    printf( "Re-converted string: %s\n", TString( qstring ).toQString().toUtf8().data() );
    printf( "  -> utf16 data: " );
    printUtf16( TString( qstring ).toQString().utf16(), TString( qstring ).toQString().length() );

    TString tfromq( qstring );
    TString qtot = qstring;
    QString qfromqtoq = tfromq.toQString();
    TString tfromttoq( ttoq );
    TString ttoqtot = ttoq;

    QCOMPARE( TString( qstring ).toQString(), qstring );
    QCOMPARE( TString::fromUtf8( qstring.toUtf8().data() ).toQString(), qstring );

    QCOMPARE( ttoq, qstring );
    QCOMPARE( qfromqtoq, qstring );

    QCOMPARE( tfromq, qtot );
    QCOMPARE( qtot, tfromttoq );
    QCOMPARE( tfromttoq, tfromq );
}


void TStringTest::testLength()
{
    for ( int i = 0; i < 100; ++i ) {
        QString s = QString::number( i ).rightJustified( 10, 'x' );
        if ( s != TString( s ).toQString() ) {
            qDebug() << "strings differ: " << s << "(QString length" << s.length() << "TString length" << TString( s ).length() << ")";
        }
        QCOMPARE( s, TString( s ).toQString() );
    }
}

QTEST_MAIN( TStringTest )

#include "tstringtest.moc"
