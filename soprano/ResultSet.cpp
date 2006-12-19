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

#include "Model.h"
#include "Node.h"
#include "QueryResult.h"

#include "ResultSet.h"

using namespace Soprano;

ResultSet::ResultSet( QueryResult *qr ): d(qr) 
{
}

ResultSet::~ResultSet()
{
  if ( d == 0L ) return; 

  delete d;
}

bool ResultSet::next() const
{
  return d == 0L ? false : d->next();
}

Soprano::Node ResultSet::binding( const QString &name ) const
{
  return d->binding( name );
}

Soprano::Node ResultSet::binding( int offset ) const
{
  return d->binding( offset );
}

int ResultSet::bindingCount() const
{
  return d->bindingCount();
}

const QStringList &ResultSet::bindingNames() const
{
  return d->bindingNames();
}

bool ResultSet::isGraph() const
{
  return d->isGraph();
}

bool ResultSet::isBinding() const
{
  return d->isBinding();
}

bool ResultSet::isBool() const
{
  return d->isBool();
}

bool ResultSet::boolValue() const
{
  return d->boolValue();
}

Soprano::Model *ResultSet::model() const
{
  return d->model();
} 
