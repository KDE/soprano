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

#include "storagescalingtest.h"

#include <stdio.h>

#include <QtTest/QTest>
#include <QtCore/QDir>
#include <QtCore/QProcess>
#include <QtCore/QCoreApplication>
#include <QtCore/QFileInfo>
#include <QtCore/QDebug>

#include "../soprano/soprano.h"

using namespace Soprano;

void StorageScalingTest::initTestCase()
{
    QList<BackendSetting> settings;
    settings.append( BackendSetting( BackendOptionStorageDir, "/tmp/scalingtest" ) );

    QDir( "/tmp" ).mkdir( "scalingtest" );

    m_model = Soprano::createModel( settings );

    QVERIFY( m_model );

    m_counter = 0;
}


void StorageScalingTest::cleanupTestCase()
{
    delete m_model;

    quint64 size = 0;
    QStringList files = QDir( "/tmp/scalingtest" ).entryList( QDir::Files );
    foreach( QString s, files )
        size += QFileInfo( "/tmp/scalingtest/" + s ).size();

    qDebug() << "Added " << m_counter << " statements to test storage with size: " << size;

    // delete it all
    QProcess::execute( "rm", QString( "-rf,/tmp/scalingtest" ).split( "," ) );
}


void StorageScalingTest::testScaling()
{
    // hardcore!
    const int max = 1000000;
    for ( m_counter = 0; m_counter < max; ++m_counter ) {
        QVERIFY( m_model->addStatement( Statement( QUrl( QString( "http://soprano.org/hardcore/subject%1" ).arg( m_counter ) ),
                                          QUrl( QString( "http://soprano.org/hardcore/predicate%1" ).arg( m_counter ) ),
                                          LiteralValue( QString( "object%1" ).arg( m_counter ) ) ) ) == 0 );
        fprintf( stderr, "Cnt: %d\r", m_counter );
    }
}

QTEST_MAIN( StorageScalingTest )

