/* 
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
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

#ifndef SOPRANO_STATEMENT_ITERATOR_PRIVATE_H
#define SOPRANO_STATEMENT_ITERATOR_PRIVATE_H

#include <QtCore/QSharedData>

#include <soprano/soprano_export.h>

namespace Soprano {

class Statement;

// FIXME: not a good idea to name a public member of the API "Private"!

class SOPRANO_EXPORT StatementIteratorPrivate : public QSharedData
{
public:
  virtual ~StatementIteratorPrivate();

  /**
   *\return true if there is another Statement
   */
  virtual  bool hasNext() const = 0;

  /**
   *\return the Next Statement
   */
  virtual Statement next() const = 0;
};

}

#endif // SOPRANO_STATEMENT_ITERATOR_PRIVATE_H

