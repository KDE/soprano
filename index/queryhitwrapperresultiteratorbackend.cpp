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

#include "queryhitwrapperresultiteratorbackend.h"

Soprano::Index::QueryHitWrapperResultIteratorBackend::QueryHitWrapperResultIteratorBackend( const Iterator<QueryHit>& it )
    : QueryResultIteratorBackend(),
      m_it( it )
{
    m_bingingNameCache += QLatin1String( "resource" );
    m_bingingNameCache += QLatin1String( "score" );
}


Soprano::Index::QueryHitWrapperResultIteratorBackend::~QueryHitWrapperResultIteratorBackend()
{
}


bool Soprano::Index::QueryHitWrapperResultIteratorBackend::next()
{
    return m_it.next();
}


void Soprano::Index::QueryHitWrapperResultIteratorBackend::close()
{
    m_it.close();
}


Soprano::Node Soprano::Index::QueryHitWrapperResultIteratorBackend::binding( const QString &name ) const
{
    if ( name == m_bingingNameCache[0] ) {
        return m_it.current().resource();
    }
    else if ( name == m_bingingNameCache[1] ) {
        return LiteralValue( m_it.current().score() );
    }
    else {
        return Node();
    }
}


Soprano::Node Soprano::Index::QueryHitWrapperResultIteratorBackend::binding( int offset ) const
{
    switch( offset ) {
    case 0:
        return m_it.current().resource();
    case 1:
        return LiteralValue( m_it.current().score() );
    default:
        return Node();
    }
}


int Soprano::Index::QueryHitWrapperResultIteratorBackend::bindingCount() const
{
    return m_bingingNameCache.count();
}


QStringList Soprano::Index::QueryHitWrapperResultIteratorBackend::bindingNames() const
{
    return m_bingingNameCache;
}
