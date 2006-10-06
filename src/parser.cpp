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
#include "parser.h"

using namespace RDF;

struct Parser::Private
{
  Private() : parser(0L)
  {}
  librdf_parser *parser;
  World world;
};

Parser::Parser(const QString &name)
{
  d = new Private;
  d->parser = librdf_new_parser(d->world.worldPtr(), name.toLatin1().data(), NULL, NULL);
  Q_ASSERT(d->parser != NULL);
}

Parser::~Parser()
{
  librdf_free_parser(d->parser);
  delete d;
}

librdf_parser* Parser::parserPtr()
{
  return d->parser;
}

