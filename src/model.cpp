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

#include <redland.h>
#include "model.h"
#include "world.h"
#include "statement.h"
#include "query.h"
#include "query_result.h"

using namespace RDF;

struct Model::Private
{
  Private() : model(0L), world(0L)
  {}
  librdf_model *model;
  librdf_world *world;
};

librdf_node *redland_node( librdf_world *world, Node *node )
{
  librdf_node *converted = node->hook( world );
  delete node;

  return converted;
}

Model::Model( const Model &rhs )
{
  d = new Private;
  d->model = librdf_new_model_from_model( rhs.hookModel() );
  d->world = rhs.hookWorld();
  Q_ASSERT(d->model != 0L);
}

Model::Model( World *world, Storage *storage, const QString &options )
{
  d = new Private;
  d->model = librdf_new_model(world->hook(), storage->hook(), options.toLatin1().data());
  d->world = world->hook();
  Q_ASSERT(d->model != 0L);
}

Model::~Model()
{
  librdf_free_model(d->model);
  delete d;
}

bool Model::containsStatement( Statement *s ) const
{
  return ( librdf_model_contains_statement(d->model, s->hook( d->world )) != 0 );
}

void Model::add( Node *subject, Node *predicate, Node *object )
{
  librdf_node *s = redland_node( d->world, subject );
  librdf_node *p = redland_node( d->world, predicate );
  librdf_node *o = redland_node( d->world, object );

  librdf_model_add( d->model, s, p, o );
}


void Model::addStringLiteralStatement( Node *subject, Node *predicate, QString *literal )
{
  librdf_node *s = redland_node( d->world, subject );
  librdf_node *p = redland_node( d->world, predicate );

  librdf_model_add_string_literal_statement ( d->model, s, p, (const unsigned char*) literal->toLatin1().constData(), NULL, 0);
  delete literal;
}

int Model::size() const
{
  return librdf_model_size( d->model );
}

void Model::addStatement( Statement *s )
{
  librdf_statement *st = s->hook( d->world);
  librdf_model_add_statement( d->model, st );
  librdf_free_statement( st );
}

void Model::removeStatement( Statement *s )
{
  librdf_statement *st = s->hook( d->world);
  librdf_model_remove_statement( d->model, s->hook( d->world ) );
  librdf_free_statement( st );
}

void Model::print( FILE *fh )
{
  librdf_model_print( d->model, fh );
}

QueryResult *Model::executeQuery( Query *query )
{
  /*
   * RedlandFactory->query( Query *query );
   *
   * librdf_query *q = RedlandFactory->query( query );
   */

  return new QueryResult( librdf_model_query_execute( d->model, query->hook( d->world ) ) );
}

librdf_model* Model::hookModel() const
{
  return d->model;
}

librdf_world *Model::hookWorld() const
{
  return d->world;
}

