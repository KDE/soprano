/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2008 Sebastian Trueg <trueg@kde.org>
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

#include "transaction.h"

class Soprano::Transaction::Private
{
public:
    Model* model;
    bool finished;
};


Soprano::Transaction::Transaction( Model* model )
    : Model(),
      d( new Private() )
{
    d->model = model;
    d->finished = false;
}


Soprano::Transaction::~Transaction()
{
    delete d;
}


Soprano::Model* Soprano::Transaction::model() const
{
    return d->model;
}

bool Soprano::Transaction::isValid() const
{
    return !d->finished;
}


Soprano::Error::ErrorCode Soprano::Transaction::rollback()
{
    if( isValid() ) {
        return doRollback();
    }
    else {
        setError( "Cannot rollback a finished transaction.", Error::ErrorInvalidOperation );
        return Error::ErrorInvalidOperation;
    }
}


Soprano::Error::ErrorCode Soprano::Transaction::commit()
{
    if( isValid() ) {
        d->finished = true;
        return doCommit();
    }
    else {
        setError( "Cannot commit a finished transaction.", Error::ErrorInvalidOperation );
        return Error::ErrorInvalidOperation;
    }
}
