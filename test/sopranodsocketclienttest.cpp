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

#include "sopranodsocketclienttest.h"
#include "../server/localsocketclient.h"
#include "../soprano/storagemodel.h"

#include <QtTest/QtTest>
#include <QtCore/QTime>
#include <QtCore/QList>


using namespace Soprano;
using namespace Soprano::Client;


void SopranodSocketClientTest::initTestCase()
{
    m_client = new LocalSocketClient();
    QVERIFY( m_client->connect() );
    m_modelCnt = 0;
}


void SopranodSocketClientTest::cleanupTestCase()
{
    delete m_client;
}


Soprano::Model* SopranodSocketClientTest::createModel()
{
    Soprano::Model* m = m_client->createModel( QString( "Testmodel%1" ).arg( ++m_modelCnt ) );
    m_modelMap.insert( m, QString( "Testmodel%1" ).arg( m_modelCnt ) );
    return m;
}


void SopranodSocketClientTest::deleteModel( Soprano::Model* m )
{
    m_client->removeModel( m_modelMap[m] );
    m_modelMap.remove( m );
    delete m;
}

QTEST_MAIN( SopranodSocketClientTest )

#include "sopranodsocketclienttest.moc"
