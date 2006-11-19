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

#include <QString>
#include <QUrl>
#include "Node.h"

using namespace Soprano;

struct Node::Private
{
  Private(): type(Unknown)
  {}
  Type type;
  QUrl uri;
  QString value;
};

static QUrl emptyQUrl;
static QString emptyQString;

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

Node::Node( const QUrl &uri )
{
  d = new Private;
  d->uri = uri;
  d->type = Node::Resource;
}

Node::Node( const QString &value, Type type )
{
  d = new Private;
  d->value = value;
  d->type = type;
  Q_ASSERT( type == Node::Literal || type == Node::Blank );
}

Node::~Node()
{
  delete d;
}

bool Node::isEmpty() const
{
  return ( d->type == Node::Unknown );
}

bool Node::isValid() const
{
  return !isEmpty();
}

bool Node::isLiteral() const
{
  return ( d->type == Node::Literal );
}

bool Node::isResource() const
{
  return ( d->type == Node::Resource );
}

bool Node::isBlank() const
{
  return ( d->type == Node::Blank );
}

Node::Type Node::type() const
{
  return d->type;
}

const QUrl &Node::uri() const
{
  return ( isResource() ? d->uri : emptyQUrl );
}

const QString &Node::literal() const
{
  return ( isLiteral() ? d->value : emptyQString );
}

const QString &Node::blank() const
{
  return ( isBlank() ? d->value : emptyQString );
}

QString Node::toString() const
{
  if ( isLiteral() || isBlank() ) 
  {
    return d->value;
  }
  if ( isResource() )
  {
    return d->uri.toString();
  }
  return emptyQString;
}

Node& Node::operator=( const Node& other )
{
  d->type = other.type();
  d->uri = other.uri();
  d->value = other.literal();
  return *this;
}

bool Node::operator==( const Node& other ) const
{
  if (d->type != other.d->type)
    return false;
  
  if (d->type == Resource)
    return d->uri == other.d->uri;
  else
    return d->value == other.d->value;
  
  // never reached
  return false;
}
