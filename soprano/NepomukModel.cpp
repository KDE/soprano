/* 
 * This file is part of Soprano Project.
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

#include <knep/knep.h>
#include <knep/services/tripleservice.h>
#include <knep/services/tripleservicepublisher.h>
#include <knep/services/queryservice.h>

#include "Query.h"
#include "NepomukUtil.h"
#include "NepomukStatementIterator.h"
#include "NepomukModel.h"

using namespace Nepomuk;
using namespace Nepomuk::Backbone;
using namespace Soprano;

struct Backend::Nepomuk::NepomukModel::Private {
  Private() : ts(0L), qs(0L), registry(0L)
  {}

  Services::TripleService *ts;
  Services::QueryService *qs;
  Registry *registry;

  QString name;
};

Backend::Nepomuk::NepomukModel::NepomukModel( const QString &name )
{
  init();
  
  d->name = name;
  d->ts->addGraph( name );
}

Backend::Nepomuk::NepomukModel::NepomukModel( const NepomukModel &other )
{
  init();
  
  d->name = other.name();
}

void Backend::Nepomuk::NepomukModel::init()
{
  d = new Private();
  d->registry = new Registry();
  d->ts = new Services::TripleService( d->registry->discoverServiceByType( "http://nepomuk.semanticdesktop.org/services/storage/rdf/Triple" ) );
  d->qs = new Services::QueryService( d->registry->discoverServiceByType( "http://nepomuk.semanticdesktop.org/services/storage/rdf/Query" ) );

  Q_ASSERT( d->ts != 0L );
  Q_ASSERT( d->qs != 0L );
}

Backend::Nepomuk::NepomukModel::~NepomukModel()
{
  delete d->ts;
  delete d->qs;
  delete d->registry;
  delete d;
}

Soprano::Model::ExitCode Backend::Nepomuk::NepomukModel::add( const Statement &statement )
{
  if ( !statement.isValid() )
  {
    return ERROR_EXIT;
  }

  if ( d->ts->addStatement( d->name, Util::createStatement( statement ) ) )
  {
    return ERROR_EXIT;
  }

  return SUCCESS_EXIT;
}

bool Backend::Nepomuk::NepomukModel::contains( const Statement &statement ) const
{
  RDF::Statement converted = Util::createStatement(statement);

  return d->ts->contains( d->name, converted ) == 1;
}

Soprano::QueryResult *Backend::Nepomuk::NepomukModel::executeQuery( const Query &query ) const
{
  Q_ASSERT( "not tested" != 0L);

  /*  RDF::QueryResult result = d->qs->executeQuery( d->name, query.query(), "rdql", query.offset(), query.limit() );*/



  return 0L;
}

Soprano::StatementIterator *Backend::Nepomuk::NepomukModel::listStatements( const Statement &partial ) const
{
  Q_ASSERT( "not tested" != 0L);

  /*int max = 20;

  RDF::StatementListIterator *iter = new RDF::StatementListIterator( d->ts->listStatements( d->name, Util::createStatement(partial), max), d->ts);

  return new Backend::Nepomuk::NepomukStatementIterator( iter );*/
}

Soprano::Model::ExitCode Backend::Nepomuk::NepomukModel::remove( const Statement &statement )
{
  if ( !statement.isValid() )
  {
    return ERROR_EXIT;
  }

  if ( d->ts->removeStatement( d->name, Util::createStatement( statement ) ) )
  {
    return ERROR_EXIT;
  }
  return SUCCESS_EXIT;
}

Soprano::Model::ExitCode Backend::Nepomuk::NepomukModel::removeAll( const Node &subject, const Node &predicate, const Node &object )
{
  Statement st(subject, predicate, object);

  if ( d->ts->removeAllStatements( d->name, Util::createStatement( st ) ) )
  {
    return ERROR_EXIT;
  }
  return SUCCESS_EXIT;
}

int Backend::Nepomuk::NepomukModel::size() const
{
  return d->ts->size( d->name );
}

Soprano::Model::ExitCode Backend::Nepomuk::NepomukModel::write( QTextStream &os ) const
{
  return ERROR_EXIT;
}

Soprano::Model::ExitCode Backend::Nepomuk::NepomukModel::print() const
{ 
  return ERROR_EXIT;
}

const QString &Backend::Nepomuk::NepomukModel::name() const
{
  return d->name;
} 

