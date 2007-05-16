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

#include "redlandqueryresult.h"

#include "model.h"
#include "node.h"
#include "statementiterator.h"
#include "redlandutil.h"
#include "redlandstatementiterator.h"
#include "redlandworld.h"
#include "redlandmodel.h"

#include <redland.h>

class Soprano::Redland::RedlandQueryResult::Private
{
public:
    Private()
        : result( 0 ),
          stream( 0 ),
          first( true )
    {}

    librdf_query_results *result;

    librdf_stream* stream;

    QStringList names;
    bool first;

    const RedlandModel* model;
};


Soprano::Redland::RedlandQueryResult::RedlandQueryResult( const RedlandModel* model, librdf_query_results *result )
{
    d = new Private;
    d->model = model;
    d->result = result;

    Q_ASSERT( d->result != 0 );

    const char** names = 0;
    if ( !librdf_query_results_get_bindings( d->result, &names, 0 ) ) {
        for ( ; *names; names++ ) {
            d->names.append( QString::fromUtf8( *names ) );
        }
    }
}


Soprano::Redland::RedlandQueryResult::~RedlandQueryResult()
{
    d->model->removeQueryResult( this );
    delete d;
}


void Soprano::Redland::RedlandQueryResult::close() const
{
    if ( d->result ) {
        librdf_free_query_results( d->result );
        if ( d->stream ) {
            librdf_free_stream( d->stream );
            d->stream = 0;
        }
        d->result = 0;
    }
    d->model = 0;
}


bool Soprano::Redland::RedlandQueryResult::next()
{
    if ( !d->result ) {
        return false;
    }
    else if ( isBool() ) {
        return false;
    }
    else if ( isBinding() ) {
        bool hasNext = librdf_query_results_finished( d->result ) == 0;

        if ( !d->first ) {
            hasNext = ( librdf_query_results_next( d->result ) == 0 );
        }
        else {
            d->first = false;
        }

        return hasNext;
    }
    else if ( isGraph() ) {
        if ( d->first ) {
            d->stream = librdf_query_results_as_stream( d->result );
            d->first = false;
        }

        if ( d->stream ) {
            return librdf_stream_end( d->stream ) == 0;
        }
        else {
            // we cannot reuse a result and it has already been used in model()
            return false;
        }
    }
    return false;
}


Soprano::Statement Soprano::Redland::RedlandQueryResult::currentStatement() const
{
    if ( d->stream ) {
        librdf_statement *st = librdf_stream_get_object( d->stream );

        if ( !st ) {
            return Soprano::Statement();
        }
        else {
            Statement copy = Util::createStatement( st );

            // Move to the next element
            librdf_stream_next( d->stream );

            return copy;
        }
    }
    else {
        return Statement();
    }
}

Soprano::Node Soprano::Redland::RedlandQueryResult::binding( const QString &name ) const
{
    if ( d->result ) {
        librdf_node *node = librdf_query_results_get_binding_value_by_name( d->result, (const char *)name.toLatin1().data() );
        if ( !node ) {
            // Return a not valid node (empty)
            return Soprano::Node();
        }

        Soprano::Node tmp = Util::createNode( node );
        Util::freeNode( node );

        return tmp;
    }
    else {
        return Node();
    }
}


Soprano::Node Soprano::Redland::RedlandQueryResult::binding( int offset ) const
{
    if ( d->result ) {
        librdf_node *node = librdf_query_results_get_binding_value( d->result, offset );
        if ( !node ) {
            // Return a not valid node (empty)
            return Soprano::Node();
        }

        Soprano::Node tmp = Util::createNode( node );
        Util::freeNode( node );

        return tmp;
    }
    else {
        return Node();
    }
}


int Soprano::Redland::RedlandQueryResult::bindingCount() const
{
    // CAUTION: It seems that, at least in Redland 1.0.5, librdf_query_results_get_count is broken
    return d->names.count();
}


QStringList Soprano::Redland::RedlandQueryResult::bindingNames() const
{
    return d->names;
}


bool Soprano::Redland::RedlandQueryResult::isGraph() const
{
    if ( d->result ) {
        return librdf_query_results_is_graph( d->result ) != 0;
    }
    else {
        return false;
    }
}


bool Soprano::Redland::RedlandQueryResult::isBinding() const
{
    if ( d->result ) {
        return librdf_query_results_is_bindings( d->result ) != 0;
    }
    else {
        return false;
    }
}


bool Soprano::Redland::RedlandQueryResult::isBool() const
{
    if ( d->result ) {
        return librdf_query_results_is_boolean( d->result ) != 0;
    }
    else {
        return false;
    }
}


bool Soprano::Redland::RedlandQueryResult::boolValue() const
{
    if ( d->result ) {
        return librdf_query_results_get_boolean( d->result ) > 0;
    }
    else {
        return false;
    }
}


Soprano::Model *Soprano::Redland::RedlandQueryResult::model() const
{
    if ( !isGraph() ) {
        return 0;
    }

    // we cannot reuse a result. :(
    if ( !d->first ) {
        return 0;
    }

    d->first = false;

    // Create a memory model with context support and type hashes
    librdf_storage *storage = librdf_new_storage( World::self()->worldPtr(), "hashes", NULL, "hash-type='memory',contexts='yes'");
    if ( !storage ) {
        return 0;
    }

    librdf_model *model = librdf_new_model( World::self()->worldPtr(), storage, 0 );
    if ( !model ) {
        librdf_free_storage( storage );
        return 0;
    }

    if ( librdf_stream *stream = librdf_query_results_as_stream( d->result ) ) {
        librdf_model_add_statements( model, stream );
        librdf_free_stream( stream );
        return new RedlandModel( model, storage );
    }
    else {
        librdf_free_model( model );
        librdf_free_storage( storage );
        return 0;
    }
}
