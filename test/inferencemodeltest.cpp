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

#include "inferencemodeltest.h"
#include "soprano/soprano.h"
#include "soprano/vocabulary/rdfs.h"
#include "soprano/inference/inferencemodel.h"
#include "soprano/inference/statementpattern.h"
#include "soprano/inference/nodepattern.h"
#include "soprano/inference/inferencerule.h"
#include "soprano/inference/inferenceruleset.h"
#include "soprano/inference/inferenceruleparser.h"

#include <QtTest/QTest>
#include <QtCore/QDebug>
#include <QtCore/QTime>


void InferenceModelTest::initTestCase()
{
//    Soprano::setUsedBackend( Soprano::discoverBackendByName( "sesame2" ) );
    m_model = Soprano::createModel();
    QVERIFY( m_model );
    m_infModel = new InferenceModel( m_model );
    m_infModel->setOptimizedQueriesEnabled( false );

    // create a simple rule
    Rule rule;
    rule.addPrecondition( StatementPattern( NodePattern( "a" ), NodePattern( Vocabulary::RDFS::SUBCLASSOF() ), NodePattern( "b" ) ) );
    rule.addPrecondition( StatementPattern( NodePattern( "b" ), NodePattern( Vocabulary::RDFS::SUBCLASSOF() ), NodePattern( "c" ) ) );
    rule.setEffect( StatementPattern( NodePattern( "a" ), NodePattern( Vocabulary::RDFS::SUBCLASSOF() ), NodePattern( "c" ) ) );

    m_infModel->addRule( rule );
}


void InferenceModelTest::cleanupTestCase()
{
    delete m_infModel;
    delete m_model;
}


void InferenceModelTest::init()
{
    m_model->removeAllStatements();
}


void InferenceModelTest::testAddStatementSingle()
{
    Statement s1( QUrl( "http://soprano.sf.net/test#A" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#B" ) );
    Statement s2( QUrl( "http://soprano.sf.net/test#B" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#C" ) );

    Statement s3( QUrl( "http://soprano.sf.net/test#A" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#C" ) );

    m_infModel->addStatement( s1 );

    // at this point nothing should be infered
    QCOMPARE( 1, m_model->statementCount() );
    QVERIFY( m_model->containsAnyStatement( s1 ) );

    m_infModel->addStatement( s2 );

    // now some inference should have been done

    // first check the actual triple we want to be infered
    QVERIFY( m_model->containsAnyStatement( s3 ) );
}


void InferenceModelTest::testAddStatementMulti()
{
    // F -> E -> D -> C -> B -> A
    Statement fe( QUrl( "http://soprano.sf.net/test#F" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#E" ) );
    Statement ed( QUrl( "http://soprano.sf.net/test#E" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#D" ) );
    Statement dc( QUrl( "http://soprano.sf.net/test#D" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#C" ) );
    Statement cb( QUrl( "http://soprano.sf.net/test#C" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#B" ) );
    Statement ba( QUrl( "http://soprano.sf.net/test#B" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#A" ) );

    // X -> B
    // X -> A
    Statement xb( QUrl( "http://soprano.sf.net/test#X" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#C" ) );
    Statement xa( QUrl( "http://soprano.sf.net/test#X" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#A" ) );

    // we throw them all in adn check the inference afterwards
    m_infModel->addStatement( fe );
    m_infModel->addStatement( ed );
    m_infModel->addStatement( dc );
    m_infModel->addStatement( cb );
    m_infModel->addStatement( ba );
    m_infModel->addStatement( xa );
    m_infModel->addStatement( xb );

//    m_infModel->print();

    // now check that we have all the inferred statements
    QVERIFY( m_model->containsAnyStatement( Statement( QUrl( "http://soprano.sf.net/test#F" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#A" ) ) ) );
    QVERIFY( m_model->containsAnyStatement( Statement( QUrl( "http://soprano.sf.net/test#F" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#B" ) ) ) );
    QVERIFY( m_model->containsAnyStatement( Statement( QUrl( "http://soprano.sf.net/test#F" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#C" ) ) ) );
    QVERIFY( m_model->containsAnyStatement( Statement( QUrl( "http://soprano.sf.net/test#F" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#D" ) ) ) );

    QVERIFY( m_model->containsAnyStatement( Statement( QUrl( "http://soprano.sf.net/test#E" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#A" ) ) ) );
    QVERIFY( m_model->containsAnyStatement( Statement( QUrl( "http://soprano.sf.net/test#E" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#B" ) ) ) );
    QVERIFY( m_model->containsAnyStatement( Statement( QUrl( "http://soprano.sf.net/test#E" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#C" ) ) ) );

    QVERIFY( m_model->containsAnyStatement( Statement( QUrl( "http://soprano.sf.net/test#D" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#A" ) ) ) );
    QVERIFY( m_model->containsAnyStatement( Statement( QUrl( "http://soprano.sf.net/test#D" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#B" ) ) ) );

    QVERIFY( m_model->containsAnyStatement( Statement( QUrl( "http://soprano.sf.net/test#C" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#A" ) ) ) );

    QVERIFY( m_model->containsAnyStatement( Statement( QUrl( "http://soprano.sf.net/test#X" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#B" ) ) ) );
}


void InferenceModelTest::testRemoveStatementsSingle()
{
    Statement s1( QUrl( "http://soprano.sf.net/test#A" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#B" ) );
    Statement s2( QUrl( "http://soprano.sf.net/test#B" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#C" ) );
    m_infModel->addStatement( s1 );
    m_infModel->addStatement( s2 );

    m_infModel->removeAllStatements( s1 );

    // now we should only have a single statement left
    StatementIterator it = m_model->listStatements();
    QVERIFY( it.next() );
    QCOMPARE( s2, *it );
    QVERIFY( !it.next() );
}


void InferenceModelTest::testRemoveStatementsMulti()
{
    // F -> E -> D -> C -> B -> A
    Statement fe( QUrl( "http://soprano.sf.net/test#F" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#E" ) );
    Statement ed( QUrl( "http://soprano.sf.net/test#E" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#D" ) );
    Statement dc( QUrl( "http://soprano.sf.net/test#D" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#C" ) );
    Statement cb( QUrl( "http://soprano.sf.net/test#C" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#B" ) );
    Statement ba( QUrl( "http://soprano.sf.net/test#B" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#A" ) );

    // we throw them all in adn check the inference afterwards
    m_infModel->addStatement( fe );
    m_infModel->addStatement( ed );
    m_infModel->addStatement( dc );
    m_infModel->addStatement( cb );
    m_infModel->addStatement( ba );

//    m_infModel->print();

    // now let's remove one in the middle
    m_infModel->removeAllStatements( cb );

//    m_infModel->print();

    // now nothing should be based on A anymore, except for B
    QVERIFY( !m_model->containsAnyStatement( Statement( QUrl( "http://soprano.sf.net/test#F" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#A" ) ) ) );
    QVERIFY( !m_model->containsAnyStatement( Statement( QUrl( "http://soprano.sf.net/test#F" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#B" ) ) ) );
    QVERIFY( m_model->containsAnyStatement( Statement( QUrl( "http://soprano.sf.net/test#F" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#C" ) ) ) );
    QVERIFY( m_model->containsAnyStatement( Statement( QUrl( "http://soprano.sf.net/test#F" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#D" ) ) ) );

    QVERIFY( !m_model->containsAnyStatement( Statement( QUrl( "http://soprano.sf.net/test#E" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#A" ) ) ) );
    QVERIFY( !m_model->containsAnyStatement( Statement( QUrl( "http://soprano.sf.net/test#E" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#B" ) ) ) );
    QVERIFY( m_model->containsAnyStatement( Statement( QUrl( "http://soprano.sf.net/test#E" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#C" ) ) ) );

    QVERIFY( !m_model->containsAnyStatement( Statement( QUrl( "http://soprano.sf.net/test#D" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#A" ) ) ) );
    QVERIFY( !m_model->containsAnyStatement( Statement( QUrl( "http://soprano.sf.net/test#D" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#B" ) ) ) );
}


void InferenceModelTest::testPerformInference()
{
    Statement s1( QUrl( "http://soprano.sf.net/test#A" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#B" ) );
    Statement s2( QUrl( "http://soprano.sf.net/test#B" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#C" ) );
    m_model->addStatement( s1 );
    m_model->addStatement( s2 );

    m_infModel->performInference();

    Statement s3( QUrl( "http://soprano.sf.net/test#A" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#C" ) );
    QVERIFY( m_model->containsAnyStatement( s3 ) );
}


void InferenceModelTest::testPerformance()
{
    // F -> E -> D -> C -> B -> A
    Statement fe( QUrl( "http://soprano.sf.net/test#F" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#E" ) );
    Statement ed( QUrl( "http://soprano.sf.net/test#E" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#D" ) );
    Statement dc( QUrl( "http://soprano.sf.net/test#D" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#C" ) );
    Statement cb( QUrl( "http://soprano.sf.net/test#C" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#B" ) );
    Statement ba( QUrl( "http://soprano.sf.net/test#B" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#A" ) );

    // X -> B
    // X -> A
    Statement xb( QUrl( "http://soprano.sf.net/test#X" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#C" ) );
    Statement xa( QUrl( "http://soprano.sf.net/test#X" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#A" ) );

    // we just do some performance comparision
    QTime timer;

    // performance with uncompressed source statements
    // -----------------------------------------------
    timer.start();

    m_infModel->setCompressedSourceStatements( false );

    m_infModel->addStatement( fe );
    m_infModel->addStatement( ed );
    m_infModel->addStatement( dc );
    m_infModel->addStatement( cb );
    m_infModel->addStatement( ba );
    m_infModel->addStatement( xa );
    m_infModel->addStatement( xb );

    qDebug() << "Time for adding with inferencing: " << timer.elapsed();

    timer.start();
    m_infModel->removeAllStatements( cb );

    qDebug() << "Time for removing one statement with inferencing: " << timer.elapsed();

    m_model->removeAllStatements();
    // -----------------------------------------------

    // performance with compressed statements
    // -----------------------------------------------
    timer.start();
    m_infModel->setCompressedSourceStatements( true );
    m_infModel->addStatement( fe );
    m_infModel->addStatement( ed );
    m_infModel->addStatement( dc );
    m_infModel->addStatement( cb );
    m_infModel->addStatement( ba );
    m_infModel->addStatement( xa );
    m_infModel->addStatement( xb );

    qDebug() << "Time for adding with inferencing and compressed source statements: " << timer.elapsed();

    timer.start();
    m_infModel->removeAllStatements( cb );

    qDebug() << "Time for removing one statement with inferencing and compressed source statements: " << timer.elapsed();

    m_model->removeAllStatements();
    // -----------------------------------------------


    timer.start();
    m_model->addStatement( fe );
    m_model->addStatement( ed );
    m_model->addStatement( dc );
    m_model->addStatement( cb );
    m_model->addStatement( ba );
    m_model->addStatement( xa );
    m_model->addStatement( xb );

    qDebug() << "Time for adding without inferencing: " << timer.elapsed();

    timer.start();
    m_infModel->setCompressedSourceStatements( true );
    m_infModel->performInference();

    qDebug() << "Time for performing inference after adding the statements with compressed source statements: " << timer.elapsed();
}


void InferenceModelTest::testRuleParser_data()
{
    QTest::addColumn<int>( "ruleset" );
    QTest::addColumn<int>( "ruleCount" );

    QTest::newRow( "rdfs" ) << ( int )RDFS << 10;
    QTest::newRow( "nrl" ) << ( int )NRL << 3;
}


void InferenceModelTest::testRuleParser()
{
    QFETCH( int, ruleset );
    QFETCH( int, ruleCount );

    RuleSet rules = RuleSet::standardRuleSet( ( StandardRuleSet )ruleset );

    QCOMPARE( rules.count(), ruleCount );

    QStringList ruleNames = rules.ruleNames();
    Q_FOREACH( QString name, ruleNames ) {
        qDebug() << name << ": " << rules[name];
    }
}


void InferenceModelTest::testClearInference()
{
    Statement s1( QUrl( "http://soprano.sf.net/test#A" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#B" ) );
    Statement s2( QUrl( "http://soprano.sf.net/test#B" ), Vocabulary::RDFS::SUBCLASSOF(), QUrl( "http://soprano.sf.net/test#C" ) );
    m_infModel->addStatement( s1 );
    m_infModel->addStatement( s2 );

    m_infModel->clearInference();

    StatementIterator it = m_model->listStatements();
    QVERIFY( it.next() );
    QVERIFY( it.current() == s1 || it.current() == s2 );
    QVERIFY( it.next() );
    QVERIFY( it.current() == s1 || it.current() == s2 );
    QVERIFY( !it.next() );
}

QTEST_MAIN( InferenceModelTest );

#include "inferencemodeltest.moc"
