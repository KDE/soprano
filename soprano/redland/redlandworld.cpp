/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
 * Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>
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

#include "redlandworld.h"
#include "redlandmodel.h"
#include "redlandparser.h"

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QStringList>
#include <QtCore/QDebug>


Q_GLOBAL_STATIC( Soprano::Redland::World, worldInstance );


static QHash<QString, QString> parseOptions( const QStringList& options )
{
  QHash<QString, QString> oh;

  // set default options
  oh["contexts"] = "yes";
  oh["storageType"] = "hashes";
  oh["hash-type"] = "bdb";

  // parse options
  for( QStringList::const_iterator it = options.begin(); it != options.end(); ++it ) {
    QStringList tokens = (*it).split( '=' );
    if( tokens.count() == 2 ) {
      QString key = tokens[0];
      QString value = tokens[1];

      // support some default Soprano options
      if( key == "storagePath" )
          key = "dir";

      oh[key] = value;
    }
    else
      qDebug() << "(Soprano::Redland) invalid option: " << *it << endl;
  }

  return oh;
}


static QString createRedlandOptionString( const QHash<QString, QString>& options )
{
  QStringList os;
  for( QHash<QString, QString>::const_iterator it = options.begin(); it != options.end(); it++ )
    os += QString("%1='%2'").arg( it.key() ).arg( it.value() );
  return os.join(",");
}


Soprano::Redland::World* Soprano::Redland::World::self()
{
  return worldInstance();
}

Soprano::Redland::World::World()
{
  m_world = librdf_new_world();
  librdf_world_open( m_world );
}

Soprano::Redland::World::~World()
{
  librdf_free_world( m_world );
}

librdf_world* Soprano::Redland::World::worldPtr() const
{
  return m_world;
}

Soprano::Redland::RedlandModel* Soprano::Redland::World::createModel() const
{
  // create a new storage
  librdf_storage* storage = librdf_new_storage( worldPtr(), "hashes", NULL, "hash-type='memory',contexts='yes'" );
  if( !storage ) {
    qDebug() << "(Soprano::Redland) storage creation failed!" << endl;
    return 0;
  }

  librdf_model *model = librdf_new_model( worldPtr(), storage, 0 );
  if ( !model ) {
    librdf_free_storage( storage );
    return 0;
  }

  return new RedlandModel( model, storage );
}


Soprano::Redland::RedlandModel* Soprano::Redland::World::createModel( const QString& name, const QStringList& options ) const
{
  QHash<QString, QString> oh = parseOptions( options );

  // check some manditory options
  QString storageType = oh["storageType"];

  // remove unused options from the option hash
  oh.remove( "storageType" );

  Q_ASSERT( !name.isEmpty() );

  QString os = createRedlandOptionString( oh );

  qDebug() << "(Soprano::Redland::BackendPlugin) creating model " << name << " of type " << storageType << " with options " << os << endl;

  // create a new storage
  librdf_storage* storage = librdf_new_storage( worldPtr(),
						storageType.toLatin1().data(),
						name.toLatin1().data(),
						os.toLatin1().data() );
  if( !storage ) {
    qDebug() << "(Soprano::Redland) storage creation failed!" << endl;
    return 0;
  }

  librdf_model *model = librdf_new_model( worldPtr(), storage, 0 );
  if ( !model ) {
    librdf_free_storage( storage );
    return 0;
  }

  return new RedlandModel( model, storage );
}


Soprano::Redland::RedlandParser* Soprano::Redland::World::createParser( const QStringList& options ) const
{
  Q_UNUSED( options );
  return new RedlandParser();
}