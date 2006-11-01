/* This file is part of QRDF
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

#ifndef RDF_STATEMENT_ITERATOR_H
#define RDF_STATEMENT_ITERATOR_H

#include <QtGlobal>

namespace RDF {

class Statement;

class StatementIterator
{
public:
  virtual ~StatementIterator();

  /**
   *\return true if there is another Statement
   */
  virtual  bool hasNext() const = 0;

  /**
   *\return the Next Statement
   */
  virtual const Statement next() const = 0;
};

}

#endif // RDF_STATEMENT_ITERATOR_H

