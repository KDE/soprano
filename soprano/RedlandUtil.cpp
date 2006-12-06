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
 * You should have received a copy of the GNU Library General Public
 * License along with this library; see the file COPYING.LIB.  If 
 * not, write to the Free Software Foundation, Inc., 51 Franklin 
 * Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <QUrl>
#include <QtGlobal>

#include "Node.h"
#include "Query.h"
#include "Statement.h"
#include "World.h"

#include "RedlandUtil.h"

using namespace Soprano::Backend::Redland;

Soprano::Node Util::createNode( librdf_node *node )
{
  if ( librdf_node_is_resource( node ) ) 
  {
    librdf_uri *uri = librdf_node_get_uri( node );
    return Soprano::Node( QUrl( (const char *)librdf_uri_as_string(uri) ) );
  }
  else if ( librdf_node_is_literal( node ) )
  {
    return Soprano::Node( (const char *)librdf_node_get_literal_value( node ), Node::Literal );
  }
  else if ( librdf_node_is_blank( node ) )
  {
    return Soprano::Node( (const char *)librdf_node_get_blank_identifier( node ), Node::Blank );
  }

  return Soprano::Node();
}

librdf_node *Util::createNode( const Node &node )
{
  librdf_world *world = World::self()->worldPtr();

  if ( node.isResource() )
  {
    return librdf_new_node_from_uri_string( world, (unsigned char *)node.uri().toString().toLatin1().data());
  }
  else if ( node.isLiteral() )
  {
    return librdf_new_node_from_literal( world, (unsigned char *)node.literal().toLatin1().data(), 0L, 0L );
  }
  else if ( node.isBlank() )
  {
    return librdf_new_node_from_blank_identifier( world, (unsigned char *) node.blank().toLatin1().data() );
  }
  else
    Q_ASSERT(false);

  return 0L;
}

librdf_statement *Util::createStatement( const Statement &statement )
{
  librdf_world *world = World::self()->worldPtr();

  librdf_node *subject = createNode( statement.subject() );
  librdf_node *predicate = createNode( statement.predicate() );
  librdf_node *object = createNode( statement.object() );

  return librdf_new_statement_from_nodes( world, subject, predicate, object );
}

Soprano::Statement Util::createStatement( librdf_statement *st )
{
  librdf_node *subject = librdf_statement_get_subject( st );
  librdf_node *predicate = librdf_statement_get_predicate( st );
  librdf_node *object = librdf_statement_get_object( st );

  return Soprano::Statement( createNode( subject), createNode( predicate), createNode( object ) );
}

const char *Util::queryType( const Query &query )
{
  if ( query.type() == Query::RDQL )
  {
    return "rdql";
  }
  else if (query.type() == Query::SPARQL )
  {
    return "sparql";
  }
  return 0L;
}

