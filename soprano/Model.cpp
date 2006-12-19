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

#include <QtCore>
#include <QList>

#include "Node.h"
#include "Statement.h"
#include "StatementIterator.h"

#include "Model.h"

using namespace Soprano;

Model::~Model()
{
}

Model::ExitCode Model::add( const Model &model )
{
  StatementIterator stmi = model.listStatements();

  while ( stmi.hasNext() )
  {
    if ( !add( stmi.next() ) ) 
    {
      return Model::ERROR_EXIT;
    }
  }

  return Model::SUCCESS_EXIT;
}

Model::ExitCode Model::add( const StatementIterator &iter )
{
  while ( iter.hasNext() )
  {
    if ( !add( iter.next() ) ) 
    {
      return Model::ERROR_EXIT;
    }
  }

  return Model::SUCCESS_EXIT;
}

Model::ExitCode Model::add( const QList<Statement> &statements )
{
  QListIterator<Statement> iter(statements);
  while ( iter.hasNext() )
  {
    if ( !add( iter.next() ) )
    {
      return Model::ERROR_EXIT;
    }
  }

  return Model::SUCCESS_EXIT;
}

Node Model::createPredicate( const QString &ns, const QString &value )
{
  if ( ns.isNull() ) 
  {
    return Node( QUrl( value ) );
  }
  return Node( QUrl( ns + '#' + value ) );
}

Node Model::createPredicate( const QUrl &uri )
{
  return Node( uri );
}

Node Model::createBlank( const QString &uri )
{
  return Node( uri, Node::Blank );
}

Node Model::createResource( const QUrl &uri )
{
  return Node( uri );
}

Node Model::createLiteral( const QString &literal )
{
  return Node( literal, Node::Literal );
}

bool Model::isEmpty() const
{
  return size() == 0;
}

StatementIterator Model::listStatements() const
{
  return listStatements( Statement() );
}

StatementIterator Model::listStatements( const Node &subject, const Node &predicate, const Node &object ) const
{
  return listStatements( Statement(subject, predicate, object) );
}

Model::ExitCode Model::removeAll( const Node &subject, const Node &predicate, const Node &object )
{
  StatementIterator iter = listStatements(subject, predicate, object);
  while ( iter.hasNext() )
  {
    if ( !remove( iter.next() ) )
    {
      return Model::ERROR_EXIT;
    }
  }

  return Model::SUCCESS_EXIT;
}

Model::ExitCode Model::removeAll()
{
  return removeAll( Node(), Node(), Node() );
}
