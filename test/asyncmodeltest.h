/* 
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2008 Sebastian Trueg <strueg@mandriva.com>
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

#ifndef SOPRANO_ASYNC_MODEL_TEST_H
#define SOPRANO_ASYNC_MODEL_TEST_H

#include <QtCore/QObject>

#include "asyncmodel.h"
#include "statement.h"


class AsyncModelTest: public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void testAddStatement();
    void testCountStatements();
    void testListStatements();
    void testSelectQuery();
    void testConstructQuery();
    void testAskQuery();
    void testListAndAdd();
    void testMultiAdd();

private:
    Soprano::Util::AsyncModel* m_asyncModel;
    Soprano::Statement m_s1;
    Soprano::Statement m_s2;
};

#endif
