/* 
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2006-2007 Sebastian Trueg <strueg@mandriva.com>
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

#include <QtCore/QObject>

#ifndef SOPRANO_TEST_H
#define SOPRANO_TEST_H

#include <soprano/backend.h>

namespace Soprano {
  class Model;
  class Statement;
}

class SopranoModelTest: public QObject
{
    Q_OBJECT

public:
    SopranoModelTest();

private Q_SLOTS:
  void init();
  void cleanup();

  void testAddModel();   
  void testAddListOfStatement(); 
  void testAddStatementIterator();
  void testAddStatements();

  void testListStatements();
  void testListStatementsWithContext();

  void testRemoveStatement();
  void testRemoveAllStatement();

  void testGraphQuery();
  void testBooleanQuery();
  void testQuery();

  void testCloseStatementIteratorOnModelDelete();

  void testContexts();

protected:
   Soprano::Statement* m_st1;
   Soprano::Statement* m_st2;
   Soprano::Statement* m_st3;
   Soprano::Statement* m_st4;

   Soprano::Model* m_model;
};

#endif
