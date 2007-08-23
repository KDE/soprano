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
#include "jniwrapper.h"

#include <soprano/statement.h>
#include <soprano/node.h>

#include <QtCore/QDebug>
#include <QtCore/QStringList>


class Soprano::Sesame2::QueryResultIteratorBackend::Private
{
public:
    Private( jobject result_ )
        : result( result_ ) {
        tupleResult = JNIWrapper::instance()->env()->IsInstanceOf( result_,
                                                                   JNIWrapper::instance()->env()->FindClass( ORG_OPENRDF_QUERY_TUPLEQUERYRESULT ) );

        // cache the binding names, it is just simpler
        if ( tupleResult ) {
            jobject bindingList = result.callObjectMethod( result.getMethodID( "getBindingNames", "()L"JAVA_UTIL_LIST";" ) );
            JNIObjectWrapper listWrapper( bindingList );
            Iterator it( listWrapper.callObjectMethod( listWrapper.getMethodID( "iterator", "()L"JAVA_UTIL_ITERATOR";" ) ) );
            while ( it.hasNext() ) {
                bindingNames.append( JNIWrapper::instance()->convertString( reinterpret_cast<jstring>( it.next() ) ) );
            }
        }
    }

    Iterator result;

    bool tupleResult;

    Statement currentStatement;
    BindingSet currentBindings;

    QStringList bindingNames;
};


Soprano::Sesame2::QueryResultIteratorBackend::QueryResultIteratorBackend( jobject result )
    : d( new Private( result ) )
{
}


Soprano::Sesame2::QueryResultIteratorBackend::~QueryResultIteratorBackend()
{
    delete d;
}


bool Soprano::Sesame2::QueryResultIteratorBackend::next()
{
    if ( d->result.hasNext() ) {
        if ( d->tupleResult ) {
            d->currentBindings.setObject( d->result.next() );
        }
        else {
            d->currentStatement = convertStatement( d->result.next() );
        }

        return true;
    }
    else {
        return false;
    }
}


Soprano::Statement Soprano::Sesame2::QueryResultIteratorBackend::currentStatement() const
{
    return d->currentStatement;
}


Soprano::Node Soprano::Sesame2::QueryResultIteratorBackend::binding( const QString &name ) const
{
    // make sure we do not crash
    if ( d->currentBindings.object() ) {
        return convertNode( d->currentBindings.getValue( JNIWrapper::instance()->convertString( name ) ) );
    }
    else {
        qDebug() << "(Soprano::Sesame2::QueryResultIteratorBackend) called binding on an invalid iterator.";
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
    return !d->tupleResult;
}


bool Soprano::Sesame2::QueryResultIteratorBackend::isBinding() const
{
    return d->tupleResult;
}


bool Soprano::Sesame2::QueryResultIteratorBackend::isBool() const
{
    // FIXME: how does sesame handle ask queries?
    return false;
}


bool Soprano::Sesame2::QueryResultIteratorBackend::boolValue() const
{
    return false;
}


void Soprano::Sesame2::QueryResultIteratorBackend::close()
{
    d->result.close();
}
