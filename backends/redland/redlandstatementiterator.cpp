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

#include "redlandstatementiterator.h"

#include "statement.h"
#include "redlandutil.h"
#include "redlandmodel.h"

#include <QtCore/QtGlobal>
#include <QtCore/QSharedData>
#include <QtCore/QDebug>


Soprano::Redland::RedlandStatementIterator::RedlandStatementIterator( const RedlandModel* model, librdf_stream *s, const Node& forceContext )
    : m_model( model ),
      m_stream( s ),
      m_forceContext( forceContext ),
      m_initialized( false )
{
}


Soprano::Redland::RedlandStatementIterator::~RedlandStatementIterator()
{
    if ( m_model ) {
        m_model->removeIterator( this );
    }
    close();
}


bool Soprano::Redland::RedlandStatementIterator::next()
{
    if ( m_initialized ) {
        // Move to the next element
        librdf_stream_next( m_stream );
    }

    m_initialized = true;

    return ( m_stream ? librdf_stream_end( m_stream ) == 0 : false );
}


Soprano::Statement Soprano::Redland::RedlandStatementIterator::current() const
{
    if ( !m_stream || librdf_stream_end( m_stream ) ) {
        return Statement();
    }

    librdf_statement *st = librdf_stream_get_object( m_stream );
    if ( !st ) {
        // Return a not valid Statement
        // as last value.
        return Soprano::Statement();
    }

    Statement copy = Util::createStatement( st );
    if ( librdf_node* context = static_cast<librdf_node*>( librdf_stream_get_context( m_stream ) ) ) {
        copy.setContext( Util::createNode( context ) );
    }
    else if ( m_forceContext.isValid() ) {
        copy.setContext( m_forceContext );
    }

    return copy;
}


void Soprano::Redland::RedlandStatementIterator::close()
{
    if( m_stream ) {
        librdf_free_stream( m_stream );
        m_stream = 0;
    }
    m_model = 0;
}
