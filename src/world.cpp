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

librdf_world *World::_world = 0L;
int World::_refcount = 0;

World::World()
{
  cout << "ref:" << _refcount << std::endl;
  if ( _refcount == 0 )
  {
    cout << "Creating world.." << std::endl;
    _world = librdf_new_world();
    librdf_world_open(_world);
  }
  _refcount++;
  Q_ASSERT(_world != NULL);
}

World::~World()
{
  cout << "~ ref:" << _refcount << std::endl;
  _refcount--;
  if ( _refcount == 0 )
  {
    cout << "Destroying world.." << std::endl;
    librdf_free_world(_world);
  }
}

librdf_world* World::worldPtr()
{
  return _world;
}

