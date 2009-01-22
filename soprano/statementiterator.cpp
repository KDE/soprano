/*
 * This file is part of Soprano Project.
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

#include "statementiterator.h"
#include "statement.h"
#include "node.h"
#include "iteratorbackend.h"
#include "nodeiterator.h"


Soprano::StatementIterator::StatementIterator()
    : Iterator<Statement>()
{
}

Soprano::StatementIterator::StatementIterator( IteratorBackend<Statement> *sti )
    : Iterator<Statement>( sti )
{
}

Soprano::StatementIterator::StatementIterator( const StatementIterator &other )
    : Iterator<Statement>( other )
{
}

Soprano::StatementIterator::~StatementIterator()
{
}

Soprano::StatementIterator& Soprano::StatementIterator::operator=( const StatementIterator& other )
{
    Iterator<Statement>::operator=( other );
    return *this;
}

class StatementNodeIteratorBackend : public Soprano::IteratorBackend<Soprano::Node>
{
public:
    enum Which {
        SUBJECT,
        PREDICATE,
        OBJECT,
        CONTEXT
    };

    StatementNodeIteratorBackend( const Soprano::StatementIterator& it, Which w )
        : m_it( it ),
          m_which( w ) {
    }

    bool next() {
        return m_it.next();
    }

    Soprano::Node current() const {
        switch( m_which ) {
        case SUBJECT:
            return m_it.current().subject();
        case PREDICATE:
            return m_it.current().predicate();
        case OBJECT:
            return m_it.current().object();
        case CONTEXT:
            return m_it.current().context();
        default:
            return Soprano::Node();
        }
    }

    void close() {
        m_it.close();
    }

    Soprano::Error::Error lastError() const {
        return m_it.lastError();
    }

private:
    Soprano::StatementIterator m_it;
    Which m_which;
};


Soprano::NodeIterator Soprano::StatementIterator::iterateSubjects() const
{
    return new StatementNodeIteratorBackend( *this, StatementNodeIteratorBackend::SUBJECT );
}

Soprano::NodeIterator Soprano::StatementIterator::iteratePredicates() const
{
    return new StatementNodeIteratorBackend( *this, StatementNodeIteratorBackend::PREDICATE );
}

Soprano::NodeIterator Soprano::StatementIterator::iterateObjects() const
{
    return new StatementNodeIteratorBackend( *this, StatementNodeIteratorBackend::OBJECT );
}

Soprano::NodeIterator Soprano::StatementIterator::iterateContexts() const
{
    return new StatementNodeIteratorBackend( *this, StatementNodeIteratorBackend::CONTEXT );
}
