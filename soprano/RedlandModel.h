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

#ifndef SOPRANO_BACKEND_REDLAND_MODEL_H
#define SOPRANO_BACKEND_REDLAND_MODEL_H

#include <QtGlobal>
#include <QTextStream>
#include <redland.h>

#include "Model.h"
#include "Statement.h"

namespace Soprano
{

class ResultSet;
class StatementIterator;

namespace Backend
{
namespace Redland
{

class RedlandModel: public Soprano::Model
{
public:
  explicit RedlandModel( librdf_model *model );

  RedlandModel( const RedlandModel &other );

  ~RedlandModel();

  Model::ExitCode add( const Statement &st );

  bool contains( const Statement &statement ) const;

  Soprano::ResultSet executeQuery( const Query &query ) const;

  Soprano::StatementIterator *listStatements( const Statement &partial ) const;

  Model::ExitCode remove( const Statement &st );

  int size() const;

  Model::ExitCode write( QTextStream &os ) const;

  Model::ExitCode print() const;

  librdf_model *modelPtr() const;

  librdf_storage *storagePtr() const;
private:
  class Private;
  Private *d;
}; 

}
}
}

#endif // SOPRANO_BACKEND_REDLAND_MODEL_H

