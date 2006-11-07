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

#include "Node.h"
#include "Statement.h"

using namespace Soprano;

struct Statement::Private
{
  Private()
  {}
  Node subject;
  Node predicate;
  Node object;
};

Statement::Statement()
{
  d = new Private;
}

Statement::Statement( const Statement &other )
{
  d = new Private;
  d->subject = other.subject();
  d->predicate = other.predicate();
  d->object = other.object();
}

Statement::Statement( const Node &subject, const Node &predicate, const Node &object )
{
  d = new Private;
  d->subject = subject;
  d->predicate = predicate;
  d->object = object;
}

Statement::~Statement()
{
  delete d;
}

Statement& Statement::operator=( const Statement& other )
{
  d->subject = other.subject();
  d->predicate = other.predicate();
  d->object = other.object();

  return *this;
}

void Statement::setSubject( const Node &subject )
{
  d->subject = subject;
}

const Node &Statement::subject() const
{
  return d->subject;
}

void Statement::setPredicate( const Node &predicate )
{
  d->predicate = predicate;
}

const Node &Statement::predicate() const
{
  return d->predicate;
}

void Statement::setObject( const Node &object )
{
  d->object = object;
}

const Node &Statement::object() const
{
  return d->object;
}

bool Statement::isValid() const
{
  return ( d->subject.isValid() && d->predicate.isValid() && d->object.isValid() );
}
