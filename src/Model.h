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

#ifndef SOPRANO_MODEL_H
#define SOPRANO_MODEL_H


class QString;
class QTextStream;
class QUrl;

namespace Soprano
{

class Node;
class Query;
class QueryResult;
class Statement;
class StatementIterator;

class Model
{
public:
  enum ExitCode {
    SUCCESS_EXIT = 0,
    ERROR_EXIT
  };

  virtual ~Model();

  virtual Model::ExitCode add( const Model &model ) = 0;

  virtual Model::ExitCode add( const Statement &st ) = 0;
  
  Node createProperty( const QString &ns, const QString &value );

  Node createBlankNode( const QString &uri );

  Node createResource( const QUrl &uri );

  Node createLiteral( const QString &literal );

  bool isEmpty() const;

  virtual bool contains( const Statement &statement ) const = 0;

  virtual QueryResult *executeQuery( const Query &query ) const = 0;

  StatementIterator *listStatements() const;

  virtual StatementIterator *listStatements( const Statement &partial ) const = 0;

  virtual Model::ExitCode remove( const Statement &st ) = 0;

  virtual int size() const = 0;

  virtual Model::ExitCode write( QTextStream &os ) const = 0;

  virtual Model::ExitCode print() const = 0;
};

}

#endif // SOPRANO_MODEL_H

