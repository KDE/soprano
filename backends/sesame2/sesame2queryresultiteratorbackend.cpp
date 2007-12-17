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

#include "sesame2queryresultiteratorbackend.h"
#include "sesame2iterator.h"
#include "sesame2utils.h"
#include "sesame2types.h"
#include "sesame2bindingset.h"
#include "sesame2model.h"
#include "jniwrapper.h"

#include <soprano/statement.h>
#include <soprano/node.h>

#include <QtCore/QDebug>
#include <QtCore/QStringList>


class Soprano::Sesame2::QueryResultIteratorBackend::Private
{
public:
    Private( bool result )
        : result( 0 ),
          booleanResult( result ),
          isTupleResult( false ),
          isBooleanResult( true ) {
    }

    Private( const JObjectRef& result_ )
        : result( new Iterator( result_ ) ),
          booleanResult( false ),
          isBooleanResult( false ) {
        isTupleResult = JNIWrapper::instance()->env()->IsInstanceOf( result_,
                                                                     JNIWrapper::instance()->env()->FindClass( ORG_OPENRDF_QUERY_TUPLEQUERYRESULT ) );

        // cache the binding names, it is just simpler
        if ( isTupleResult ) {
            jobject bindingList = result->callObjectMethod( result->getMethodID( "getBindingNames", "()L"JAVA_UTIL_LIST";" ) );
            JNIObjectWrapper listWrapper( bindingList );
            Iterator it( listWrapper.callObjectMethod( listWrapper.getMethodID( "iterator", "()L"JAVA_UTIL_ITERATOR";" ) ) );
            while ( it.hasNext() ) {
                bindingNames.append( JStringRef( it.next() ).toQString() );
            }
        }
    }

    Iterator* result;
    bool booleanResult;

    bool isTupleResult;
    bool isBooleanResult;

    Statement currentStatement;
    BindingSet currentBindings;

    QStringList bindingNames;

    const Model* model;
};


Soprano::Sesame2::QueryResultIteratorBackend::QueryResultIteratorBackend( const JObjectRef& result, const Model* model )
    : d( new Private( result ) )
{
    d->model = model;
}


Soprano::Sesame2::QueryResultIteratorBackend::QueryResultIteratorBackend( bool result, const Model* model )
    : d( new Private( result ) )
{
    d->model = model;
}


Soprano::Sesame2::QueryResultIteratorBackend::~QueryResultIteratorBackend()
{
    close();
    delete d->result;
    delete d;
}


bool Soprano::Sesame2::QueryResultIteratorBackend::next()
{
    if ( d->isBooleanResult ) {
        return d->model != 0;
    }

    if ( d->result->hasNext() ) {
        JObjectRef next = d->result->next();
        if ( next ) {
            if ( d->isTupleResult ) {
                d->currentBindings.setObject( next );
            }
            else {
                d->currentStatement = convertStatement( next );
            }

            return true;
        }
    }

    // if there is an exception d->result returns false and it
    // is sufficient to catch it once here
    setError( JNIWrapper::instance()->convertAndClearException() );
    return false;
}


Soprano::Statement Soprano::Sesame2::QueryResultIteratorBackend::currentStatement() const
{
    return d->currentStatement;
}


Soprano::Node Soprano::Sesame2::QueryResultIteratorBackend::binding( const QString &name ) const
{
    // make sure we do not crash
    if ( d->currentBindings.object() ) {
        JObjectRef node = d->currentBindings.getValue( JStringRef( name ) );
        setError( JNIWrapper::instance()->convertAndClearException() );
        return convertNode( node );
    }
    else {
        setError( "Invalid iterator" );
        return Node();
    }
}


Soprano::Node Soprano::Sesame2::QueryResultIteratorBackend::binding( int offset ) const
{
    return binding( d->bindingNames[offset] );
}


int Soprano::Sesame2::QueryResultIteratorBackend::bindingCount() const
{
    return d->bindingNames.count();
}


QStringList Soprano::Sesame2::QueryResultIteratorBackend::bindingNames() const
{
    return d->bindingNames;
}


bool Soprano::Sesame2::QueryResultIteratorBackend::isGraph() const
{
    return !d->isTupleResult && !d->isBooleanResult;
}


bool Soprano::Sesame2::QueryResultIteratorBackend::isBinding() const
{
    return d->isTupleResult;
}


bool Soprano::Sesame2::QueryResultIteratorBackend::isBool() const
{
    return d->isBooleanResult;
}


bool Soprano::Sesame2::QueryResultIteratorBackend::boolValue() const
{
    return d->booleanResult;
}


void Soprano::Sesame2::QueryResultIteratorBackend::close()
{
    if ( d->model ) {
        if ( d->result ) {
            d->result->close();
        }
        setError( JNIWrapper::instance()->convertAndClearException() );
        d->model->removeIterator( this );
        d->model = 0;
    }
}
