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

#ifndef _MULTI_MUTEX_H_
#define _MULTI_MUTEX_H_

/**
 * A MultiMutex is a mutex that can be locked by the same thread
 * an arbitrary number of times for reading. Once the same thread called 
 * unlock() the same number of times it called lock() the mutex
 * is released for real.
 *
 * Writing is only allowed once.
 *
 * \sa QReadWriteLock
 */
class MultiMutex
{
public:
    MultiMutex();
    ~MultiMutex();

    void lockForRead();
    void lockForWrite();
    void unlock();

private:
    class Private;
    Private* const d;
};

class MultiMutexWriteLocker
{
public:
    MultiMutexWriteLocker( MultiMutex* );
    ~MultiMutexWriteLocker();

private:
    MultiMutex* m_mutex;
};

class MultiMutexReadLocker
{
public:
    MultiMutexReadLocker( MultiMutex* );
    ~MultiMutexReadLocker();

private:
    MultiMutex* m_mutex;
};

#endif
