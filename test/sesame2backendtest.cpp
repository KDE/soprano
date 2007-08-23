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

#include "sesame2backendtest.h"

#include <soprano.h>

#include <QtTest>

using namespace Soprano;


Soprano::Model* Sesame2BackendTest::createModel()
{
    const Soprano::Backend* b = Soprano::discoverBackendByName( "sesame2" );
    if ( b ) {
        return b->createModel();
    }
    return 0;
}


// this test was used to determine if Sesame2::Model::closeIterators() is necessary. It is.
void Sesame2BackendTest::testIteratorClose()
{
    StatementIterator it = m_model->listStatements( *m_st1 );

    // now the iterator is open

    // try read-only stuff
    QVERIFY( m_model->listStatements( *m_st2 ).next() );

    // try other stuff (actually the fear is that this will hang)
    QVERIFY( m_model->removeStatements( *m_st2 ) == ERROR_NONE );
}

QTEST_MAIN( Sesame2BackendTest );

#include "sesame2backendtest.moc"
