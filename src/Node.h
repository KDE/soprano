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

#ifndef NODE_H
#define NODE_H

#include <QString>
#include <QUrl>

namespace RDF
{

class Node
{
public:

  enum NodeType {
    TypeUnknown = 0,
    TypeResource,
    TypeLiteral,
    TypeBlank
  };

  Node();
  Node(const QUrl &url, NodeType type);
  Node(const QString &value, NodeType type);
  Node(const Node &other);
  ~Node();

  Node &operator=( const Node& other );

  NodeType type() const;

  bool isEmpty() const;
  bool isValid() const ;
  bool isLiteral() const;
  bool isResource() const;
  bool isBlank() const;

  const QUrl &uri() const;
  const QString &literal() const;
  const QString &blank() const;

private:
  class Private;
  Private *d;
};

}

#endif
