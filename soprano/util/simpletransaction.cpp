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

#include "simpletransaction.h"
#include "simpletransactionmodel.h"
#include "simpletransactionmodel_p.h"
#include "simpletransactionstatementiteratorbackend.h"
#include "statementiterator.h"
#include "nodeiterator.h"
#include "model.h"
#include "node.h"
#include "queryresultiterator.h"

#include <QtCore/QSet>


uint Soprano::qHash( const Soprano::Statement& s )
{
    return( qHash( s.subject().toString() )
            + qHash( s.predicate().toString() )
            + qHash( s.object().toString() )
            + qHash( s.context().toString() ) );
}


bool Soprano::Util::SimpleTransactionPrivate::containsAnyStatement( const Statement& s )
{
    QSet<Statement>::const_iterator end = addedStatements.constEnd();
    for( QSet<Statement>::const_iterator it = addedStatements.constBegin();
         it != end; ++it ) {
        if( s.matches( *it ) &&
            !removedStatements.contains( s ) )
            return true;
    }
    return false;
}


Soprano::Util::SimpleTransaction::SimpleTransaction( SimpleTransactionModelPrivate* modelPrivate )
    : Transaction( modelPrivate->model ),
      d( new SimpleTransactionPrivate() )
{
    d->modelPrivate = modelPrivate;
    d->modelPrivate->mutex.lock();
    d->modelPrivate->openTransactions.append( this );
    d->modelPrivate->mutex.unlock();
}


Soprano::Util::SimpleTransaction::~SimpleTransaction()
{
    if( isValid() ) {
        d->modelPrivate->mutex.lock();
        d->modelPrivate->openTransactions.removeAll( this );
        d->modelPrivate->mutex.unlock();
    }
    invalidate();
    delete d;
}


bool Soprano::Util::SimpleTransaction::isValid() const
{
    return d->modelPrivate != 0;
}


void Soprano::Util::SimpleTransaction::invalidate()
{
    foreach( SimpleTransactionStatementIteratorBackend* backend, d->openIterators ) {
        backend->close();
    }
    d->modelPrivate = 0;
    clearError();
}


Soprano::Error::ErrorCode Soprano::Util::SimpleTransaction::doRollback()
{
    d->addedStatements.clear();
    d->removedStatements.clear();
    clearError();
    return Error::ErrorNone;
}


Soprano::Error::ErrorCode Soprano::Util::SimpleTransaction::doCommit()
{
    clearError();
    Error::ErrorCode c = model()->addStatements( d->addedStatements.toList() );
    if( c == Error::ErrorNone )
        c = model()->removeStatements( d->removedStatements.toList() );
    setError( model()->lastError() );
    d->addedStatements.clear();
    d->removedStatements.clear();
    return c;
}


Soprano::Error::ErrorCode Soprano::Util::SimpleTransaction::addStatement( const Statement& statement )
{
    if( !model()->containsStatement( statement ) )
        d->addedStatements << statement;
    d->removedStatements.remove( statement );
    setError( model()->lastError() );
    return Error::convertErrorCode( lastError().code() );
}


Soprano::Error::ErrorCode Soprano::Util::SimpleTransaction::removeStatement( const Statement& statement )
{
    if( model()->containsStatement( statement ) )
        d->removedStatements << statement;
    d->addedStatements.remove( statement );
    setError( model()->lastError() );
    return Error::convertErrorCode( lastError().code() );
}


Soprano::Error::ErrorCode Soprano::Util::SimpleTransaction::removeAllStatements( const Statement& statement )
{
    if( isValid() ) {
        // clean addedStatements
        QSet<Statement>::iterator it = d->addedStatements.begin();
        while( it != d->addedStatements.end() ) {
            if( it->matches( statement ) ) {
                it = d->addedStatements.erase( it );
            }
            else {
                ++it;
            }
        }

        // the evil hardcore way
        QList<Statement> sl = model()->listStatements( statement ).allStatements();
        d->removedStatements += sl.toSet();
        d->addedStatements -= sl.toSet();
        setError( model()->lastError() );
        return Error::convertErrorCode( lastError().code() );
    }
    else {
        setError( "Cannot change an invalid transaction.", Error::ErrorInvalidOperation );
        return Error::ErrorInvalidOperation;
    }
}


Soprano::StatementIterator Soprano::Util::SimpleTransaction::listStatements( const Statement& partial ) const
{
    if( isValid() ) {
        StatementIterator it = model()->listStatements( partial );
        setError( model()->lastError() );
        if( it.isValid() ) {
            return new SimpleTransactionStatementIteratorBackend( it, d );
        }
        else {
            return StatementIterator();
        }
    }
    else {
        setError( "Cannot check for statements on an invalid transaction.", Error::ErrorInvalidOperation );
        return StatementIterator();
    }
}


bool Soprano::Util::SimpleTransaction::containsStatement( const Statement& statement ) const
{
    if( isValid() ) {
        bool b = ( ( d->addedStatements.contains( statement ) || model()->containsStatement( statement ) ) &&
                   !d->removedStatements.contains( statement ) );
        // in case there was an error in the above containsStatement
        setError( model()->lastError() );
        return b;
    }
    else {
        setError( "Cannot check for statements on an invalid transaction.", Error::ErrorInvalidOperation );
        return false;
    }
}


bool Soprano::Util::SimpleTransaction::containsAnyStatement( const Statement& statement ) const
{
    if( isValid() ) {
        // 1. check if our cache already contains the requested statement
        if( d->containsAnyStatement( statement ) )
            return true;

        // 2. not in cache, thus, we need to do it the hard way
        StatementIterator it = model()->listStatements( statement );
        while( it.next() ) {
            if( !d->removedStatements.contains( it.current() ) )
                return true;
        }

        // in case there was an error in the above listStatements
        setError( model()->lastError() );

        // nothing found
        return false;
    }
    else {
        setError( "Cannot check for statements on an invalid transaction.", Error::ErrorInvalidOperation );
        return false;
    }
}


// sadly, no support for listing contexts
Soprano::NodeIterator Soprano::Util::SimpleTransaction::listContexts() const
{
    if( isValid() ) {
        NodeIterator it = model()->listContexts();
        setError( model()->lastError() );
        return it;
    }
    else {
        setError( "Cannot list contexts in an invalid transaction.", Error::ErrorInvalidOperation );
        return NodeIterator();
    }
}


// sadly, no support for queries
Soprano::QueryResultIterator  Soprano::Util::SimpleTransaction::executeQuery( const QString& query, Query::QueryLanguage language, const QString& userQueryLanguage ) const
{
    if( isValid() ) {
        QueryResultIterator it = model()->executeQuery( query, language, userQueryLanguage );
        setError( model()->lastError() );
        return it;
    }
    else {
        setError( "Cannot execute query on an invalid transaction.", Error::ErrorInvalidOperation );
        return QueryResultIterator();
    }
}


bool Soprano::Util::SimpleTransaction::isEmpty() const
{
    if( isValid() ) {
        bool me = model()->isEmpty();
        setError( model()->lastError() );
        if( me ) {
            return (d->addedStatements - d->removedStatements).isEmpty();
        }
        else {
            // TODO: better idea?
            return statementCount() == 0;
        }
    }
    else {
        setError( "Cannot check for empty on an invalid transaction.", Error::ErrorInvalidOperation );
        return true;
    }
}


int Soprano::Util::SimpleTransaction::statementCount() const
{
    if( isValid() ) {
        int cnt = model()->statementCount() + d->addedStatements.count() - d->removedStatements.count();
        setError( model()->lastError() );
        return cnt;
    }
    else {
        setError( "Cannot count statements in an invalid transaction.", Error::ErrorInvalidOperation );
        return -1;
    }
}


// TODO: move these default implementations to Soprano::Transaction
Soprano::Node Soprano::Util::SimpleTransaction::createBlankNode()
{
    if( isValid() ) {
        Node n = model()->createBlankNode();
        setError( model()->lastError() );
        return n;
    }
    else {
        setError( "Cannot create blank node in an invalid transaction.", Error::ErrorInvalidOperation );
        return Node();
    }
}
