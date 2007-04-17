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

#include "node.h"

#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtCore/QDebug>


using namespace Soprano;

class Node::Private : public QSharedData
{
public:
  Private(): type(Unknown)
  {}
  Type type;
  QUrl uri;
  QString value;
  QUrl datatype;
  QString language;
};


Node::Node()
{
  d = new Private;
}

Node::Node( const Node &other )
{
  d = other.d;
}

Node::Node( const QUrl &uri )
{
  d = new Private;
  if( !uri.isEmpty() ) {
    d->uri = uri;
    d->type = Node::Resource;
  }
}

Node::Node( const QString &value, Type type, const QUrl& datatype, const QString& lang )
{
  d = new Private;
  d->type = type;
  if( type == Resource )
    d->uri = value;
  else
    d->value = value;

  if( type == Literal ) {
    d->datatype = datatype;
    d->language = lang;
  }
}

Node::~Node()
{
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

QUrl Node::uri() const
{
  // d->uri is only defined for Resource Nodes
  return d->uri;
}


QString Node::literal() const
{
  if ( isLiteral() )
      return d->value;
  return QString();
}

QString Node::blank() const
{
  if ( isBlank() )
    return d->value;
  return QString();
}

QUrl Node::dataType() const
{
  return d->datatype;
}

QString Node::language() const
{
  return d->language;
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
  return QString();
}

Node& Node::operator=( const Node& other )
{
  d = other.d;
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


QDebug operator<<( QDebug s, const Soprano::Node& n )
{
  switch(n.type()) {
  case Soprano::Node::Unknown:
    s.nospace() << "(null)";
    break;
  case Soprano::Node::Blank:
    s.nospace() << "(blank)";
    break;
  case Soprano::Node::Literal:
    s.nospace() << n.literal();
    if( !n.language().isEmpty() )
      s.nospace() << " (" << n.language() << ")";
    break;
  default:
    s.nospace() << n.uri();
    break;
  }
  return s.space();
}
