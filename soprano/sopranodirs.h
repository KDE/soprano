/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2009 Sebastian Trueg <trueg@kde.org>
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

#ifndef _SOPRANO_DIRS_H_
#define _SOPRANO_DIRS_H_

#include <QtCore/QStringList>

namespace Soprano {
    QStringList envDirList( const char* var );

    /**
     * lib dirs without lib suffix or trailing slash.
     */
    QStringList libDirs();
    QStringList dataDirs();
    QStringList exeDirs();

    /**
     * Find a library named \p libName.
     *
     * Search the standard library paths (including the Qt paths) and the
     * paths optionally specified in \p extraDirs. Optionally search
     * \p subDirs, too. The \p subDirs will be appended to the standard
     * searchpaths and \p extraDirs.
     *
     * \return The path to the library file or an emtpy string if no lib
     * could be found.
     */
    QString findLibraryPath( const QString& libName,
                             const QStringList& extraDirs = QStringList(),
                             const QStringList& subDirs = QStringList() );
}

#endif
