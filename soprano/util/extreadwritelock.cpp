/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2008 Sebastian Trueg <trueg@kde.org>
 * Based on QReadWriteLock Copyright (C) 1992-2008 Trolltech ASA
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

#include "extreadwritelock.h"

#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtCore/QThread>
#include <QtCore/QHash>


class ExtReadWriteLock::Private
{
public:
    Private()
        : accessCount( 0 ),
          writerWaiting( 0 ),
          readerWaiting( 0 ) {
    }

    QMutex mutex;
    QWaitCondition readerWait;
    QWaitCondition writerWait;

    int accessCount;
    int writerWaiting;
    int readerWaiting;

    QHash<Qt::HANDLE, int> currentReaders;
};


ExtReadWriteLock::ExtReadWriteLock()
    : d( new Private() )
{
}


ExtReadWriteLock::~ExtReadWriteLock()
{
    delete d;
}


void ExtReadWriteLock::lockForWrite()
{
    QMutexLocker lock( &d->mutex );

    while ( d->accessCount != 0 ) {
        ++d->writerWaiting;
        d->writerWait.wait( &d->mutex );
        --d->writerWaiting;
    }

    --d->accessCount;
}


void ExtReadWriteLock::lockForRead()
{
    QMutexLocker lock(&d->mutex);

    Qt::HANDLE self = QThread::currentThreadId();

    QHash<Qt::HANDLE, int>::iterator it = d->currentReaders.find( self );
    if ( it != d->currentReaders.end() ) {
      ++it.value();
      ++d->accessCount;
      return;
    }

    while ( d->accessCount < 0 || d->writerWaiting ) {
        ++d->readerWaiting;
        d->readerWait.wait( &d->mutex );
        --d->readerWaiting;
    }

    d->currentReaders.insert( self, 1 );

    ++d->accessCount;
}


void ExtReadWriteLock::unlock()
{
    QMutexLocker lock(&d->mutex);

    bool unlocked = false;
    if ( d->accessCount > 0 ) {
        // releasing a read lock
        Qt::HANDLE self = QThread::currentThreadId();
        QHash<Qt::HANDLE, int>::iterator it = d->currentReaders.find(self);
        if ( it != d->currentReaders.end() ) {
            if ( --it.value() <= 0 )
                d->currentReaders.erase( it );
        }

        unlocked = --d->accessCount == 0;
    }
    else if ( d->accessCount < 0 && ++d->accessCount == 0 ) {
        // released a write lock
        unlocked = true;
    }

    if ( unlocked ) {
        if ( d->writerWaiting ) {
            d->writerWait.wakeOne();
        } else if ( d->readerWaiting ) {
            d->readerWait.wakeAll();
        }
    }
}
