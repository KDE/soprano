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

#include "filtertransaction.h"
#include "statement.h"
#include "queryresultiterator.h"
#include "statementiterator.h"
#include "nodeiterator.h"

#include <QtCore/QList>


class Soprano::FilterTransaction::Private
{
public:
    Private()
        : parent( 0 ) {
    }

    Transaction* parent;
};


Soprano::FilterTransaction::FilterTransaction( Transaction* parent, Model* model )
    : Transaction( model ),
      d( new Private() )
{
    Q_ASSERT( parent != this );
    Q_ASSERT( parent );

    d->parent = parent;
    connect( d->parent, SIGNAL(statementsAdded()), this, SLOT(slotStatementsAdded()) );
    connect( d->parent, SIGNAL(statementsRemoved()), this, SLOT(slotStatementsRemoved()) );
    connect( d->parent, SIGNAL(statementAdded(const Soprano::Statement&)), this, SLOT(slotStatementAdded(const Soprano::Statement&)) );
    connect( d->parent, SIGNAL(statementRemoved(const Soprano::Statement&)), this, SLOT(slotStatementRemoved(const Soprano::Statement&)) );
}


Soprano::FilterTransaction::~FilterTransaction()
{
    delete d->parent;
    delete d;
}


Soprano::Transaction* Soprano::FilterTransaction::parentTransaction() const
{
    return d->parent;
}


Soprano::Error::ErrorCode Soprano::FilterTransaction::addStatement( const Statement &statement )
{
    Error::ErrorCode c = d->parent->addStatement( statement );
    setError( d->parent->lastError() );
    return c;
}


bool Soprano::FilterTransaction::isEmpty() const
{
    bool b = d->parent->isEmpty();
    setError( d->parent->lastError() );
    return b;
}


Soprano::NodeIterator Soprano::FilterTransaction::listContexts() const
{
    NodeIterator it = d->parent->listContexts();
    setError( d->parent->lastError() );
    return it;
}


bool Soprano::FilterTransaction::containsStatement( const Statement &statement ) const
{
    bool b = d->parent->containsStatement( statement );
    setError( d->parent->lastError() );
    return b;
}


bool Soprano::FilterTransaction::containsAnyStatement( const Statement &statement ) const
{
    bool b = d->parent->containsAnyStatement( statement );
    setError( d->parent->lastError() );
    return b;
}


Soprano::QueryResultIterator Soprano::FilterTransaction::executeQuery( const QString& query, Query::QueryLanguage language, const QString& userQueryLanguage ) const
{
    QueryResultIterator it = d->parent->executeQuery( query, language, userQueryLanguage );
    setError( d->parent->lastError() );
    return it;
}


Soprano::StatementIterator Soprano::FilterTransaction::listStatements( const Statement &partial ) const
{
    StatementIterator it = d->parent->listStatements( partial );
    setError( d->parent->lastError() );
    return it;
}


Soprano::Error::ErrorCode Soprano::FilterTransaction::removeStatement( const Statement &statement )
{
    Error::ErrorCode c = d->parent->removeStatement( statement );
    setError( d->parent->lastError() );
    return c;
}


Soprano::Error::ErrorCode Soprano::FilterTransaction::removeAllStatements( const Statement &statement )
{
    Error::ErrorCode c = d->parent->removeAllStatements( statement );
    setError( d->parent->lastError() );
    return c;
}


int Soprano::FilterTransaction::statementCount() const
{
    int cnt = d->parent->statementCount();
    setError( d->parent->lastError() );
    return cnt;
}


Soprano::Node Soprano::FilterTransaction::createBlankNode()
{
    Node n = d->parent->createBlankNode();
    setError( d->parent->lastError() );
    return n;
}


Soprano::Error::ErrorCode Soprano::FilterTransaction::write( QTextStream &os ) const
{
    Error::ErrorCode c = d->parent->write( os );
    setError( d->parent->lastError() );
    return c;
}


Soprano::Error::ErrorCode Soprano::FilterTransaction::doRollback()
{
    Error::ErrorCode c = d->parent->rollback();
    setError( d->parent->lastError() );
    return c;
}


Soprano::Error::ErrorCode Soprano::FilterTransaction::doCommit()
{
    Error::ErrorCode c = d->parent->commit();
    setError( d->parent->lastError() );
    return c;
}


void Soprano::FilterTransaction::parentStatementsAdded()
{
    emit statementsAdded();
}


void Soprano::FilterTransaction::parentStatementsRemoved()
{
    emit statementsRemoved();
}


void Soprano::FilterTransaction::parentStatementAdded( const Statement& s )
{
    emit statementAdded( s );
}


void Soprano::FilterTransaction::parentStatementRemoved( const Statement& s )
{
    emit statementRemoved( s );
}


void Soprano::FilterTransaction::slotStatementsAdded()
{
    parentStatementsAdded();
}


void Soprano::FilterTransaction::slotStatementsRemoved()
{
    parentStatementsRemoved();
}


void Soprano::FilterTransaction::slotStatementAdded( const Statement& s )
{
    parentStatementAdded( s );
}


void Soprano::FilterTransaction::slotStatementRemoved( const Statement& s )
{
    parentStatementRemoved( s );
}

#include "filtertransaction.moc"
