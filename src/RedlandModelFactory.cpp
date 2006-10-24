/* This file is part of QRDF
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
 * You should have received a copy of the GNU Library General Public
 * License
 * along with this library; see the file COPYING.LIB.  If not, write
 * to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <redland.h>
#include "Model.h"
#include "RedlandModel.h"
#include "RedlandModelFactory.h"
using namespace RDF;

struct RedlandModelFactory::Private
{
  Private() {};
  librdf_world *world;
};

RedlandModelFactory::RedlandModelFactory( const World &world ) 
{
  d = new Private;
  d->world = world.worldPtr();
}

RedlandModelFactory::~RedlandModelFactory() 
{
  delete d;
}

Model *RedlandModelFactory::createMemoryModel( const QString &name )
{
  librdf_storage *storage = librdf_new_storage( d->world, "memory", name.toLatin1().data(), 0L );
  librdf_model *model = librdf_new_model( d->world, storage, 0L );

  return new RedlandModel( d->world, model, storage);
}

Model *RedlandModelFactory::createPersistentModel( const QString &name, const QString &filePath )
{
  QString prefix("hash-type='bdb',dir='");
  prefix.append(filePath);
  prefix.append("'");

  librdf_storage *storage = librdf_new_storage( d->world, "hashes", name.toLatin1().data(), prefix.toLatin1().data() );
  librdf_model *model = librdf_new_model( d->world, storage, 0L );

  return new RedlandModel( d->world, model, storage);
}
