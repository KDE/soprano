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

#include "3storemodeltest.h"

#include <soprano/soprano.h>

using namespace Soprano;

void TStoreModelTest::initTestCase()
{
    const Soprano::Backend* b = Soprano::discoverBackendByName( "3store" );
    QVERIFY( b != 0 );
    Soprano::setUsedBackend( b );
    m_model = Soprano::createModel( "3storetestdb2", QString( "user=rdf,password=rdf" ).split( "," ) );
    QVERIFY( m_model != 0 );
}

void TStoreModelTest::cleanupTestCase()
{
    delete m_model;
}

QTEST_MAIN(TStoreModelTest)

#include "3storemodeltest.moc"

