/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2008 Sebastian Trueg <trueg@kde.org>
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

#include "localsocketmultithreadingtest.h"

#include <soprano.h>
#include "../client/localsocketclient.h"

#include <QtTest/QtTest>

using namespace Soprano;


LocalSocketMultiThreadingTest::LocalSocketMultiThreadingTest()
    : m_modelCounter( 0 )
{
    m_client = new Soprano::Client::LocalSocketClient( this );
    m_client->connect();
}


Soprano::Model* LocalSocketMultiThreadingTest::createModel()
{
    return m_client->createModel( QString( "model%1" ).arg( ++m_modelCounter ) );
}

QTEST_MAIN( LocalSocketMultiThreadingTest )

