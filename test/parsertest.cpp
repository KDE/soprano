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

#include "parser.h"
#include "pluginmanager.h"
#include "statementiterator.h"
#include "simplestatementiterator.h"
#include "statement.h"
#include "vocabulary.h"

#include <QtTest/QTest>
#include <QtCore/QFile>
#include <QtCore/QDebug>


using namespace Soprano;

#include "testdata.h"


void ParserTest::init()
{
}

Q_DECLARE_METATYPE( Soprano::RdfSerialization )

void ParserTest::testParser_data()
{
    QTest::addColumn<Soprano::RdfSerialization>( "serialization" );
    QTest::addColumn<QString>( "filename" );
    QTest::addColumn<bool>( "withContext" );

    QTest::newRow("rdf_xml") << SerializationRdfXml << SOPRANO_TEST_DATA_DIR"/rdf_xml-testdata.rdf" << false;
    QTest::newRow("turtle") << SerializationTurtle << SOPRANO_TEST_DATA_DIR"/turtle-testdata.ttl" << false;
    QTest::newRow("trig") << SerializationTrig << SOPRANO_TEST_DATA_DIR"/trig-testdata.trig" << true;
}



void ParserTest::testParser()
{
    QFETCH( RdfSerialization, serialization );
    QFETCH( QString, filename );
    QFETCH( bool, withContext );

    QList<const Parser*> parsers = PluginManager::instance()->allParsers();

    QList<Statement> testStatements = testData( withContext );

    Q_FOREACH( const Parser* parser, parsers ) {
        if ( parser->supportsSerialization( serialization ) ) {
            // test parsing
            StatementIterator it = parser->parseFile( filename, QUrl("http://soprano.sf.net/testdata/"), serialization );
            QList<Statement> all = it.allStatements();

            QCOMPARE( all.count(), testStatements.count() );

            Q_FOREACH( Statement s, testStatements ) {
                if ( !all.contains( s ) ) {
                    qDebug() << "Not found in parsed: " << s;
                }

                QVERIFY( all.contains( s ) );
            }

            // test serialization
//             it = SimpleStatementIterator( testStatements );
//             QString serializedData;
//             QTextStream s( &serializedData );
//             QVERIFY( parser->serialize( it, &s, serialization ) );

//             // the only way to check the result I can think of is to parse again
//             all = parser->parseString( serializedData, QUrl("http://soprano.sf.net/testdata/"), serialization ).allStatements();

//             QCOMPARE( testStatements.count(), all.count() );

//             Q_FOREACH( Statement s, testStatements ) {
//                 QVERIFY( all.contains( s ) );
//            }
        }
    }
}

QTEST_MAIN( ParserTest )

#include "parsertest.moc"
