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
 * You should have received a copy of the GNU Library General Public
 * License along with this library; see the file COPYING.LIB.  If not, 
 * write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301, USA.
 */

#include <knep/registry.h>
#include <knep/service.h>
#include <knep/services/tripleservice.h>
#include <knep/services/statementlistiterator.h>

#include "Query.h"
#include "NepomukUtil.h"
#include "NepomukStatementIterator.h"
#include "NepomukModel.h"

using namespace Nepomuk::Backbone;
using namespace Nepomuk::Backbone::Services;

using namespace Soprano;
using namespace Soprano::Backend::Nepomuk;

struct NepomukModel::Private {
  Private()
  {}

  Registry *registry;
  TripleService *ts;
  Service *service;

  QString name;
};

NepomukModel::NepomukModel( const QString &name )
{
  init();
  
  d->name = name;
  d->ts->addGraph( name );
}

NepomukModel::NepomukModel( const NepomukModel &other )
{
  init();
  
  d->name = other.name();
}

void NepomukModel::init()
{
  d = new Private();
  d->registry = new Registry();
  d->service = d->registry->discoverServiceByType( "http://nepomuk.semanticdesktop.org/services/storage/rdf/Triple" );

  Q_ASSERT( d->service != 0L );
  d->ts = new TripleService( d->service );
}

NepomukModel::~NepomukModel()
{
  delete d->ts;
  delete d->service;
  delete d->registry;
  delete d;
}

Model::ExitCode NepomukModel::add( const Statement &st )
{
  if ( !st.isValid() )
  {
    return Model::ERROR_EXIT;
  }	

  if ( d->ts->addStatement(d->name, Util::createStatement(st)) )
  {
    return Model::ERROR_EXIT;
  }
  return Model::SUCCESS_EXIT;
}

bool NepomukModel::contains( const Statement &statement ) const
{
  if ( !statement.isValid() )
  {
    return false;
  }
	
  return d->ts->contains( d->name, Util::createStatement(statement) );
}

Soprano::QueryResult *NepomukModel::executeQuery( const Query &query ) const
{
  // TODO: When query service will be ready
  return 0L;
}

Soprano::StatementIterator *NepomukModel::listStatements( const Statement &partial ) const
{
  int max = 20;

  RDF::StatementListIterator *iter = new RDF::StatementListIterator( d->ts->listStatements( d->name, Util::createStatement(partial), max), d->ts);

  return new Nepomuk::NepomukStatementIterator( iter );
}

Model::ExitCode NepomukModel::remove( const Statement &st )
{
  if ( !st.isValid() )
  {
    return Model::ERROR_EXIT;
  }

  if ( d->ts->removeStatement( d->name, Util::createStatement(st) ) )
  {
    return Model::ERROR_EXIT;
  }
  return Model::SUCCESS_EXIT;
}

int NepomukModel::size() const
{
  return d->ts->size( d->name );
}

Model::ExitCode NepomukModel::write( QTextStream &os ) const
{
  return Model::ERROR_EXIT;
}

Model::ExitCode NepomukModel::print() const
{ 
  return Model::ERROR_EXIT;
}

const QString &NepomukModel::name() const
{
  return d->name;
} 

