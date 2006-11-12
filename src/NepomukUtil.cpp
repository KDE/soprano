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
 * License along with this library; see the file COPYING.LIB.  If not, 
 * write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301, USA.
 */

#include "Statement.h"
#include "Node.h"

#include "NepomukUtil.h"

using namespace Soprano::Backend::Nepomuk;

RDF::Statement Util::createStatement( const Soprano::Statement &st )
{
  RDF::Node subject = createNode( st.subject() );
  RDF::Node predicate = createNode( st.predicate() );
  RDF::Node object = createNode( st.object() );

  return RDF::Statement( subject, predicate, object);
}

Soprano::Statement Util::createStatement( const RDF::Statement &st )
{
  Soprano::Node subject = createNode( st.subject );
  Soprano::Node predicate = createNode( st.predicate );
  Soprano::Node object = createNode( st.object );

  return Soprano::Statement( subject, predicate, object );
}

RDF::Node Util::createNode( const Soprano::Node &node )
{
  if ( node.isResource() )
  {
    return RDF::Node( node.uri().toString(), RDF::NodeResource ); 
  }
  if ( node.isLiteral() )
  {
    return RDF::Node( node.literal(), RDF::NodeLiteral ); 
  }
  if ( node.isBlank() )
  {
    return RDF::Node( node.blank(), RDF::NodeBlank ); 
  }
  
  return RDF::Node( QString(), RDF::NodeUnknown ); 
}

Soprano::Node Util::createNode( const RDF::Node &node )
{
  if ( node.type == RDF::NodeResource )
  {
    return Soprano::Node( QUrl(node.value) ); 
  }
  if ( node.type == RDF::NodeLiteral )
  {
    return Soprano::Node( node.value, Soprano::Node::Literal ); 
  }
  if ( node.type == RDF::NodeBlank )
  {
    return Soprano::Node( node.value, Soprano::Node::Blank ); 
  }

  return Soprano::Node( node.value, Soprano::Node::Unknown ); 
}

