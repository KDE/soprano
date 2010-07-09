/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2008 Sebastian Trueg <strueg@mandriva.com>
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

#include "asyncresultwaiter.h"
#include "asyncmodel.h"

#include <QtCore/QEventLoop>
#include <QtCore/QVariant>


class Soprano::Util::AsyncResultWaiter::Private
{
public:
    QEventLoop loop;
    QVariant value;
};


Soprano::Util::AsyncResultWaiter::AsyncResultWaiter()
    : d( new Private() )
{
}


Soprano::Util::AsyncResultWaiter::~AsyncResultWaiter()
{
    delete d;
}


void Soprano::Util::AsyncResultWaiter::slotResultReady( AsyncResult* result )
{
    d->value = result->value();
    d->loop.exit();
}


QVariant Soprano::Util::AsyncResultWaiter::waitForResult( AsyncResult* result )
{
    AsyncResultWaiter waiter;
    connect( result, SIGNAL( resultReady( Soprano::Util::AsyncResult* ) ),
             &waiter, SLOT( slotResultReady( Soprano::Util::AsyncResult* ) ) );
    waiter.d->loop.exec();
    return waiter.d->value;
}

#include "asyncresultwaiter.moc"
