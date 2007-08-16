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

#include "parsertest.h"
#include "config-testdata.h"

#include <soprano/parser.h>
#include <soprano/pluginmanager.h>
#include <soprano/statementiterator.h>
#include <soprano/statement.h>

#include <QtTest/QTest>
#include <QtCore/QDebug>


using namespace Soprano;

#include "testdata.h"


void ParserTest::init()
{
}


void ParserTest::testParser_data()
{
    QTest::addColumn<QString>( "parserName" );

    QList<const Parser*> pl = PluginManager::instance()->allParsers();
    Q_FOREACH( const Parser* p, pl ) {
        QTest::newRow( p->pluginName().toLatin1().data() ) << p->pluginName();
    }
}


void ParserTest::testParser()
{
    QFETCH( QString, parserName );
    const Parser* parser = PluginManager::instance()->discoverParserByName( parserName );

    QList<Statement> testStatements = testData();

    if ( parser->supportsSerialization( RDF_XML ) ) {
        StatementIterator it = parser->parseFile( SOPRANO_TEST_DATA_DIR"/rdf_xml-testdata.rdf", QUrl("http://soprano.sf.net/testdata/"), RDF_XML );
        QList<Statement> all = it.allStatements();

        QCOMPARE( testStatements.count(), all.count() );

        Q_FOREACH( Statement s, testStatements ) {
            QVERIFY( all.contains( s ) );
        }
    }
}

QTEST_MAIN( ParserTest );

#include "parsertest.moc"
