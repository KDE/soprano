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
 * License along with this library; see the file COPYING.LIB.  If not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <redland.h>

/* one prototype needed */
int main(int argc, char *argv[]);

int
main(int argc, char *argv[]) 
{
  World *world;
  Storage *storage;
  Parser* parser;
  Model* model;
  //Stream* stream;
  Node *subject, *predicate;
  //Iterator* iterator;
  Statement *partial_statement, *statement2;
  char *program=argv[0];
  QUrl *uri;
  char *parser_name=NULL;
  int count;

  if(argc <2 || argc >3) {
    fprintf(stderr, "USAGE: %s: <RDF source URI> [rdf parser name]\n", program);
    return(1);
  }

  World *world = new Worl();
  world->open();

  uri = new QUrl( (const unsigned char*)argv[1] );
  if(!uri) {
    fprintf(stderr, "%s: Failed to create URI\n", program);
    return(1);
  }

  storage = new Storage(world, "memory", "test", NULL);
  if(!storage) {
    fprintf(stderr, "%s: Failed to create new storage\n", program);
    return(1);
  }

  model=librdf_new_model(world, storage, NULL);
  if(!model) {
    fprintf(stderr, "%s: Failed to create model\n", program);
    return(1);
  }
  
      
  if(argc==3)
    parser_name=argv[2];

  parser=librdf_new_parser(world, parser_name, NULL, NULL);
  if(!parser) {
    fprintf(stderr, "%s: Failed to create new parser\n", program);
    return(1);
  }


  /* PARSE the URI as RDF/XML*/
  fprintf(stdout, "%s: Parsing URI %s\n", program, librdf_uri_as_string(uri));
  if(librdf_parser_parse_into_model(parser, uri, uri, model)) {
    fprintf(stderr, "%s: Failed to parse RDF into model\n", program);
    return(1);
  }
  librdf_free_parser(parser);

  
  statement2=librdf_new_statement_from_nodes(world, librdf_new_node_from_uri_string(world, (const unsigned char*)"http://purl.org/net/dajobe/"),
                                             librdf_new_node_from_uri_string(world, (const unsigned char*)"http://purl.org/dc/elements/1.1/title"),
                                             librdf_new_node_from_literal(world, (const unsigned char*)"My Home Page", NULL, 0)
                                             );
  librdf_model_add_statement(model, statement2);

  /* Free what we just used to add to the model - now it should be stored */
  librdf_free_statement(statement2);


  /* Print out the model*/


  fprintf(stdout, "%s: Resulting model is:\n", program);
  librdf_model_print(model, stdout);

  /* Construct the query predicate (arc) and object (target) 
   * and partial statement bits
   */
  subject=librdf_new_node_from_uri_string(world, (const unsigned char*)"http://purl.org/net/dajobe/");
  predicate=librdf_new_node_from_uri_string(world, (const unsigned char*)"http://purl.org/dc/elements/1.1/title");
  if(!subject || !predicate) {
    fprintf(stderr, "%s: Failed to create nodes for searching\n", program);
    return(1);
  }
  partial_statement=librdf_new_statement(world);
  librdf_statement_set_subject(partial_statement, subject);
  librdf_statement_set_predicate(partial_statement, predicate);


  /* QUERY TEST 1 - use find_statements to match */

  fprintf(stdout, "%s: Trying to find_statements\n", program);
  stream=librdf_model_find_statements(model, partial_statement);
  if(!stream) {
    fprintf(stderr, "%s: librdf_model_find_statements returned NULL stream\n", program);
  } else {
    count=0;
    while(!librdf_stream_end(stream)) {
      librdf_statement *statement=librdf_stream_get_object(stream);
      if(!statement) {
        fprintf(stderr, "%s: librdf_stream_next returned NULL\n", program);
        break;
      }
      
      fputs("  Matched statement: ", stdout);
      librdf_statement_print(statement, stdout);
      fputc('\n', stdout);
      
      librdf_stream_next(stream);
      count++;
    }
    librdf_free_stream(stream);  
    fprintf(stderr, "%s: got %d matching statements\n", program, count);
  }


  /* QUERY TEST 2 - use get_targets to do match */
  fprintf(stdout, "%s: Trying to get targets\n", program);
  iterator=librdf_model_get_targets(model, subject, predicate);
  if(!iterator)  {
    fprintf(stderr, "%s: librdf_model_get_targets failed to return iterator for searching\n", program);
    return(1);
  }

  count=0;
  while(!librdf_iterator_end(iterator)) {
    librdf_node *target;
    
    target=(librdf_node*)librdf_iterator_get_object(iterator);
    if(!target) {
      fprintf(stderr, "%s: librdf_iterator_get_object returned NULL\n", program);
      break;
    }

    fputs("  Matched target: ", stdout);
    librdf_node_print(target, stdout);
    fputc('\n', stdout);

    count++;
    librdf_iterator_next(iterator);
  }
  librdf_free_iterator(iterator);
  fprintf(stderr, "%s: got %d target nodes\n", program, count);

  librdf_free_statement(partial_statement);
  /* the above does this since they are still attached */
  /* librdf_free_node(predicate); */
  /* librdf_free_node(object); */

  librdf_free_model(model);

  librdf_free_storage(storage);

  librdf_free_uri(uri);

  librdf_free_world(world);

#ifdef LIBRDF_MEMORY_DEBUG
  librdf_memory_report(stderr);
#endif
	
  /* keep gcc -Wall happy */
  return(0);
}
