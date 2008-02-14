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

#ifndef _EXTREADWRITE_LOCK_H_
#define _EXTREADWRITE_LOCK_H_

/**
 * The only difference to QReadWriteLock is that the same thread may lock
 * for read multiple times even if another write lock is waiting. This is a feature
 * that should make sense in almost every situation.
 *
 * So it is the same as QReadWriteLock from QT 4.4 in recursive mode without write recursion.
 */
class ExtReadWriteLock
{
public:
    ExtReadWriteLock();
    ~ExtReadWriteLock();

    void lockForWrite();
    void lockForRead();

    void unlock();
    
private:
    class Private;
    Private* const d;
};

#endif
