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

#ifndef _SOPRANO_LOCK_FILE_H_
#define _SOPRANO_LOCK_FILE_H_

#include <QtCore/QString>

class LockFile
{
public:
    LockFile();
    LockFile( const QString& path );
    ~LockFile();

    void setFileName( const QString& path );
    QString fileName() const;

    bool aquireLock();
    void releaseLock();

private:
    class Private;
    Private* const d;
};

#endif
