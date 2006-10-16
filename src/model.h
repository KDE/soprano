/* This file is part of QRDF
 *
 * Copyright (C) 2006 Duncan Mac-Vicar <duncan@kde.org>
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

#ifndef MODEL_H
#define MODEL_H

#include "storage.h"

namespace RDF
{

class Statement;
class Node;
class World;

class Model
{
public:
  
  Model(const Model &rhs);
  
  Model( World *world, Storage *storage, const QString &options );
  
  ~Model();
  
  bool containsStatement( Statement *s ) const;
  
  void add( Node *subject, Node *predicate, Node *object );

  void addStringLiteralStatement( Node *subject, Node *predicate, const QString &literal );
  
  int size() const;

  void addStatement( Statement *s );

  void removeStatement( Statement *s);

  void print( FILE *fh );

  librdf_model* hook() const;
private:
  class Private;
  Private *d;
};

}

#endif

