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

#include "simplenodeiterator.h"
#include "iteratorbackend.h"

#include "statement.h"


namespace Soprano {
    class SimpleNodeIteratorBackend : public IteratorBackend<Node>
    {
    public:
        SimpleNodeIteratorBackend();
        SimpleNodeIteratorBackend( const QList<Node>& );
        ~SimpleNodeIteratorBackend();

        SimpleNodeIteratorBackend& operator=( const QList<Node>& );

        bool next();

        Node current() const;

        void close() {}

    private:
        QList<Node> m_nodes;
        QList<Node>::const_iterator m_iterator;
        bool m_first;
    };
}

Soprano::SimpleNodeIteratorBackend::SimpleNodeIteratorBackend()
{
    // make sure we are properly initialized
    operator=( m_nodes );
}


Soprano::SimpleNodeIteratorBackend::SimpleNodeIteratorBackend( const QList<Node>& s )
{
    operator=( s );
}


Soprano::SimpleNodeIteratorBackend::~SimpleNodeIteratorBackend()
{
}


Soprano::SimpleNodeIteratorBackend& Soprano::SimpleNodeIteratorBackend::operator=( const QList<Node>& s )
{
    m_nodes = s;
    m_iterator = m_nodes.constBegin();
    m_first = true;
    return *this;
}


bool Soprano::SimpleNodeIteratorBackend::next()
{
    if ( !m_first &&
         m_iterator != m_nodes.constEnd() ) {
        ++m_iterator;
    }

    m_first = false;
    return m_iterator != m_nodes.constEnd();
}


Soprano::Node Soprano::SimpleNodeIteratorBackend::current() const
{
    if ( m_iterator != m_nodes.constEnd() ) {
        return *m_iterator;
    }
    else {
        return Node();
    }
}



class Soprano::SimpleNodeIterator::Private
{
public:
};


Soprano::SimpleNodeIterator::SimpleNodeIterator()
    : d( new Private() )
{
}


Soprano::SimpleNodeIterator::SimpleNodeIterator( const QList<Node>& sl )
    : NodeIterator( new SimpleNodeIteratorBackend( sl ) ),
      d( new Private() )
{
}


Soprano::SimpleNodeIterator::SimpleNodeIterator( const SimpleNodeIterator& st )
    : NodeIterator( st ),
      d( new Private() )
{
}


Soprano::SimpleNodeIterator::~SimpleNodeIterator()
{
    delete d;
}



Soprano::SimpleNodeIterator& Soprano::SimpleNodeIterator::operator=( const QList<Node>& sl )
{
    setBackend( new SimpleNodeIteratorBackend( sl ) );
    return *this;
}


Soprano::SimpleNodeIterator& Soprano::SimpleNodeIterator::operator=( const SimpleNodeIterator& it )
{
    NodeIterator::operator=( it );
    return *this;
}
