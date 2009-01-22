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
 * You should have received a copy of the GNU Library General Public
 * License along with this library; see the file COPYING.LIB.  If not,
 * write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301, USA.
 */

#include "multimutex.h"

#include <QtCore/QMutex>
#include <QtCore/QReadWriteLock>
#include <QtCore/QThread>


class MultiMutex::Private
{
public:
    Private()
        : lockingThread( 0 ),
          lockCnt( 0 ),
          lockedForWrite( false ) {
    }

    QReadWriteLock mainMutex;
    QMutex lockMutex; // protects modification of fields below
    QMutex readMutex; // makes sure only one thread can lock for read

    QThread* lockingThread;
    int lockCnt;
    bool lockedForWrite;
};

MultiMutex::MultiMutex()
    : d( new Private() )
{
}


MultiMutex::~MultiMutex()
{
    delete d;
}


void MultiMutex::lockForRead()
{
    d->lockMutex.lock();
    if ( d->lockingThread == QThread::currentThread() ) {
        ++d->lockCnt;
        d->lockMutex.unlock();
    }
    else {
        d->lockMutex.unlock();
        d->readMutex.lock();
        d->mainMutex.lockForRead();
        d->lockingThread = QThread::currentThread();
        ++d->lockCnt;
    }
}


void MultiMutex::lockForWrite()
{
    d->mainMutex.lockForWrite();
    d->lockedForWrite = true;
}


void MultiMutex::unlock()
{
    d->lockMutex.lock();
    if ( d->lockedForWrite ) {
        d->lockedForWrite = false;
        d->mainMutex.unlock();
    }

    else if ( d->lockingThread == QThread::currentThread() ) {
        if ( --d->lockCnt == 0 ) {
            d->lockingThread = 0;
            d->mainMutex.unlock();
            d->readMutex.unlock();
        }
    }
    d->lockMutex.unlock();
}


MultiMutexWriteLocker::MultiMutexWriteLocker( MultiMutex* m )
    : m_mutex( m )
{
    m->lockForWrite();
}


MultiMutexWriteLocker::~MultiMutexWriteLocker()
{
    m_mutex->unlock();
}


MultiMutexReadLocker::MultiMutexReadLocker( MultiMutex* m )
    : m_mutex( m )
{
    m->lockForRead();
}


MultiMutexReadLocker::~MultiMutexReadLocker()
{
    m_mutex->unlock();
}

