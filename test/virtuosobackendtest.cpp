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

Soprano::Model* Soprano::VirtuosoBackendTest::createModel()
{
    const Soprano::Backend* b = Soprano::discoverBackendByName( "virtuosobackend" );
    if ( b ) {
        BackendSettings settings;
        settings << BackendSetting( BackendOptionHost, "localhost" );
        settings << BackendSetting( BackendOptionPort, 1111 );
        Model* m = b->createModel( settings );
        m->removeAllStatements();
        return m;
    }
    return 0;
}

QTEST_MAIN( Soprano::VirtuosoBackendTest )

#include "virtuosobackendtest.moc"
