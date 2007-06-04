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

#include <QtCore/QObject>
#include <QtCore/QList>

#ifndef STATEMENT_ITERATOR_TEST_H
#define STATEMENT_ITERATOR_TEST_H

namespace Soprano {
  class Model;
  class ModelFactory;
  class Statement;
}

class StatementIteratorTest: public QObject
{
Q_OBJECT

private Q_SLOTS:
  void testIterator();
  void testSharedStuffs();

  void init();
  void cleanup();

protected:
   Soprano::Model* m_model;
   Soprano::ModelFactory *m_factory;

   QList<Soprano::Statement> m_statements;
};

#endif // STATEMENT_ITERATOR_TEST_H
