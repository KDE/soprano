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

#include "cluceneindextest.h"

#include <QtTest/QTest>
#include <QtCore/QDebug>
#include <QtCore/QProcess>
#include <QtCore/QDir>

#include "../soprano/soprano.h"
#include "../soprano/index/indexfiltermodel.h"
#include "../soprano/index/cluceneindex.h"
#include "../soprano/index/wstring.h"

#include <CLucene.h>

using namespace Soprano;
using namespace Soprano::Index;


void IndexTest::initTestCase()
{
    // create any memory model
    QList<BackendSetting> settings;
    settings.append( BackendSetting( BACKEND_OPTION_STORAGE_MEMORY ) );
    m_model = createModel( settings );

    QVERIFY( m_model != 0 );

    // remove previous testdata
    QProcess::execute( "rm", QString( "-rf,/tmp/sopranoindextest" ).split( "," ) );
    QDir( "/tmp" ).mkdir( "sopranoindextest" );

    m_indexModel = new IndexFilterModel( "/tmp/sopranoindextest", m_model );
}


void IndexTest::cleanupTestCase()
{
    delete m_indexModel;
    delete m_model;
}


void IndexTest::testAddStatement()
{
    Statement s1( QUrl( "http://soprano.sf.net/test#A" ),
                  QUrl( "http://soprano.sf.net/test#valueX" ),
                  LiteralValue( "Hello World" ) );
    Statement s2( QUrl( "http://soprano.sf.net/test#A" ),
                  QUrl( "http://soprano.sf.net/test#valueY" ),
                  LiteralValue( "Wurst" ) );

    QVERIFY( m_indexModel->addStatement( s1 ) == Error::ERROR_NONE );
    QVERIFY( m_indexModel->containsStatements( s1 ) );

    // now check if the statement was properly indexed
    lucene::search::Hits* hits = m_indexModel->index()->search( "Hello World" );
    QVERIFY( hits != 0 );
    QCOMPARE( hits->length(), 1 );
    QCOMPARE( WString( hits->doc( 0 ).get( L"id" ) ), WString( s1.subject().toString() ) );
    _CLDELETE( hits );

    QVERIFY( m_indexModel->addStatement( s2 ) == Error::ERROR_NONE );
    QVERIFY( m_indexModel->containsStatements( s2 ) );

    QTextStream s( stderr );
    m_indexModel->index()->dump( s );

    // check the previous one again to make sure the index is not borked
    hits = m_indexModel->index()->search( "Hello World" );
    QVERIFY( hits != 0 );
    QCOMPARE( hits->length(), 1 );
    QCOMPARE( WString( hits->doc( 0 ).get( L"id" ) ), WString( s1.subject().toString() ) );
    _CLDELETE( hits );

    // and check for the second statement
    hits = m_indexModel->index()->search( "Wurst" );
    QVERIFY( hits != 0 );
    QCOMPARE( hits->length(), 1 );
    QCOMPARE( WString( hits->doc( 0 ).get( L"id" ) ), WString( s2.subject().toString() ) );
    _CLDELETE( hits );
}

QTEST_MAIN( IndexTest );

#include "cluceneindextest.moc"
