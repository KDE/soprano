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

#include "redlandnodeiteratorbackend.h"
#include "redlandworld.h"

#include "node.h"
#include "redlandmodel.h"

#include <QtCore/QtGlobal>
#include <QtCore/QSharedData>
#include <QtCore/QDebug>


Soprano::Redland::NodeIteratorBackend::NodeIteratorBackend( const RedlandModel* model, librdf_iterator* it )
    : m_model( model ),
      m_iterator( it ),
      m_initialized( false )
{
}


Soprano::Redland::NodeIteratorBackend::~NodeIteratorBackend()
{
    close();
}


bool Soprano::Redland::NodeIteratorBackend::next()
{
    if ( m_initialized ) {
        // Move to the next element
        librdf_iterator_next( m_iterator );
    }

    m_initialized = true;

    if ( m_iterator ) {
        if ( librdf_iterator_end( m_iterator ) ) {
            close();
            return false;
        }
        else {
            return true;
        }
    }
    else {
        return false;
    }
}


Soprano::Node Soprano::Redland::NodeIteratorBackend::current() const
{
    if ( !m_iterator || librdf_iterator_end( m_iterator ) ) {
        return Node();
    }

    librdf_node* ctx = (librdf_node *)librdf_iterator_get_object( m_iterator );
    if ( !ctx ) {
        // Return a not valid Node
        // as last value.
        return Soprano::Node();
    }

    return m_model->world()->createNode( ctx );
}


void Soprano::Redland::NodeIteratorBackend::close()
{
    if( m_iterator ) {
        librdf_free_iterator( m_iterator );
        m_iterator = 0;
    }
    if ( m_model ) {
        m_model->removeIterator( this );
    }
    m_model = 0;
}
