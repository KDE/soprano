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
#include "redlandworld.h"

#include "statement.h"
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
    close();
}


bool Soprano::Redland::RedlandStatementIterator::next()
{
    clearError();

    if ( m_stream ) {
        if ( m_initialized ) {
            // Move to the next element
            librdf_stream_next( m_stream );
        }

        m_initialized = true;

        if ( librdf_stream_end( m_stream ) ) {
            close();
            return false;
        }
        else {
            return true;
        }
    }
    else {
        setError( "Invalid iterator" );
        return false;
    }
}


Soprano::Statement Soprano::Redland::RedlandStatementIterator::current() const
{
    if ( !m_stream || librdf_stream_end( m_stream ) ) {
        setError( "Invalid iterator" );
        return Statement();
    }

    clearError();

    librdf_statement *st = librdf_stream_get_object( m_stream );
    if ( !st ) {
        // Return a not valid Statement
        // as last value.
        return Soprano::Statement();
    }

    Statement copy = m_model->world()->createStatement( st );
    if ( librdf_node* context = static_cast<librdf_node*>( librdf_stream_get_context( m_stream ) ) ) {
        copy.setContext( m_model->world()->createNode( context ) );
    }
    else if ( m_forceContext.isValid() ) {
        copy.setContext( m_forceContext );
    }

    return copy;
}


void Soprano::Redland::RedlandStatementIterator::close()
{
    clearError();

    if( m_stream ) {
        librdf_free_stream( m_stream );
        m_stream = 0;
    }
    if ( m_model ) {
        m_model->removeIterator( this );
    }
    m_model = 0;
}
