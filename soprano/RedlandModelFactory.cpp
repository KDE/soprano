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

#include <redland.h>
#include "World.h"
#include "RedlandModel.h"
#include "RedlandParser.h"
#include "RedlandModelFactory.h"

using namespace Soprano::Backend::Redland;

struct RedlandModelFactory::Private
{
  Private(): world(0L)
  {}

  librdf_world *world;
};

RedlandModelFactory::RedlandModelFactory()
{
  d = new Private;
  d->world = World::self()->worldPtr();
  Q_ASSERT( d->world != 0 );
}

RedlandModelFactory::~RedlandModelFactory() 
{
  delete d;
}

RedlandModel *RedlandModelFactory::createMemoryModel( const QString &name ) const
{
  return createModel( "memory", name, QString() ); 
}

RedlandModel *RedlandModelFactory::createPersistentModel( const QString &name, const QString &path ) const
{
  QString options("hash-type='bdb',dir='");
  options.append( path );
  options.append("'");

  return createModel( "hashes", name, options ); 
}

RedlandModel *RedlandModelFactory::createModel( const QString &type, const QString &name, const QString &options ) const
{
  librdf_storage *storage = librdf_new_storage( d->world, type.toLatin1().data(), name.toLatin1().data(), options.toLatin1().data() );
  if ( !storage )
  {
    return 0L;
  } 

  librdf_model *model = librdf_new_model( d->world, storage, 0L );
  if ( !model )
  {
    librdf_free_storage( storage );
    return 0L;
  } 

  return new RedlandModel( model );
}


Soprano::Parser *RedlandModelFactory::createParser() const
{
  return new RedlandParser();
}
