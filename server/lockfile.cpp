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

#ifndef _WIN32_WCE
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include <string.h>
#include <errno.h>
#else
#include <Winbase.h>
#include <atlbase.h>
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


bool LockFile::aquireLock( int* owningPid )
{
    releaseLock();

    // make sure we have write permission to the file
    if ( QFile::exists( d->path ) ) {
        QFile f( d->path );
        f.setPermissions( f.permissions() | QFile::WriteOwner );
    }

#ifndef _WIN32_WCE
    d->fd = open( QFile::encodeName( d->path ).data(), O_WRONLY|O_CREAT, 0600 );
#else
    CA2W wpath( QFile::encodeName( d->path ).data() );
    d->fd = reinterpret_cast<int>(CreateFile( wpath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ));
#endif
    if ( d->fd == -1 ) {
#ifndef _WIN32_WCE
        qDebug() << "(LockFile) could not open" << d->path << QString( "(%1)" ).arg( strerror( errno ) );
#else
        char *msg;

        FormatMessageW (FORMAT_MESSAGE_ALLOCATE_BUFFER |
                      FORMAT_MESSAGE_IGNORE_INSERTS |
                      FORMAT_MESSAGE_FROM_SYSTEM,
                      NULL, GetLastError(), 0,
                      (LPWSTR) &msg, 0, NULL);
                      
        qDebug() << "(LockFile) could not open" << d->path << QString( "(%1)" ).arg( msg );
#endif
        return false;
    }
    // flock isn't defined under windows
#if defined (F_SETLK)
    struct flock mlock;
    mlock.l_type = F_WRLCK;
    mlock.l_whence = SEEK_SET;
    mlock.l_start = 0;
    mlock.l_len = 0;
    int r = fcntl( d->fd, F_SETLK, &mlock );
    if ( r == -1 ) {
        qDebug() << "(LockFile) could not set lock for" << d->path;
        if ( owningPid ) {
            // we could not get a lock, so who owns it?
            fcntl( d->fd, F_GETLK, &mlock );
            *owningPid = mlock.l_pid;
        }
        close( d->fd );
        return false;
    }
#endif
    return true;
}


void LockFile::releaseLock()
{
    if ( d->fd > 0 ) {
#ifndef _WIN32_WCE
        close( d->fd );
#else
        CloseHandle( reinterpret_cast<HANDLE>(d->fd ));
#endif
    }
    d->fd = -1;
}
