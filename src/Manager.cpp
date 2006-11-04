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

#include <QStringList>
#include <QString>

#include "RedlandModelFactory.h"
#include "ModelFactory.h"
#include "Manager.h"
using namespace Soprano;

struct Manager::Private {
  Private()
  {}

  QStringList names;
};

Manager::Manager()
{
  d = new Private;
  d->names << "Redland";
}

Manager::~Manager()
{  
  delete d;
}

const QStringList &Manager::listModelFactoryNames() const
{
  return d->names;
}

ModelFactory *Manager::factory( const QString &name ) const
{
  if ( name == "Redland" ) return new RedlandModelFactory();
  return 0L;
}
