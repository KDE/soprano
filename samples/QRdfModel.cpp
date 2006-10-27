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
#include <QFile>
#include <QTextStream>

#include "../src/Node.h"
#include "../src/Statement.h"
#include "../src/Model.h"
#include "../src/RedlandModelFactory.h"

using namespace RDF;

int
main(int argc, char *argv[])
{
  World world;
  world.open();

  Node subject( QUrl("http://purl.org/net/dagnele/"), Node::Resource );
  Node predicate( QUrl("http://purl.org/dc/elements/1.1/creator"), Node::Resource );
  Node object( QString("Daniele Galdi"), Node::Literal );
  Statement st( subject, predicate, object );

  RedlandModelFactory factory( world );

  //Model *model = factory.createPersistentModel( "test" , "/tmp" );
  Model *model = factory.createMemoryModel( "memory-model" );
  model->add( st );
  
  QFile file("/tmp/model-before-remove.rdf");
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    qDebug( "Cannot create '/tmp/model-before-remove.rdf' file..\n" );
    return (1);
  }

  qDebug( "'cat /tmp/model-before-remove.rdf' to see the model." );

  QTextStream os( &file );
  model->write( os );

  qDebug( "Now we remove the same statement..\n");
 
  model->remove( st );
  
  QFile file1("/tmp/model-after-remove.rdf");
  if (!file1.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    qDebug( "Cannot create '/tmp/model-after-remove.rdf' file..\n" );
    return (1);
  }

  QTextStream os1( &file1 );
  model->write( os1 );

  qDebug( "'cat /tmp/model-after-remove.rdf' to see the model." );

  delete model;

  /* keep gcc -Wall happy */
  return (0);
}
