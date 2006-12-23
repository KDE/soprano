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

#ifndef SOPRANO_STATEMENT_ITERATOR_H
#define SOPRANO_STATEMENT_ITERATOR_H

#include <QtCore>

namespace Soprano {

class Statement;
class StatementIteratorPrivate;

class StatementIterator
{
public:
  StatementIterator();
  StatementIterator( StatementIteratorPrivate *sti );

  StatementIterator( const StatementIterator &sti );

  virtual ~StatementIterator();

  StatementIterator& operator=( const StatementIterator& );

  /**
   *\return true if there is another Statement
   */
  bool hasNext() const;

  /**
   *\return the Next Statement
   */
  const Statement next() const;

  /**
   *\return true if the StatementIterator is a valid Iterator.
   */
  bool isValid() const;

private:
  QSharedDataPointer<StatementIteratorPrivate> d;
  bool m_valid;
};

}

#endif // SOPRANO_STATEMENT_ITERATOR_H

