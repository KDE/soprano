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

#include "sopranodbusmultithreadtest.h"
#include "../server/dbus/dbusclient.h"
#include "../server/dbus/dbusmodel.h"
#include "../soprano/storagemodel.h"
#include "../soprano/util/mutexmodel.h"

#include <QtTest/QtTest>


using namespace Soprano;
using namespace Soprano::Client;


SopranoDBusMultiThreadingTest::SopranoDBusMultiThreadingTest()
{
    m_client = new DBusClient();
    m_modelCnt = 0;
}


SopranoDBusMultiThreadingTest::~SopranoDBusMultiThreadingTest()
{
    delete m_client;
}


Soprano::Model* SopranoDBusMultiThreadingTest::createModel()
{
    Soprano::Model* m = m_client->createModel( QString( "Testmodel%1" ).arg( ++m_modelCnt ) );
    m_modelMap.insert( m, QString( "Testmodel%1" ).arg( m_modelCnt ) );
    Soprano::Util::MutexModel* mutexModel = new Soprano::Util::MutexModel( Soprano::Util::MutexModel::ReadWriteMultiThreading, m );
    return mutexModel;
}


void SopranoDBusMultiThreadingTest::deleteModel( Soprano::Model* model )
{
    Soprano::Model* origModel = static_cast<Soprano::Util::MutexModel*>( model )->parentModel();
    m_client->removeModel( m_modelMap[origModel] );
    delete origModel;
    delete model;
}


QTEST_MAIN( SopranoDBusMultiThreadingTest )

#include "sopranodbusmultithreadtest.moc"
