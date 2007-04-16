/*
 * This file is part of Soprano Project
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

#ifndef SOPRANO_QUERY_H
#define SOPRANO_QUERY_H

#include <soprano/soprano_export.h>

class QString;

namespace Soprano
{

class SOPRANO_EXPORT Query
{
public:
  enum QueryType {
    SPARQL = 0,
    RDQL
  };

  Query( const QString &query, QueryType type);

  Query( const QString &query, QueryType type, int limit, int offset );

  Query( const Query &other );

  ~Query();

  QueryType type() const;

  QString query() const;

  int limit() const;

  void setLimit(int limit);

  int offset() const;

  void setOffset(int offset);
private:
  class Private;
  Private *d;
};

}

#endif
