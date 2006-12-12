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

#ifndef SOPRANO_BACKEND_NEPOMUK_MODEL_H
#define SOPRANO_BACKEND_NEPOMUK_MODEL_H

#include <QtGlobal>
#include <QTextStream>

#include "Model.h"
#include "Statement.h"

namespace Soprano
{

class QueryResult;
class StatementIterator;

namespace Backend
{
namespace Nepomuk
{

class NepomukModel: public Soprano::Model
{
public:
  explicit NepomukModel( const QString &name );

  NepomukModel( const NepomukModel &other );

  ~NepomukModel();

  Model::ExitCode add( const Statement &st );

  bool contains( const Statement &statement ) const;

  Soprano::QueryResult *executeQuery( const Query &query ) const;

  Soprano::StatementIterator *listStatements( const Statement &partial ) const;

  Model::ExitCode remove( const Statement &st );

  Model::ExitCode removeAll( const Node &subject, const Node &predicate, const Node &object );

  int size() const;

  Model::ExitCode write( QTextStream &os ) const;

  Model::ExitCode print() const;

  const QString &name() const;
private:
  class Private;
  Private *d;

  void init();
}; 

}
}
}

#endif // SOPRANO_BACKEND_NEPOMUK_MODEL_H

