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

#include <QtGlobal>
#include <redland.h>

#include "RedlandModel.h"
#include "RedlandModelFactory.h"
#include "RedlandParser.h"

using namespace Soprano;

Model *RedlandParser::parse( const World &world, const QUrl &url ) const
{
  RedlandModelFactory factory( world );
  
  RedlandModel *model = dynamic_cast<RedlandModel *>( factory.createMemoryModel( url.toString() ) );
  if (!model) return 0L;

  librdf_world *w = world.worldPtr();
  
  librdf_uri *uri = librdf_new_uri( w, (unsigned char *) url.toString().toLatin1().data() );  
  if (!uri) return 0L;

  librdf_parser *parser = librdf_new_parser( w, "rdfxml", "application/rdf+xml", NULL );
  if (!parser) return 0L;

  if ( librdf_parser_parse_into_model( parser, uri, uri, model->modelPtr() ) )
  {
    delete model;
    model = 0L;
  }

  librdf_free_parser( parser );
  librdf_free_uri( uri );

  return model;
}
