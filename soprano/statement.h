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

#ifndef STATEMENT_H
#define STATEMENT_H

#include <QString>
#include <QUrl>

#include <redland.h>
#include "world.h"
#include "node.h"

namespace RDF
{

class Statement
{
public:
  Statement( const Statement &rhs );
  Statement(Node *subject, Node *predicate, Node *object);
  ~Statement();

  bool equals( Statement *s );

  void setSubject( Node *node );
  void setPredicate( Node *node );
  void setObject( Node *node );

  Node* subject() const;
  Node* predicate() const;
  Node* object() const;

  void clear();

  librdf_statement* statementPtr() const;
  QString toString() const;
private:
  class Private;
  Private *d;
};


}

#endif

