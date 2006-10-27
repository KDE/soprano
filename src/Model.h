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

#ifndef MODEL_H
#define MODEL_H

#include <QtGlobal>
#include "Statement.h"
#include "Query.h"

namespace RDF
{

class Node;
class QueryResult;

class Model
{
public:
  virtual ~Model();

  virtual void add( const QList<Statement> &statements ) = 0;

  virtual void add( const Model &model ) = 0;

  virtual void add( const Statement &st ) = 0;
  
  Node *createProperty( const QString &ns, const QString &value );

  Node *createBlankNode( const QString &uri );

  Node *createResource( const QUrl &uri );

  Node *createLiteral( const QString &literal );

  virtual bool isEmpty() const = 0;

  virtual bool contains( const Statement &statement ) const = 0;

  virtual QueryResult *execute( const Query &query ) const = 0;

  virtual void remove( const Statement &st ) = 0;

  virtual void remove( const QList<Statement> &statements ) = 0;

  virtual int size() const = 0;

  virtual void write( FILE *fh ) const = 0;
};

}

#endif

