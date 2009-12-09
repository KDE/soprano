/*
 * This file is part of Soprano Project
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

#include "virtuosobackendtest.h"

#include <soprano.h>

#include <QtTest/QtTest>

Soprano::VirtuosoBackendTest::VirtuosoBackendTest()
    : m_modelCnt( 0 )
{
}


Soprano::Model* Soprano::VirtuosoBackendTest::createModel()
{
    const Soprano::Backend* b = Soprano::discoverBackendByName( "virtuosobackend" );
    if ( b ) {
        BackendSettings settings;
        QString dirName( "soprano_virtuoso_backend_test_model_" );
        QDir tmpDir( QDir::tempPath() );
        while ( tmpDir.exists( dirName + QString::number( m_modelCnt ) ) )
            ++m_modelCnt;
        dirName += QString::number( m_modelCnt );
        tmpDir.mkdir( dirName );
        settings << BackendSetting( BackendOptionStorageDir, QDir::tempPath() + '/' + dirName );
//         settings << BackendSetting( BackendOptionHost, "localhost" );
//         settings << BackendSetting( BackendOptionPort, 1111 );
//         settings << BackendSetting( BackendOptionUsername, "dba" );
//         settings << BackendSetting( BackendOptionPassword, "dba" );
        Model* m = b->createModel( settings );
        m_settingsHash.insert( m, settings );
        return m;
    }
    return 0;
}


void Soprano::VirtuosoBackendTest::deleteModel( Soprano::Model* m )
{
	BackendSettings settings = m_settingsHash[m];
    m_settingsHash.remove( m );
	// we need to shutdown the server before removing the model data files and dir
    delete m;

    const Soprano::Backend* b = Soprano::discoverBackendByName( "virtuosobackend" );
    if ( b ) {
    	b->deleteModelData( settings );
        
        QDir tmpDir( QDir::tempPath() );
        tmpDir.rmdir( settingInSettings( settings, BackendOptionStorageDir ).value().toString().section( '/', -1 ) );
    }
}

QTEST_MAIN( Soprano::VirtuosoBackendTest )

#include "virtuosobackendtest.moc"
