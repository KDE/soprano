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
#include <iostream>

#include "../src/Model.h"
#include "../src/RedlandParser.h"

using namespace RDF;
using namespace std;

int
main(int argc, char *argv[])
{
  char *program=argv[0];

  if ( argc != 2 ) {
    fprintf(stderr, "USAGE: %s: <RDF source URI>\n", program);
    return (1);
  }

  World world;
  world.open();

  RedlandParser parser;

  Model *model = parser.parse(world, QUrl( (const char*)argv[1] ));
  if (!model) 
  {
    cout << "error during model creation.." << endl;
    return 0L;  
  }
  model->write( stdout );

  delete model;
 
  return (0);
}
