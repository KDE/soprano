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

#ifndef _MUTEXLOCKER_H_
#define _MUTEXLOCKER_H_

class QMutex;

/**
 * The MutexLocker locks a QMutex instance until the locker
 * is deleted.
 *
 * This is very useful in methods that need to block a mutex 
 * and that have multiple return statements.
 */
class MutexLocker
{
public:
    MutexLocker( QMutex* );
    ~MutexLocker();

private:
    class Private;
    Private* d;
};

#endif
