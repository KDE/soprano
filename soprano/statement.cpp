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
  Private() : statement(0L)
  {}
  librdf_statement *statement;
};

Statement::Statement( World *world)
{
  d = new Private;
  d->statement = librdf_new_statement( world->hook() );
  Q_ASSERT(d->statement != NULL);
}

Statement::Statement( const Statement &rhs )
{
  d = new Private;
  d->statement = librdf_new_statement_from_statement( rhs.hook() );
  Q_ASSERT(d->statement != NULL);
}

Statement::Statement(World *world, Node *subject, Node *predicate, Node *object)
{
  d = new Private;
  d->statement = librdf_new_statement_from_nodes(world->hook(), subject->hook(), predicate->hook(), object->hook());
  Q_ASSERT(d->statement != NULL);
}

Statement::~Statement()
{
  librdf_free_statement( d->statement );
  delete d;
}

void Statement::setSubject( Node *node )
{
  librdf_statement_set_subject( d->statement, node->hook() );
}

Node* Statement::subject() const
{
  return new Node( librdf_statement_get_subject( d->statement ) );
}

void Statement::setPredicate( Node *node )
{
  librdf_statement_set_predicate( d->statement, node->hook() );
}

Node* Statement::predicate() const
{
  return new Node( librdf_statement_get_predicate( d->statement ) );
}

void Statement::setObject( Node *node )
{
  librdf_statement_set_object(d->statement, node->hook());
}

Node* Statement::object() const
{
  return new Node( librdf_statement_get_object( d->statement ) );
}

void Statement::clear()
{
  librdf_statement_clear( d->statement );
}

bool Statement::isComplete()
{
  return librdf_statement_is_complete( d->statement ) != 0; 
}

librdf_statement *Statement::hook() const
{
  return d->statement;
}


