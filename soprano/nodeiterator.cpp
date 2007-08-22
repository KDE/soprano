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

#include "nodeiterator.h"
#include "nodeiteratorbackend.h"
#include "node.h"


class Soprano::NodeIterator::Private : public QSharedData
{
public:
    Private()
        : backend( 0 ) {
    }

    ~Private() {
        delete backend;
    }

    NodeIteratorBackend* backend;
};


Soprano::NodeIterator::NodeIterator()
    : d( new Private() )
{
}

Soprano::NodeIterator::NodeIterator( NodeIteratorBackend *sti )
    : d( new Private() )
{
    d->backend = sti;
}

Soprano::NodeIterator::NodeIterator( const NodeIterator &other )
    : d( other.d )
{
}

Soprano::NodeIterator::~NodeIterator()
{
}

Soprano::NodeIterator& Soprano::NodeIterator::operator=( const NodeIterator& other )
{
    d = other.d;
    return *this;
}

void Soprano::NodeIterator::setBackend( NodeIteratorBackend* b )
{
    if ( d->backend != b ) {
        // now we want it to detach
        d->backend = b;
    }
}

bool Soprano::NodeIterator::next()
{
    // some evil hacking to avoid detachment of the shared data
    const Private* cd = d.constData();
    return isValid() ? cd->backend->next() : false;
}

Soprano::Node Soprano::NodeIterator::current() const
{
    return isValid() ? d->backend->current() : Node();
}

Soprano::Node Soprano::NodeIterator::operator*() const
{
    return current();
}

bool Soprano::NodeIterator::isValid() const
{
    return d->backend != 0;
}

QList<Soprano::Node> Soprano::NodeIterator::allNodes()
{
    QList<Node> sl;
    while ( next() ) {
        sl.append( current() );
    }
    return sl;
}
