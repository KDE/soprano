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

#ifndef REDLAND_MODEL_FACTORY_H
#define REDLAND_MODEL_FACTORY_H

#include <QtGlobal>
#include "ModelFactory.h"
#include "World.h"

namespace RDF
{

class Model;

class RedlandModelFactory: public ModelFactory
{
public:
  RedlandModelFactory(const World &world);
  ~RedlandModelFactory();

  Model *createMemoryModel( const QString &name );

  Model *createPersistentModel( const QString &name, const QString &filePath );
private:
  RedlandModelFactory( const RedlandModelFactory &other ) {};

  class Private;
  Private *d;
};

}

#endif
