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

#ifndef SOPRANO_QUERY_RESULT_H
#define SOPRANO_QUERY_RESULT_H

#include <QString>
#include <QStringList>
#include <soprano/soprano_export.h>

namespace Soprano {

class Model;
class Node;

class SOPRANO_EXPORT QueryResult
{
public:
  virtual ~QueryResult();

  virtual bool next() const = 0;

  virtual Node binding( const QString &name ) const = 0;

  virtual Node binding( int offset ) const = 0;

  virtual int bindingCount() const = 0;

  virtual const QStringList &bindingNames() const = 0;

  virtual bool isGraph() const = 0;

  virtual bool isBinding() const = 0;

  virtual bool isBool() const = 0;

  virtual bool boolValue() const = 0;

  virtual Model *model() const = 0;
};

}

#endif // SOPRANO_QUERY_RESULT_H

