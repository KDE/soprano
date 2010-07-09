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

#include "simplestatementiterator.h"
#include "iteratorbackend.h"

#include "statement.h"


namespace Soprano {
    namespace Util {
        class SimpleStatementIteratorBackend : public IteratorBackend<Statement>
        {
        public:
            SimpleStatementIteratorBackend();
            SimpleStatementIteratorBackend( const QList<Statement>& );
            ~SimpleStatementIteratorBackend();

            SimpleStatementIteratorBackend& operator=( const QList<Statement>& );

            bool next();

            Statement current() const;

            void close() {}

        private:
            QList<Statement> m_statements;
            QList<Statement>::const_iterator m_iterator;
            bool m_first;
        };
    }
}

Soprano::Util::SimpleStatementIteratorBackend::SimpleStatementIteratorBackend()
    : m_first(false)
{
    // make sure we are properly initialized
    operator=( m_statements );
}


Soprano::Util::SimpleStatementIteratorBackend::SimpleStatementIteratorBackend( const QList<Statement>& s )
    : m_first(false)
{
    operator=( s );
}


Soprano::Util::SimpleStatementIteratorBackend::~SimpleStatementIteratorBackend()
{
}


Soprano::Util::SimpleStatementIteratorBackend& Soprano::Util::SimpleStatementIteratorBackend::operator=( const QList<Statement>& s )
{
    m_statements = s;
    m_iterator = m_statements.constBegin();
    m_first = true;
    return *this;
}


bool Soprano::Util::SimpleStatementIteratorBackend::next()
{
    if ( !m_first &&
         m_iterator != m_statements.constEnd() ) {
        ++m_iterator;
    }

    m_first = false;
    return m_iterator != m_statements.constEnd();
}


Soprano::Statement Soprano::Util::SimpleStatementIteratorBackend::current() const
{
    if ( m_iterator != m_statements.constEnd() ) {
        return *m_iterator;
    }
    else {
        return Statement();
    }
}



class Soprano::Util::SimpleStatementIterator::Private
{
public:
};


Soprano::Util::SimpleStatementIterator::SimpleStatementIterator()
    : d( new Private() )
{
}


Soprano::Util::SimpleStatementIterator::SimpleStatementIterator( const QList<Statement>& sl )
    : StatementIterator( new SimpleStatementIteratorBackend( sl ) ),
      d( new Private() )
{
}


Soprano::Util::SimpleStatementIterator::SimpleStatementIterator( const SimpleStatementIterator& st )
    : StatementIterator( st ),
      d( new Private() )
{
}


Soprano::Util::SimpleStatementIterator::~SimpleStatementIterator()
{
    delete d;
}



Soprano::Util::SimpleStatementIterator& Soprano::Util::SimpleStatementIterator::operator=( const QList<Statement>& sl )
{
    setBackend( new SimpleStatementIteratorBackend( sl ) );
    return *this;
}


Soprano::Util::SimpleStatementIterator& Soprano::Util::SimpleStatementIterator::operator=( const SimpleStatementIterator& it )
{
    StatementIterator::operator=( it );
    return *this;
}
