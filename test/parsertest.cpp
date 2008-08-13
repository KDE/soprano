/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2008 Sebastian Trueg <trueg@kde.org>
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

            it = parser->parseFile( filename, QUrl("http://soprano.sf.net/testdata/"), serialization );
            all = it.allStatements();
            QCOMPARE( all.count(), testStatements.count() );

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


void ParserTest::testEncoding()
{
    // FIXME: provide test data for all supported encodings (no big problem so far since we only have the raptor parser)

    // parse both files, same content, different encodings and compare the results
    const QString utf8File = SOPRANO_TEST_DATA_DIR"/rdf_xml-testdata-utf8.rdf";
    const QString isoFile = SOPRANO_TEST_DATA_DIR"/rdf_xml-testdata-iso8859.rdf";

    const Soprano::Parser* parser = PluginManager::instance()->discoverParserForSerialization( SerializationRdfXml );
    if ( parser ) {
        QList<Statement> utf8Statements = parser->parseFile( utf8File, QUrl(), SerializationRdfXml ).allStatements();
        QList<Statement> isoStatements = parser->parseFile( isoFile, QUrl(), SerializationRdfXml ).allStatements();

        // brute force comparision, time is not an issue, no need to get inventive. ;)
        foreach( const Statement& s, utf8Statements ) {
            if ( !isoStatements.contains( s ) ) qDebug() << s;
            QVERIFY( isoStatements.contains( s ) );
        }
        foreach( const Statement& s, isoStatements ) {
            if ( !utf8Statements.contains( s ) ) qDebug() << s;
            QVERIFY( utf8Statements.contains( s ) );
        }
    }
}

QTEST_MAIN( ParserTest )

#include "parsertest.moc"
