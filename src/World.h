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

#ifndef SOPRANO_REDLAND_WORLD_H
#define SOPRANO_REDLAND_WORLD_H

#include <redland.h>

namespace Soprano
{

class World
{
public:
  static World *self();

  ~World();

  librdf_world* worldPtr() const;
private:
  World();
  World( const World &other ) {};

  librdf_world * m_world;

  static World *m_instance;
};

}

#endif // SOPRANO_REDLAND_WORLD_H

