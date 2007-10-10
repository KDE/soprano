/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */

#include "lockfile.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include <QtCore/QFile>
#include <QtCore/QDebug>


class LockFile::Private
{
public:
    Private()
        : fd( -1 ) {
    }

    QString path;
    int fd;
};


LockFile::LockFile()
    : d( new Private() )
{
}


LockFile::LockFile( const QString& path )
    : d( new Private() )
{
    d->path = path;
}


LockFile::~LockFile()
{
    releaseLock();
    delete d;
}


void LockFile::setFileName( const QString& path )
{
    releaseLock();
    d->path = path;
}


QString LockFile::fileName() const
{
    return d->path;
}


bool LockFile::aquireLock()
{
    releaseLock();

    d->fd = open( QFile::encodeName( d->path ).data(), O_WRONLY|O_CREAT );
    if ( d->fd == -1 ) {
        qDebug() << "(LockFile) could not open" << d->path;
        return false;
    }
#ifndef _WIN32
// flock isn't defined under windows
    struct flock mlock;
    mlock.l_type = F_WRLCK;
    mlock.l_whence = SEEK_SET;
    mlock.l_start = 0;
    mlock.l_len = 0;
    int r = fcntl( d->fd, F_SETLK, &mlock );
    if ( r == -1 ) {
        qDebug() << "(LockFile) could not set lock for" << d->path;
        close( d->fd );
        return false;
    }
#endif /* _WIN32 */
    return true;
}


void LockFile::releaseLock()
{
    if ( d->fd > 0 ) {
        close( d->fd );
    }
    d->fd = -1;
}
