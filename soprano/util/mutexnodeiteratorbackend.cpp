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

#include "mutexnodeiteratorbackend.h"
#include "mutexmodel.h"

#include "node.h"
#include "error.h"


Soprano::Util::MutexNodeIteratorBackend::MutexNodeIteratorBackend( const NodeIterator& it, MutexModel* model )
    : IteratorBackend<Node>(),
      MutexIteratorBase( model ),
      m_iterator( it )
{
}


Soprano::Util::MutexNodeIteratorBackend::~MutexNodeIteratorBackend()
{
}


bool Soprano::Util::MutexNodeIteratorBackend::next()
{
    bool haveNext = m_iterator.next();
    setError( m_iterator.lastError() );
    return haveNext;
}


Soprano::Node Soprano::Util::MutexNodeIteratorBackend::current() const
{
    Node s = m_iterator.current();
    setError( m_iterator.lastError() );
    return s;
}


void Soprano::Util::MutexNodeIteratorBackend::close()
{
    m_iterator.close();
    setError( m_iterator.lastError() );
    remove();
}
