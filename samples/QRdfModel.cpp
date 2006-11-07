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


#include <QtGlobal>
#include <QFile>
#include <QTextStream>
#include <QUrl>

#include <soprano/Soprano.h>

using namespace Soprano;

int
main(int argc, char *argv[])
{
  
  Manager manager;

  Node subject( QUrl("http://purl.org/nepomuk#uri:nepomuk:3"), Node::Resource );
  Node predicate( QUrl("http://purl.org/nepomuk#Model"), Node::Resource );
  Node object( QString("Test Model"), Node::Literal );
  Statement st( subject, predicate, object );

  ModelFactory *factory = manager.factory( "Redland" );

  Model *model = factory->createPersistentModel( "test" );
  //Model *model = factory->createMemoryModel( "memory-model" );
  model->print(); 
 
  model->add( st );
  
  model->print();

  delete model;
  delete factory;

  /* keep gcc -Wall happy */
  return (0);
}
