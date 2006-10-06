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
#include "storage.h"

using namespace RDF;

struct Storage::Private
{
  librdf_storage* storage;
  World world;
};

Storage::Storage(const QString &type, const QString &name, const QString &options )
{
  d = new Private;
  d->storage = librdf_new_storage(d->world.worldPtr(), type.toLatin1().data(), name.toLatin1().data(), options.toLatin1().data());
  Q_ASSERT(d->storage != NULL);
   
}

Storage::~Storage()
{
  librdf_free_storage(d->storage);
  delete d;
}

librdf_storage* Storage::storagePtr()
{
  return d->storage;
}

