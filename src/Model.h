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

#ifndef MODEL_H
#define MODEL_H

#include <QtGlobal>
#include "Statement.h"

namespace RDF
{

class Model
{
public:
  virtual ~Model();

  virtual void add( const QList<Statement> &statements ) = 0;

  virtual void add( const Model &model ) = 0;

  virtual void add( const Statement &st ) = 0;
  
  virtual Node &createProperty( const QString &ns, const QString &value ) = 0;

  virtual Node &createBlankNode( const QString &uri ) = 0;

  virtual Node &createResource( const QUrl &uri ) = 0;

  virtual Node &createLiteral( const QString &literal ) = 0;

  virtual bool isEmpty() = 0;

  virtual bool contains( const Statement &partial ) = 0;

  virtual void remove( const Statement &st ) = 0;

  virtual void remove( const QList<Statement> &statements ) = 0;

  virtual int size() = 0;

  virtual void write( FILE *fh ) = 0;
};

}

#endif

