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

#include "../src/Node.h"
#include "../src/Statement.h"
#include "../src/Model.h"
#include "../src/Query.h"
#include "../src/QueryResult.h"

#include "../src/World.h"
#include "../src/RedlandModel.h"
#include "../src/RedlandModelFactory.h"

using namespace RDF;

#include <iostream>
using namespace std;

int
main(int argc, char *argv[])
{
  World world;
  world.open();
   
  Node subject0( QUrl("http://purl.org/net/dagnele/"), Node::TypeResource );
  Node predicate0( QUrl("http://purl.org/dc/elements/1.1/creator"), Node::TypeResource );
  Node object0( QString("Daniele Galdi"), Node::TypeLiteral );
  Statement st0( subject0, predicate0, object0 );

  Node subject1( QUrl("http://purl.org/net/dagnele/"), Node::TypeResource );
  Node predicate1( QUrl("http://purl.org/dc/elements/1.1/age"), Node::TypeResource );
  Node object1( QString("29"), Node::TypeLiteral );
  Statement st1( subject1, predicate1, object1 );

  Node subject2( QUrl("http://purl.org/net/dagnele/"), Node::TypeResource );
  Node predicate2( QUrl("http://purl.org/dc/elements/1.1/born"), Node::TypeResource );
  Node object2( QString("Rome"), Node::TypeLiteral );
  Statement st2( subject2, predicate2, object2 );

  RedlandModelFactory factory( world );

  //Model *model = factory.createPersistentModel( "test" , "/tmp" );
  Model *model = factory.createMemoryModel( "test" );
  model->add( st0 );
  model->add( st1 );
  model->add( st2 );
  model->write( stdout );

  // Rdql query
  //Query query("select ?a, ?b, ?c where (?a, ?b, ?c)", Query::RDQL);

  // Sparql query
  Query query("select ?a ?b ?c where {?a ?b ?c}", Query::SPARQL);

  QueryResult *res = model->execute( query );
  std::cout << "Result: " << res->size() << std::endl << std::endl;
  
  while ( res->hasNext() )
  {
    Node *subject = res->get("a");
    Node *predicate = res->get("b");
    Node *object = res->get("c");

    cout << "Subject: " << subject->uri().toString().toStdString() << endl;
    cout << "Predicate: " << predicate->uri().toString().toStdString() << endl;
    cout << "Object: " << object->literal().toStdString() << endl;
    cout << "-------------------------------------------------" << endl;

    delete subject;
    delete predicate;
    delete object;

    res->next();
  }
  
  delete res;
  delete model;

  /* keep gcc -Wall happy */
  return (0);
}
