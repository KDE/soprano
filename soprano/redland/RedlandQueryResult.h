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

#ifndef SOPRANO_BACKEND_REDLAND_QUERY_RESULT_H
#define SOPRANO_BACKEND_REDLAND_QUERY_RESULT_H

#include <redland.h>
#include <QString>

#include "QueryResult.h"

namespace Soprano 
{

class Node;
class Model;

namespace Redland
{

class RedlandQueryResult: public Soprano::QueryResult
{
public:
  RedlandQueryResult( librdf_query_results *result );

  ~RedlandQueryResult();

  bool next() const;
    
  Soprano::Node binding( const QString &name ) const;

  Soprano::Node binding( int offset ) const;

  int bindingCount() const;

  const QStringList &bindingNames() const;

  bool isGraph() const;

  bool isBinding() const;

  bool isBool() const;

  bool boolValue() const;

  Model *model() const;

private:
  class Private;
  Private *d;
};
 
}
}

#endif // SOPRANO_BACKEND_REDLAND_QUERY_RESULT_H

