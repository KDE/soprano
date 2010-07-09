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

#include "sesame2statementiteratorbackend.h"
#include "sesame2iterator.h"
#include "sesame2utils.h"
#include "sesame2model.h"
#include "jniwrapper.h"

#include "statement.h"

#include <QtCore/QDebug>


class Soprano::Sesame2::StatementIteratorBackend::Private
{
public:
    Private( const JObjectRef& result_ )
        : result( result_ ) {
    }

    Iterator result;

    Statement current;

    const Model* model;
};


Soprano::Sesame2::StatementIteratorBackend::StatementIteratorBackend( const JObjectRef& result, const Model* model )
    : d( new Private( result ) )
{
    d->model = model;
}


Soprano::Sesame2::StatementIteratorBackend::~StatementIteratorBackend()
{
    close();
    delete d;
}


bool Soprano::Sesame2::StatementIteratorBackend::next()
{
    if ( d->result.hasNext() ) {
        JObjectRef next = d->result.next();
        if ( next ) {
            clearError();
            d->current = convertStatement( next );
            return true;
        }
    }

    // if there is an exception d->result returns false and it
    // is sufficient to catch it once here
    setError( JNIWrapper::instance()->convertAndClearException() );
    return false;
}


Soprano::Statement Soprano::Sesame2::StatementIteratorBackend::current() const
{
    return d->current;
}


void Soprano::Sesame2::StatementIteratorBackend::close()
{
    if ( d->model ) {
        d->result.close();
        setError( JNIWrapper::instance()->convertAndClearException() );
        d->model->removeIterator( this );
        d->model = 0;
    }
}
