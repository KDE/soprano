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
#include "Node.h"

using namespace RDF;

struct Node::Private
{
  Private(): type(TypeUnknown)
  {}
  NodeType type;
  QUrl uri;
  QString value;
};

Node::Node()
{
  d = new Private;
}

Node::Node( const Node &other )
{
  d = new Private;
  d->type = other.type();
  d->uri = other.uri();
  d->value = other.literal();
}

Node::Node( const QUrl &uri, NodeType type )
{
  d = new Private;
  d->uri = uri;
  d->type = type;
}

Node::Node( const QString &value, NodeType type )
{
  d = new Private;
  d->value = value;
  d->type = type;
}

Node::~Node()
{
  delete d;
}

bool Node::isEmpty() const
{
  return ( d->type == Node::TypeUnknown );
}

bool Node::isValid() const
{
  return !isEmpty();
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

const QUrl &Node::uri() const
{
  return d->uri;
}

const QString &Node::literal() const
{
  return d->value;
}

const QString &Node::blank() const
{
  return d->value;
}

Node& Node::operator=( const Node& other )
{
  d->type = other.type();
  d->uri = other.uri();
  d->value = other.literal();
  return *this;
}
