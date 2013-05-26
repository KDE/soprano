/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2008 Sebastian Trueg <trueg@kde.org>
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

#include "readonlymodel.h"
#include "statement.h"
#include "queryresultiterator.h"
#include "statementiterator.h"
#include "nodeiterator.h"
#include "query/query.h"


namespace {
    Soprano::Error::Error createPermissionDeniedError() {
        return Soprano::Error::Error( "Soprano::Util::ReadOnlyModel - no write support", Soprano::Error::ErrorPermissionDenied );
    }
}


class Soprano::Util::ReadOnlyModel::Private
{
public:
    Private()
        : parent(0) {
    }
    Soprano::Model* parent;
};


Soprano::Util::ReadOnlyModel::ReadOnlyModel( Model* parentModel )
    : Model(),
      d(new Private())
{
    setParentModel( parentModel );
}


Soprano::Util::ReadOnlyModel::~ReadOnlyModel()
{
    delete d;
}


void Soprano::Util::ReadOnlyModel::setParentModel( Model* model )
{
    if ( model != d->parent ) {
        if ( d->parent ) {
            d->parent->disconnect( this );
        }
        d->parent = model;
        if ( d->parent ) {
            connect( d->parent, SIGNAL(statementsAdded()), this, SIGNAL(statementsAdded()) );
            connect( d->parent, SIGNAL(statementsRemoved()), this, SIGNAL(statementsRemoved()) );
            connect( d->parent, SIGNAL(statementAdded(Soprano::Statement)), this, SIGNAL(statementAdded(Soprano::Statement)) );
            connect( d->parent, SIGNAL(statementRemoved(Soprano::Statement)), this, SIGNAL(statementRemoved(Soprano::Statement)) );
        }
    }
}


Soprano::Error::ErrorCode Soprano::Util::ReadOnlyModel::addStatement( const Statement& )
{
    setError( createPermissionDeniedError() );
    return Error::ErrorPermissionDenied;
}


Soprano::Error::ErrorCode Soprano::Util::ReadOnlyModel::removeStatement( const Statement& )
{
    setError( createPermissionDeniedError() );
    return Error::ErrorPermissionDenied;
}


Soprano::Error::ErrorCode Soprano::Util::ReadOnlyModel::removeAllStatements( const Statement& )
{
    setError( createPermissionDeniedError() );
    return Error::ErrorPermissionDenied;
}


Soprano::Node Soprano::Util::ReadOnlyModel::createBlankNode()
{
    setError( createPermissionDeniedError() );
    return Node();
}


Soprano::NodeIterator Soprano::Util::ReadOnlyModel::listContexts() const
{
    Q_ASSERT( d->parent );
    NodeIterator it = d->parent->listContexts();
    setError( d->parent->lastError() );
    return it;
}


bool Soprano::Util::ReadOnlyModel::containsStatement( const Statement &statement ) const
{
    Q_ASSERT( d->parent );
    bool b = d->parent->containsStatement( statement );
    setError( d->parent->lastError() );
    return b;
}


bool Soprano::Util::ReadOnlyModel::containsAnyStatement( const Statement &statement ) const
{
    Q_ASSERT( d->parent );
    bool b = d->parent->containsAnyStatement( statement );
    setError( d->parent->lastError() );
    return b;
}


Soprano::QueryResultIterator Soprano::Util::ReadOnlyModel::executeQuery( const QString& query, Query::QueryLanguage language, const QString& userQueryLanguage ) const
{
    Q_ASSERT( d->parent );
    QueryResultIterator it = d->parent->executeQuery( query, language, userQueryLanguage );
    setError( d->parent->lastError() );
    return it;
}


Soprano::StatementIterator Soprano::Util::ReadOnlyModel::listStatements( const Statement &partial ) const
{
    Q_ASSERT( d->parent );
    StatementIterator it = d->parent->listStatements( partial );
    setError( d->parent->lastError() );
    return it;
}


int Soprano::Util::ReadOnlyModel::statementCount() const
{
    Q_ASSERT( d->parent );
    int cnt = d->parent->statementCount();
    setError( d->parent->lastError() );
    return cnt;
}


bool Soprano::Util::ReadOnlyModel::isEmpty() const
{
    Q_ASSERT( d->parent );
    bool b = d->parent->isEmpty();
    setError( d->parent->lastError() );
    return b;
}

