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

#include "nrlmodeltest.h"

#include <QtTest/QTest>
#include <QtCore/QDebug>

#include "../soprano/soprano.h"
#include "../soprano/nrlmodel.h"

using namespace Soprano;


void NRLModelTest::init()
{
    // create any memory model
    QList<BackendSetting> settings;
    settings.append( BackendSetting( BackendOptionStorageMemory ) );

    // redland does not support SPARQL UNION
    const Backend* b = PluginManager::instance()->discoverBackendByName( "sesame2" );
    if ( b )
        m_model = b->createModel( settings );
    else
        m_model = createModel( settings );

    QVERIFY( m_model != 0 );

    m_nrlModel = new NRLModel( m_model );
}


void NRLModelTest::cleanup()
{
    delete m_nrlModel;
    delete m_model;
}


void NRLModelTest::testAddStatement()
{
    Statement s1( QUrl( "http://soprano.org/test#A" ),
                  QUrl( "http://soprano.org/test#prop1" ),
                  QUrl( "http://soprano.org/test#B" ) );
    Statement s2( QUrl( "http://soprano.org/test#A" ),
                  QUrl( "http://soprano.org/test#prop1" ),
                  QUrl( "http://soprano.org/test#C" ) );
    Statement s3( QUrl( "http://soprano.org/test#A" ),
                  QUrl( "http://soprano.org/test#prop1" ),
                  QUrl( "http://soprano.org/test#D" ) );
    m_nrlModel->addStatement( s1 );

    // create a restriction
    m_model->addStatement( Statement( s1.predicate(), Vocabulary::NRL::minCardinality(), LiteralValue( 1 ) ) );
    m_model->addStatement( Statement( s1.predicate(), Vocabulary::NRL::maxCardinality(), LiteralValue( 2 ) ) );

    m_nrlModel->addStatement( s1 );
    m_nrlModel->addStatement( s2 );
    m_nrlModel->addStatement( s3 );
}

QTEST_MAIN( NRLModelTest )

#include "nrlmodeltest.moc"
