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

#include "RedlandUtil.h"
using namespace RDF;

Node *Redland::createNode( librdf_node *node )
{
  if ( librdf_node_is_resource( node ) ) 
  {
    librdf_uri *uri = librdf_node_get_uri( node );
    return new Node( QUrl( (const char *)librdf_uri_as_string(uri) ), Node::TypeResource );
  }
  else if ( librdf_node_is_literal( node ) )
  {
    return new Node( librdf_node_get_literal_value_as_latin1( node ), Node::TypeLiteral );
  }
  else if ( librdf_node_is_blank( node ) )
  {
    return new Node( (const char *)librdf_node_get_blank_identifier( node ), Node::TypeBlank );
  }
  return 0L;
}

librdf_node *Redland::createNode( librdf_world *world, const Node &node )
{
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
  return 0L;
}


