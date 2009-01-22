/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
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

#include "RedlandPersistentModelTest.h"

#include "soprano.h"

#include <QtCore/QFile>

using namespace Soprano;

static int modelCnt = 0;

Soprano::Model* RedlandPersistentModelTest::createModel()
{
    const Backend* b = Soprano::discoverBackendByName( "redland" );
    if ( !b ) {
        return 0;
    }


    QList<BackendSetting> settings;
    settings.append( BackendSetting( "hash-type", "bdb" ) );
    settings.append( BackendSetting( "name", QString( "test%1" ).arg( ++modelCnt ) ) );
    settings.append( BackendSetting( BackendOptionStorageDir, "/tmp/" ) );

    Model* m = b->createModel( settings );
    if ( m ) {
        m_nameMap[m] = QString( "test%1" ).arg( modelCnt );
    }
    return m;
}


void RedlandPersistentModelTest::deleteModel( Soprano::Model* m )
{
    QString name = m_nameMap[m];
    delete m;
    QFile::remove( QString( "/tmp/%1-po2s.db" ).arg( name ) );
    QFile::remove( QString( "/tmp/%1-so2p.db" ).arg( name ) );
    QFile::remove( QString( "/tmp/%1-sp2o.db" ).arg( name ) );
    QFile::remove( QString( "/tmp/%1-contexts.db" ).arg( name ) );
}

QTEST_MAIN(RedlandPersistentModelTest)

#include "RedlandPersistentModelTest.moc"

