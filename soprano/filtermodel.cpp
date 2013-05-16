/*
 * This file is part of Soprano Project.
 *
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

#include "filtermodel.h"
#include "statement.h"
#include "queryresultiterator.h"
#include "statementiterator.h"
#include "nodeiterator.h"
#include "query/query.h"

#include <QtCore/QList>


class Soprano::FilterModel::Private
{
public:
    Private()
        : parent( 0 ) {
    }

    Model* parent;
};


Soprano::FilterModel::FilterModel()
    : Model(),
      d( new Private() )
{
}


Soprano::FilterModel::FilterModel( Model* parent )
    : Model(),
      d( new Private() )
{
    setParentModel( parent );
}


Soprano::FilterModel::~FilterModel()
{
    delete d;
}


Soprano::Model* Soprano::FilterModel::parentModel() const
{
    return d->parent;
}


void Soprano::FilterModel::setParentModel( Model* model )
{
    Q_ASSERT( model != this );

    if ( model != d->parent ) {
        if ( d->parent ) {
            disconnect( d->parent, SIGNAL(statementsAdded()), this, SLOT(slotStatementsAdded()) );
            disconnect( d->parent, SIGNAL(statementsRemoved()), this, SLOT(slotStatementsRemoved()) );
            disconnect( d->parent, SIGNAL(statementAdded(Soprano::Statement)),
                        this, SLOT(slotStatementAdded(Soprano::Statement)) );
            disconnect( d->parent, SIGNAL(statementRemoved(Soprano::Statement)),
                        this, SLOT(slotStatementRemoved(Soprano::Statement)) );
        }
        d->parent = model;
        if ( d->parent ) {
            connect( d->parent, SIGNAL(statementsAdded()), this, SLOT(slotStatementsAdded()) );
            connect( d->parent, SIGNAL(statementsRemoved()), this, SLOT(slotStatementsRemoved()) );
            connect( d->parent, SIGNAL(statementAdded(Soprano::Statement)),
                     this, SLOT(slotStatementAdded(Soprano::Statement)) );
            connect( d->parent, SIGNAL(statementRemoved(Soprano::Statement)),
                     this, SLOT(slotStatementRemoved(Soprano::Statement)) );
        }
    }
}


Soprano::Error::ErrorCode Soprano::FilterModel::addStatement( const Statement &statement )
{
    Q_ASSERT( d->parent );
    Error::ErrorCode c = d->parent->addStatement( statement );
    setError( d->parent->lastError() );
    return c;
}


Soprano::Error::ErrorCode Soprano::FilterModel::addStatement( const Node& subject, const Node& predicate, const Node& object, const Node& context )
{
    return Model::addStatement( subject, predicate, object, context );
}


bool Soprano::FilterModel::isEmpty() const
{
    Q_ASSERT( d->parent );
    bool b = d->parent->isEmpty();
    setError( d->parent->lastError() );
    return b;
}


Soprano::NodeIterator Soprano::FilterModel::listContexts() const
{
    Q_ASSERT( d->parent );
    NodeIterator it = d->parent->listContexts();
    setError( d->parent->lastError() );
    return it;
}


bool Soprano::FilterModel::containsStatement( const Statement &statement ) const
{
    Q_ASSERT( d->parent );
    bool b = d->parent->containsStatement( statement );
    setError( d->parent->lastError() );
    return b;
}


bool Soprano::FilterModel::containsStatement( const Node& subject, const Node& predicate, const Node& object, const Node& context ) const
{
    return Model::containsStatement( subject, predicate, object, context );
}


bool Soprano::FilterModel::containsAnyStatement( const Statement &statement ) const
{
    Q_ASSERT( d->parent );
    bool b = d->parent->containsAnyStatement( statement );
    setError( d->parent->lastError() );
    return b;
}


bool Soprano::FilterModel::containsAnyStatement( const Node& subject, const Node& predicate, const Node& object, const Node& context ) const
{
    return Model::containsAnyStatement( subject, predicate, object, context );
}


// Soprano::QueryResultIterator Soprano::FilterModel::executeQuery( const Query::Query& query ) const
// {
//     Q_ASSERT( d->parent );
//     QueryResultIterator it = d->parent->executeQuery( query );
//     setError( d->parent->lastError() );
//     return it;
// }


Soprano::QueryResultIterator Soprano::FilterModel::executeQuery( const QString& query, Query::QueryLanguage language, const QString& userQueryLanguage ) const
{
    Q_ASSERT( d->parent );
    QueryResultIterator it = d->parent->executeQuery( query, language, userQueryLanguage );
    setError( d->parent->lastError() );
    return it;
}


Soprano::StatementIterator Soprano::FilterModel::listStatements( const Statement &partial ) const
{
    Q_ASSERT( d->parent );
    StatementIterator it = d->parent->listStatements( partial );
    setError( d->parent->lastError() );
    return it;
}


Soprano::StatementIterator Soprano::FilterModel::listStatements( const Node& subject, const Node& predicate, const Node& object, const Node& context ) const
{
    return Model::listStatements( subject, predicate, object, context );
}


Soprano::Error::ErrorCode Soprano::FilterModel::removeStatement( const Statement &statement )
{
    Q_ASSERT( d->parent );
    Error::ErrorCode c = d->parent->removeStatement( statement );
    setError( d->parent->lastError() );
    return c;
}


Soprano::Error::ErrorCode Soprano::FilterModel::removeStatement( const Node& subject, const Node& predicate, const Node& object, const Node& context )
{
    return Model::removeStatement( subject, predicate, object, context );
}


Soprano::Error::ErrorCode Soprano::FilterModel::removeAllStatements( const Statement &statement )
{
    Q_ASSERT( d->parent );
    Error::ErrorCode c = d->parent->removeAllStatements( statement );
    setError( d->parent->lastError() );
    return c;
}


Soprano::Error::ErrorCode Soprano::FilterModel::removeAllStatements( const Node& subject, const Node& predicate, const Node& object, const Node& context )
{
    return Model::removeAllStatements( subject, predicate, object, context );
}


int Soprano::FilterModel::statementCount() const
{
    Q_ASSERT( d->parent );
    int cnt = d->parent->statementCount();
    setError( d->parent->lastError() );
    return cnt;
}


Soprano::Node Soprano::FilterModel::createBlankNode()
{
    Q_ASSERT( d->parent );
    Node n = d->parent->createBlankNode();
    setError( d->parent->lastError() );
    return n;
}


Soprano::Error::ErrorCode Soprano::FilterModel::write( QTextStream &os ) const
{
    Q_ASSERT( d->parent );
    Error::ErrorCode c = d->parent->write( os );
    setError( d->parent->lastError() );
    return c;
}


void Soprano::FilterModel::parentStatementsAdded()
{
    emit statementsAdded();
}


void Soprano::FilterModel::parentStatementsRemoved()
{
    emit statementsRemoved();
}


void Soprano::FilterModel::parentStatementAdded( const Statement& s )
{
    emit statementAdded( s );
}


void Soprano::FilterModel::parentStatementRemoved( const Statement& s )
{
    emit statementRemoved( s );
}


void Soprano::FilterModel::slotStatementsAdded()
{
    parentStatementsAdded();
}


void Soprano::FilterModel::slotStatementsRemoved()
{
    parentStatementsRemoved();
}


void Soprano::FilterModel::slotStatementAdded( const Statement& s )
{
    parentStatementAdded( s );
}


void Soprano::FilterModel::slotStatementRemoved( const Statement& s )
{
    parentStatementRemoved( s );
}

#include "filtermodel.moc"
