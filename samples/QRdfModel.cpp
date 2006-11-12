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
#include <iostream>

#include <soprano/Soprano.h>

using namespace Soprano;

int
main(int argc, char *argv[])
{
  
  Manager manager;

  Node subject( QUrl("http://purl.org/nepomuk#uri:nepomuk:3") );
  Node predicate( QUrl("http://purl.org/nepomuk#Model") );
  Node object( QString("Test Modelssdrgg"), Node::Literal );
  Statement st( subject, predicate, object );
  
  //ModelFactory *factory = manager.factory( "Nepomuk" );
  ModelFactory *factory = manager.factory( "Redland" );

  //Model *model = factory->createPersistentModel( "nuovo-graph" );
  Model *model = factory->createMemoryModel( "memory-model" );
  //model->print(); 
 
  model->add( st );
  
  st.setObject( Node("New Object", Node::Literal) );
  model->add( st );

  model->print();
		
  Node dummy;
  Statement partial( subject, predicate, dummy);
  StatementIterator *iter = model->listStatements(partial);
  if ( !iter )
  {
    std::cout << "Failure..." << std::endl;
    return (1);
  }

  while ( iter->hasNext())    
  {
    Statement st = iter->next();
    std::cout << "subject: " << st.subject().uri().toString().toLatin1().data() << std::endl;
    std::cout << "predicate: " << st.predicate().uri().toString().toLatin1().data() << std::endl;
    std::cout << "object: " << st.object().literal().toLatin1().data() << std::endl;
  }
  std::cout << std::endl;
  delete iter;
  
  //model->print();

  delete model;
  delete factory;

  /* keep gcc -Wall happy */
  return (0);
}
