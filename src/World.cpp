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
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <QtGlobal>

#include "World.h"
using namespace RDF;

World *World::m_instance = 0;

World *World::self()
{
  if ( !m_instance )
  {
    qDebug( "new World()\n" );
    m_instance = new World();
  }

  return m_instance;
}

World::World()
{
  m_world = librdf_new_world();
  librdf_world_open( m_world );
}

World::~World()
{
  librdf_free_world( m_world );
  m_instance = 0;
}

librdf_world* World::worldPtr() const
{
  return m_world;
}

