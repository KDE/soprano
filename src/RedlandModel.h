/* This file is part of Soprano
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

#ifndef SOPRANO_REDLAND_MODEL_H
#define SOPRANO_REDLAND_MODEL_H

#include <QtGlobal>
#include <QTextStream>
#include <redland.h>

#include "Model.h"
#include "Statement.h"

namespace Soprano
{

class QueryResult;
class StatementIterator;

class RedlandModel: virtual public Model
{
public:
  RedlandModel( librdf_model *model );

  RedlandModel( const RedlandModel &other );

  virtual ~RedlandModel();

  void add( const QList<Statement> &statements );

  void add( const Model &model );

  void add( const Statement &st );
  
  Node *createProperty( const QString &ns, const QString &value );

  Node *createBlankNode( const QString &uri );

  Node *createResource( const QUrl &uri );

  Node *createLiteral( const QString &literal );

  bool isEmpty() const;

  bool contains( const Statement &statement ) const;

  QueryResult *executeQuery( const Query &query ) const;

  StatementIterator *listStatements() const;

  StatementIterator *listStatements( const Statement &partial ) const;

  void remove( const Statement &st );

  void remove( const QList<Statement> &statements );

  int size() const;

  void write( QTextStream &os ) const;

  void print() const;

  librdf_model *modelPtr() const;

  librdf_storage *storagePtr() const;
private:
  class Private;
  Private *d;
};

}

#endif

