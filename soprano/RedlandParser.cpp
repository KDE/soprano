/* 
 * This file is part of Soprano Project
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


#include <redland.h>

#include "World.h"
#include "RedlandModel.h"
#include "RedlandModelFactory.h"
#include "RedlandParser.h"

#include <QtCore>

using namespace Soprano::Backend::Redland;

RedlandParser::RedlandParser()
{
}

RedlandModel *RedlandParser::parse( const QString &url ) const
{
  librdf_world *w = World::self()->worldPtr();

  RedlandModelFactory factory;
  
  RedlandModel *model = factory.createMemoryModel( url );
  if ( !model ) 
  {
    return 0L;
  }

  librdf_uri *uri = librdf_new_uri( w, (unsigned char *) url.toLatin1().data() );  
  if ( !uri ) 
  {
    delete model;
    return 0L;
  }


  librdf_parser *parser = librdf_new_parser( w, "rdfxml", "application/rdf+xml", NULL );
  if ( !parser )
  {
    delete model;
    librdf_free_uri( uri );
    return 0L;
  }

  QFile f( url );
  if ( !f.open( QIODevice::ReadOnly ) )
  {
    qDebug() << "failed to open " << url << endl;
    delete model;
    librdf_free_uri( uri );
    return 0;
  }
  if ( librdf_parser_parse_string_into_model( parser,
					      reinterpret_cast<unsigned char*>( QTextStream( &f ).readAll().toLocal8Bit().data() ),
					      uri,
					      model->modelPtr() ) )
  {
    delete model;
    model = 0L;
  }

  librdf_free_uri( uri );
  librdf_free_parser( parser );

  return model;
}
