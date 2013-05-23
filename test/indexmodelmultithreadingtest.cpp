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

#include "indexmodelmultithreadingtest.h"

#include <soprano.h>
#include "../index/indexfiltermodel.h"

#include <QtTest/QTest>
#include <QtCore/QDebug>
#include <QtCore/QProcess>
#include <QtCore/QDir>

using namespace Soprano;


Soprano::Model* IndexMultiThreadTest::createModel()
{
    Model* m = Soprano::createModel();
    if ( m ) {
        // remove previous testdata
        QProcess::execute( "rm", QString( "-rf,/tmp/sopranoindextest" ).split( "," ) );
        QDir( "/tmp" ).mkdir( "sopranoindextest" );

        return new Index::IndexFilterModel( "/tmp/sopranoindextest", m );
    }
    return 0;
}


void IndexMultiThreadTest::deleteModel( Soprano::Model* model )
{
    if ( model ) {
        Index::IndexFilterModel* fm = ( Index::IndexFilterModel* ) model;
        Model* m = fm->parentModel();
        delete fm;
        delete m;
    }
}


QTEST_MAIN( IndexMultiThreadTest )

