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

#include "indexqueryhititeratorbackend.h"
#include "tstring.h"
#include "../soprano/node.h"
#include "cluceneutils.h"

#include <CLucene.h>

#include <QtCore/QString>
#include <QtCore/QUrl>


// FIXME: is it possible to use the stupid CLucene ref counting for the query here?
Soprano::Index::QueryHitIteratorBackend::QueryHitIteratorBackend( lucene::search::Hits* hits,
                                                                  lucene::search::Query* query )
    : m_hits( hits ),
      m_query( query ),
      m_currentDocId( -1 )
{
}


Soprano::Index::QueryHitIteratorBackend::~QueryHitIteratorBackend()
{
    close();
}


bool Soprano::Index::QueryHitIteratorBackend::next()
{
    if ( m_hits ) {
        ++m_currentDocId;
        return m_currentDocId < m_hits->length();
    }
    else {
        setError( "Invalid iterator" );
        return false;
    }
}

namespace {
    Soprano::Node getResource( lucene::document::Document* document )
    {
        QString id = TString( document->get( Soprano::Index::idFieldName().data() ) );
        if ( id.startsWith( Soprano::Index::bnodeIdPrefix() ) ) {
            return Soprano::Node( id.mid( Soprano::Index::bnodeIdPrefix().length() ) );
        }
        else {
            return Soprano::Node( QUrl( id ) );
        }
    }
}

Soprano::Index::QueryHit Soprano::Index::QueryHitIteratorBackend::current() const
{
    if ( m_hits ) {
        if ( m_currentDocId < m_hits->length() ) {
            clearError();
            lucene::document::Document& doc = m_hits->doc( m_currentDocId );
            QueryHit res( getResource( &doc ), m_hits->score( m_currentDocId ) );
            return res;
        }
        else {
            setError( "Iterator at end." );
        }
    }
    else {
        setError( "Invalid iterator" );
    }

    return QueryHit();
}


void Soprano::Index::QueryHitIteratorBackend::close()
{
    clearError();
    if ( m_hits ) {
        _CLDELETE( m_hits );
        m_hits = 0;
        _CLDELETE( m_query );
        m_query = 0;
    }
}
