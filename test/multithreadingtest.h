/* 
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007 Sebastian Trueg <strueg@mandriva.com>
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

#ifndef SOPRANO_MULTITHREADING_TEST_H
#define SOPRANO_MULTITHREADING_TEST_H

#include <QtCore/QObject>
#include <QtCore/QList>

#include "../soprano/statement.h"

namespace Soprano {
    class Model;
}

using namespace Soprano;

class MultiThreadingTest : public QObject
{
Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void testNodeIterator_data();
    void testNodeIterator();
    void testAddStatement();
    void cleanupTestCase();

protected:
    virtual Soprano::Model* createModel() = 0;
    virtual void deleteModel( Soprano::Model* );
  
private:
    void startAllTests( Soprano::Model* );
    void verifyAllTests();

    QList<QThread*> m_testThreads;
    QList<Statement> m_testStatements;
    QUrl m_testContext1;
    QUrl m_testContext2;
};

#endif
