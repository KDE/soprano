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

#include "simpletransactionmodeltest.h"
#include "soprano.h"

#include <QtTest>


using namespace Soprano;

Soprano::Model* SimpleTransactionModelTest::createModel()
{
    Model* m = Soprano::createModel();
    Q_ASSERT( m );
    Util::SimpleTransactionModel* tm = new Util::SimpleTransactionModel( m );
    m->setParent( tm ); // mem management
    return tm;
}

QTEST_MAIN( SimpleTransactionModelTest )

#include "simpletransactionmodeltest.moc"
