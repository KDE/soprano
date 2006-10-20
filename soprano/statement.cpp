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

#include <QtGlobal>
#include "world.h"
#include "node.h"
#include "statement.h"

using namespace RDF;

struct Statement::Private
{
  Private() : subject(0L), predicate(0L), object(0L)
  {}
  Node *subject;
  Node *predicate;
  Node *object;
};

Statement::Statement()
{
  d = new Private;
}

Statement::Statement( const Statement &rhs )
{
  d = new Private;
  d->subject = new Node( *rhs.subject() );
  d->predicate = new Node( *rhs.predicate() );
  d->object = new Node( *rhs.object() );
}

Statement::Statement( Node *subject, Node *predicate, Node *object )
{
  d = new Private;
  d->subject = subject;
  d->predicate = predicate;
  d->object = object;
}

Statement::~Statement()
{
  clear();
  delete d;
}

void Statement::setSubject( Node *node )
{
  d->subject = node;
}

const Node* Statement::subject() const
{
  return d->subject;
}

void Statement::setPredicate( Node *node )
{
  d->predicate = node;
}

const Node* Statement::predicate() const
{
  return d->predicate;
}

void Statement::setObject( Node *node )
{
  d->object = node;
}

const Node* Statement::object() const
{
  return d->object;
}

void Statement::clear()
{
  if ( d->subject ) 
  {
    delete d->subject;
    d->subject = 0L;
  }
  if ( d->predicate ) 
  {
    delete d->predicate;
    d->predicate = 0L;
  }
  if ( d->object ) 
  {
    delete d->object;
    d->object = 0L;
  }
}

bool Statement::isComplete()
{
  return ( d->subject != 0L && d->predicate != 0L && d->object != 0L );
}

librdf_statement *Statement::hook( librdf_world *world ) const
{
  librdf_node *subject = d->subject->hook( world ); 
  librdf_node *predicate = d->predicate->hook( world ); 
  librdf_node *object = d->object->hook( world ); 
 
  return librdf_new_statement_from_nodes( world, subject, predicate, object ); 
}

