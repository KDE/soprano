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

#include <QtGlobal>
#include "node.h"
#include <iostream>

using namespace RDF;
using namespace std;

struct Node::Private
{
  Private() : url(0L), value(0L), type(TypeUnknown)
  {}
  NodeType type;
  QUrl *url;
  QString *value;
};

Node::Node(const Node &rhs)
{
  d = new Private;
  if ( rhs.isResource() )
  {
    d->type = TypeResource;
    d->url = new QUrl( *rhs.url() );
  } 
  if ( rhs.isLiteral() )
  {
    d->type = TypeLiteral;
    d->value = new QString( *rhs.literal() );
  }
  if ( rhs.isBlank() )
  {
    d->type = TypeBlank;
    d->value = new QString( *rhs.blank() );
  }
}

Node::Node( librdf_node *node )
{
  d = new Private;
  if ( librdf_node_is_resource( node ) )
  {
    d->type = TypeResource;
    librdf_uri *uri = librdf_node_get_uri( node ); 
  }
  if ( librdf_node_is_literal( node ) )
  {
    d->type = TypeLiteral;
    d->value = new QString( librdf_node_get_literal_value_as_latin1( node ) );
  }
  if ( librdf_node_is_blank( node ) ) 
  {
    d->type = TypeBlank;
    d->value = new QString( (char *) librdf_node_get_blank_identifier( node )  );
  }
}

Node::Node( QUrl *url )
{
  d = new Private;
  d->url = url;
  d->type = TypeResource;
  Q_ASSERT( d->url != NULL );
}

Node::Node( QString *value )
{
  d = new Private;
  d->value = value;
  d->type = TypeLiteral;
  Q_ASSERT(d->value != NULL);
}

Node::~Node()
{
  if ( isResource() )
  {
    cout << "~Node(Resource)" << endl;
    delete d->url;
  }
  if ( isLiteral() || isBlank() )
  {
    cout << "~Node(LorB)" << endl;
    delete d->value;
  }
  delete d;
  cout << "~Node()" << endl;
}

librdf_node* Node::hook( librdf_world *world ) const
{
  if ( isResource() )
  {
    return librdf_new_node_from_uri_string( world, (const unsigned char*) d->url->toString().toLatin1().constData() );
  }
  if ( isLiteral() )
  {
    return librdf_new_node_from_literal( world, (unsigned char*) d->value->toLatin1().constData(), NULL, 0);
  }
  if ( isBlank() )
  {
    return librdf_new_node_from_blank_identifier( world, (unsigned char*) d->value->toLatin1().constData() );
  }
}

bool Node::isLiteral() const
{
  return ( d->type == Node::TypeLiteral );
}

bool Node::isResource() const
{
  return ( d->type == Node::TypeResource );
}

bool Node::isBlank() const
{
  return ( d->type == Node::TypeBlank );
}

Node::NodeType Node::type() const
{
  return d->type;
}

const QUrl *Node::url() const
{
  return d->url;
}

const QString *Node::literal() const
{
  return d->value;
}

const QString *Node::blank() const
{
  return d->value;
}

