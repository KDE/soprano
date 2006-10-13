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
#include "statement.h"

using namespace RDF;

struct Statement::Private
{
  Private() : statement(0L)
  {}
  librdf_statement *statement;
};

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
  librdf_free_statement(d->statement);
  delete d;
}

bool Statement::equals( Statement *s )
{
  return ( librdf_statement_equals(d->statement, s->hook()));
}

librdf_statement* Statement::hook() const
{
  return d->statement;
}

QString Statement::toString() const
{
  return QString( (const char *) librdf_statement_to_string (d->statement));
}

void Statement::setSubject( Node *node )
{
  librdf_statement_set_subject(d->statement, node->hook());
}

void Statement::setPredicate( Node *node )
{
  librdf_statement_set_predicate(d->statement, node->hook());
}

void Statement::setObject( Node *node )
{
  librdf_statement_set_object(d->statement, node->hook());
}

Node* Statement::subject() const
{
  return new Node( librdf_statement_get_subject( d->statement ) );
}

Node* Statement::predicate() const
{
  return new Node( librdf_statement_get_predicate( d->statement ) );
}

Node* Statement::object() const
{
  return new Node( librdf_statement_get_object( d->statement ) );
}

void Statement::clear()
{
  librdf_statement_clear(d->statement);
}

