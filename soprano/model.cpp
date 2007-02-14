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

#include "node.h"
#include "statement.h"
#include "statementiterator.h"

#include "model.h"

namespace Soprano {

class Model::Private
{
public:
};

Model::Model()
{
  d = new Private;
}

Model::~Model()
{
  delete d;
}

Model::ExitCode Model::add( const Model &model )
{
  StatementIterator stmi = model.listStatements();
  if ( !stmi.isValid() )
  {
    return Model::ERROR_EXIT;
  }
 
  while ( stmi.hasNext() )
  {
    Model::ExitCode c = add( stmi.next() );
    if ( SUCCESS_EXIT != c ) 
    {
      return c;
    }
  }

  return Model::SUCCESS_EXIT;
}

Model::ExitCode Model::add( const StatementIterator &iter, const Node &context )
{
  if ( !iter.isValid() )
  {
    return Model::ERROR_EXIT;
  }

  while ( iter.hasNext() )
  {
    Model::ExitCode c = add( iter.next(), context );
    if ( SUCCESS_EXIT != c ) 
    {
      return c;
    }
  }

  return Model::SUCCESS_EXIT;
}

Model::ExitCode Model::add( const StatementIterator &iter )
{
  if ( !iter.isValid() )
  {
    return Model::ERROR_EXIT;
  }

  while ( iter.hasNext() )
  {
    Model::ExitCode c = add( iter.next() );
    if ( SUCCESS_EXIT != c ) 
    {
      return c;
    }
  }

  return Model::SUCCESS_EXIT;
}

Model::ExitCode Model::add( const QList<Statement> &statements, const Node &context )
{
  for( QList<Statement>::const_iterator it = statements.constBegin();
       it != statements.constEnd(); ++it )
  {
    Model::ExitCode c = add( *it, context );
    if ( SUCCESS_EXIT != c ) 
    {
      return c;
    }
  }

  return Model::SUCCESS_EXIT;
}

Model::ExitCode Model::add( const QList<Statement> &statements )
{
  // FIXME: why not call add with an empty context here?

  QListIterator<Statement> iter(statements);
  while ( iter.hasNext() )
  {
    Model::ExitCode c = add( iter.next() );
    if ( SUCCESS_EXIT != c ) 
    {
      return c;
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

Model::ExitCode Model::removeAll( const Statement &statement, const Node &context )
{
  StatementIterator iter = listStatements(statement, context);
  while ( iter.hasNext() )
  {
    Model::ExitCode c = remove( iter.next() );
    if ( c != SUCCESS_EXIT )
    {
      return c;
    }
  }

  return Model::SUCCESS_EXIT;
}


Model::ExitCode Model::removeAll( const Statement &statement )
{
  return removeAll( statement, Node() );
}

Model::ExitCode Model::removeAll( const Node &subject, const Node &predicate, const Node &object )
{
  return removeAll( Statement( subject, predicate, object ) );
}

Model::ExitCode Model::removeAll()
{
  return removeAll( Node(), Node(), Node() );
}

}
