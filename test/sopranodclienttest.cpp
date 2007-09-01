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

#include "sopranodclienttest.h"
#include "../server/client.h"
#include "../soprano/storagemodel.h"

#include <QtTest>


using namespace Soprano;
using namespace Soprano::Server;


void SopranodClientTest::initTestCase()
{
    m_client = new Client();
    QVERIFY( m_client->connect() );
    m_modelCnt = 0;

    // start the server
//    m_serverProcess.start( "../server/sopranod" );
//    QVERIFY( !m_serverProcess.waitForStarted(-1) );
}


void SopranodClientTest::cleanupTestCase()
{
    // shutdown the server
//    m_serverProcess.terminate();

    delete m_client;
}


Soprano::Model* SopranodClientTest::createModel()
{
    return m_client->createModel( QString( "Testmodel%1" ).arg( m_modelCnt++ ) );
}

QTEST_MAIN( SopranodClientTest );

#include "sopranodclienttest.moc"
