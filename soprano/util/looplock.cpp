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

#include "looplock.h"

#include <QtCore/QQueue>
#include <QtCore/QEventLoop>


class LoopLock::Private
{
public:
    Private()
        : writeLock( 0 ),
          readLock( 0 ) {
    }

    int writeLock;
    int readLock;

    QQueue<QEventLoop*> writeLoops;
    QQueue<QEventLoop*> readLoops;
};


LoopLock::LoopLock()
    : d( new Private() )
{
}


LoopLock::~LoopLock()
{
    delete d;
}


void LoopLock::lockForWrite()
{
    if ( d->writeLock == 0 && d->readLock == 0 ) {
        ++d->writeLock;
    }
    else {
        // wait for the lock to be released
        QEventLoop loop;
        d->writeLoops.enqueue( &loop );
        loop.exec();
        ++d->writeLock;
    }
}


void LoopLock::lockForRead()
{
    if ( d->writeLock == 0 ) {
        ++d->readLock;
    }
    else {
        // wait for the lock to be released
        QEventLoop loop;
        d->readLoops.enqueue( &loop );
        loop.exec();
        ++d->readLock;
    }
}


void LoopLock::unlock()
{
    if ( d->writeLock ) {
        --d->writeLock;
    }
    else {
        --d->readLock;
    }

    // wake up some loop, prefere write loops
    if ( d->writeLock == 0 &&
         d->readLock == 0 &&
         !d->writeLoops.isEmpty() ) {
        QEventLoop* loop = d->writeLoops.dequeue();
        loop->exit();
    }

    else if ( d->writeLock == 0 &&
              !d->readLoops.isEmpty() ) {
        QEventLoop* loop = d->readLoops.dequeue();
        loop->exit();
    }
}
