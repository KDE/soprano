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

#include "simpletransactionstatementiteratorbackend.h"
#include "simpletransactionmodel_p.h"

Soprano::Util::SimpleTransactionStatementIteratorBackend::SimpleTransactionStatementIteratorBackend( const StatementIterator& originalIt, SimpleTransactionPrivate* transactionPrivate )
    : IteratorBackend<Statement>(),
      m_transactionPrivate( transactionPrivate ),
      m_iterator( originalIt ),
      m_addedCacheIterator( transactionPrivate->addedStatements.constBegin() ),
      m_iteratorDone( false )
{
    m_transactionPrivate->openIterators.append( this );
}


Soprano::Util::SimpleTransactionStatementIteratorBackend::~SimpleTransactionStatementIteratorBackend()
{
}


bool Soprano::Util::SimpleTransactionStatementIteratorBackend::next()
{
    if( !m_transactionPrivate ) {
        setError( "Invalid transaction iterator." );
        return false;
    }

    if( !m_iteratorDone ) {
        while( m_iterator.next() ) {
            // check if we are actually allowed to show this statement
            if( !m_transactionPrivate->removedStatements.contains( *m_iterator ) )
                return true;
        }

        m_iteratorDone = true;
        return m_addedCacheIterator != m_transactionPrivate->addedStatements.end();
    }

    if( m_addedCacheIterator != m_transactionPrivate->addedStatements.end() ) {
        ++m_addedCacheIterator;
        return m_addedCacheIterator != m_transactionPrivate->addedStatements.end();
    }
    else {
        return false;
    }
}


Soprano::Statement Soprano::Util::SimpleTransactionStatementIteratorBackend::current() const
{
    if( !m_transactionPrivate ) {
        setError( "Invalid transaction iterator." );
        return Statement();
    }

    if( m_iteratorDone ) {
        if( m_addedCacheIterator != m_transactionPrivate->addedStatements.end() )
            return *m_addedCacheIterator;
        else
            return Statement();
    }
    else {
        return m_iterator.current();
    }
}


void Soprano::Util::SimpleTransactionStatementIteratorBackend::close()
{
    m_iterator.close();
    if( m_transactionPrivate ) {
        m_transactionPrivate->openIterators.removeAll( this );
        m_transactionPrivate = 0;
    }
}
