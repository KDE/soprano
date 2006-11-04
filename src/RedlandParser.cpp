/* This file is part of Soprano
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

#include <QUrl>
#include <redland.h>

#include "World.h"
#include "RedlandModel.h"
#include "RedlandModelFactory.h"
#include "RedlandParser.h"

using namespace Soprano;

Model *RedlandParser::parse( const QUrl &url ) const
{
  librdf_world *w = World::self()->worldPtr();

  RedlandModelFactory factory;
  
  RedlandModel *model = factory.createMemoryModel( url.toString() );
  if (!model) return 0L;

  librdf_uri *uri = librdf_new_uri( w, (unsigned char *) url.toString().toLatin1().data() );  
  if (!uri) 
  {
    delete model;
    return 0L;
  }


  librdf_parser *parser = librdf_new_parser( w, "rdfxml", "application/rdf+xml", NULL );
  if (!parser)
  {
    delete model;
    librdf_free_uri( uri );
    return 0L;
  }

  if ( librdf_parser_parse_into_model( parser, uri, uri, model->modelPtr() ) )
  {
    delete model;
    model = 0L;
  }

  librdf_free_parser( parser );
  librdf_free_uri( uri );

  return model;
}
