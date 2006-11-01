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
#include "World.h"
#include "Model.h"
#include "RedlandModel.h"
#include "RedlandModelFactory.h"
using namespace RDF;

RedlandModelFactory::RedlandModelFactory() 
{
}

RedlandModelFactory::~RedlandModelFactory() 
{
}

Model *RedlandModelFactory::createMemoryModel( const QString &name ) const
{
  librdf_world *world = World::self()->worldPtr();

  librdf_storage *storage = librdf_new_storage( world, "memory", name.toLatin1().data(), 0L );
  Q_ASSERT( storage != 0L);
  
  librdf_model *model = librdf_new_model( world, storage, 0L );
  Q_ASSERT( model != 0L);

  return new RedlandModel( model );
}

Model *RedlandModelFactory::createPersistentModel( const QString &name, const QString &filePath ) const
{
  QString prefix("hash-type='bdb',dir='");
  prefix.append(filePath);
  prefix.append("'");

  librdf_world *world = World::self()->worldPtr();

  librdf_storage *storage = librdf_new_storage( world, "hashes", name.toLatin1().data(), prefix.toLatin1().data() );
  Q_ASSERT( storage != 0L);

  librdf_model *model = librdf_new_model( world, storage, 0L );
  Q_ASSERT( model != 0L);

  return new RedlandModel( model );
}
