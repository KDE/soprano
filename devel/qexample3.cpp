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

#include <QtGlobal>

#include "../src/world.h"
#include "../src/storage.h"
#include "../src/model.h"
#include "../src/node.h"
#include "../src/statement.h"

using namespace RDF;

int
main(int argc, char *argv[])
{
  World *world = new World();
  world->open();
   
  Storage *storage = new Storage( world, "hashes", "test", "hash-type='bdb',dir='.'" );

  Node *subject = new Node( world, QUrl("http://purl.org/net/dagnele/") );
  Node *property = new Node( world, QUrl("http://purl.org/dc/elements/1.1/creator") );
  Node *object = new Node( world, "Daniele Galdi");

  Statement *st = new Statement( world, subject, property, object);

  Model *model = new Model( world, storage, NULL);
  model->addStatement( st );
  model->print( stdout );

  // FIXME: Memory leak
  delete subject;
  delete property;
  delete object;
  // END

  delete st;
  delete model;
  delete storage;
  delete world;

  /* keep gcc -Wall happy */
  return(0);
}
