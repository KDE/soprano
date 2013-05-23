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

#include "signalcachemodel.h"

#include <QtCore/QBasicTimer>
#include <QtCore/QTimerEvent>


class Soprano::Util::SignalCacheModel::Private
{
public:
    QBasicTimer addTimer;
    QBasicTimer removeTimer;

    int cacheTime;
};


Soprano::Util::SignalCacheModel::SignalCacheModel( Model* parent )
    : FilterModel( parent ),
      d( new Private() )
{
    d->cacheTime = 50;
}


Soprano::Util::SignalCacheModel::~SignalCacheModel()
{
    delete d;
}


int Soprano::Util::SignalCacheModel::cacheTime() const
{
    return d->cacheTime;
}


void Soprano::Util::SignalCacheModel::setCacheTime( int msec )
{
    d->cacheTime = qMax( 0, msec );
}


void Soprano::Util::SignalCacheModel::parentStatementsAdded()
{
    if ( !d->addTimer.isActive() ) {
        d->addTimer.start( d->cacheTime, this );
    }
}


void Soprano::Util::SignalCacheModel::parentStatementsRemoved()
{
    if ( !d->removeTimer.isActive() ) {
        d->removeTimer.start( d->cacheTime, this );
    }
}

void Soprano::Util::SignalCacheModel::timerEvent( QTimerEvent* event )
{
    if ( event->timerId() == d->addTimer.timerId() ) {
        emit statementsAdded();
        d->addTimer.stop();
    }
    else if ( event->timerId() == d->removeTimer.timerId() ) {
        emit statementsRemoved();
        d->removeTimer.stop();
    }
    else {
        FilterModel::timerEvent( event );
    }
}

