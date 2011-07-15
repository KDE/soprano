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

#include "serializetest.h"
#include "config-testdata.h"

#include "serializer.h"
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


void SerializerTest::init()
{
#if 0
    // Init reference statements list
    const Parser * parser = PluginManager::instance()->discoverParserForSerialization(SerializationTrig);
    QVERIFY(parser);
    StatementIterator refIt = parser->parseFile(
            SOPRANO_TEST_DATA_DIR"/trig-testdata.trig",
            QUrl("http://soprano.sf.net/testdata/"), 
            SerializationTrig);
    this->referenceStatements = refIt.allStatements();
#endif 
}

Q_DECLARE_METATYPE( Soprano::RdfSerialization )

void SerializerTest::testSerializer_data()
{

    QTest::addColumn<Soprano::RdfSerialization>( "serialization" );
    QTest::addColumn<bool>( "withContext" );

    QTest::newRow("rdf_xml") << SerializationRdfXml <<  false;
    QTest::newRow("turtle")  << SerializationTurtle <<  false;
    QTest::newRow("trig")    << SerializationTrig   <<  true;
}



void SerializerTest::testSerializer()
{
    QFETCH( RdfSerialization, serialization );
    QFETCH( bool, withContext );

    //qDebug() << "Serialization: " << serializationMimeType(serialization);
    QList<const Serializer*> serializers = PluginManager::instance()->allSerializers();


    /* Step 1 - create reference statements */
    QList<Statement> referenceStatements = testData(withContext);
    //QList<Statement> testStatements = testData( withContext );

    Q_FOREACH( const Serializer * serializer, serializers ) {
        if ( serializer->supportsSerialization( serialization ) ) {
            // Find if we have Parser for this serialization, to parse data back.
            // If not, then oops )
            const Parser * back_parser = PluginManager::instance()->discoverParserForSerialization(serialization);
            if ( !back_parser ) {
                qDebug() << "We can not found parser for" << 
                    serializationMimeType(serialization)  <<
                    " Skipping it";
                continue;
            }

            // Now serialize reference data
            QByteArray data_array;
            QTextStream serializationStorage(&data_array,QIODevice::ReadWrite);

            bool result = serializer->serialize(
                    /*what*/Util::SimpleStatementIterator(referenceStatements),
                    /*where*/serializationStorage,
                    /*type*/serialization
                    );
            QVERIFY(result);

            //qDebug() << "Serialization storage: " << data_array;

            // Now parse gained text stream back
            QTextStream readStorageStream(&data_array, QIODevice::ReadOnly);
            StatementIterator it = back_parser->parseStream( 
                    readStorageStream, 
                    QUrl("http://soprano.sf.net/testdata/"), 
                    serialization);

            QList<Statement> all = it.allStatements();


            // Now compare recieved with reference
            QCOMPARE( all.count(), referenceStatements.count() );

            Q_FOREACH( Statement s, referenceStatements ) {
                bool contains = all.contains(s);
                if ( !contains ) {
                    qDebug() << "Not found in parsed: " << s;
                }

                QVERIFY( contains );
            }

        }
    }
}


#if 0
void SerializerTest::testEncoding()
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
#endif 

QTEST_MAIN( SerializerTest )

#include "serializetest.moc"

