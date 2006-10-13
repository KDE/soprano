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
#include <iostream>

using namespace RDF;
using namespace std;

struct World::Private
{
  Private() : world(0L)
  {}
  librdf_world* world;
};

World::World()
{
  d = new Private;
  d->world = librdf_new_world();
  Q_ASSERT(d->world != NULL);
}

World::~World()
{
  librdf_free_world( d->world );
  delete d;
}

void World::open()
{
  librdf_world_open( d->world );
}

librdf_world* World::hook()
{
  return d->world;
}

