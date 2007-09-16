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

#ifndef _SOPRANO_VERSION_H_
#define _SOPRANO_VERSION_H_

#include "soprano_export.h"

/// @brief Soprano version as string at compile time.
#define SOPRANO_VERSION_STRING "${CMAKE_SOPRANO_VERSION_STRING}"

/// @brief The major Soprano version number at compile time
#define SOPRANO_VERSION_MAJOR ${CMAKE_SOPRANO_VERSION_MAJOR}

/// @brief The minor Soprano version number at compile time
#define SOPRANO_VERSION_MINOR ${CMAKE_SOPRANO_VERSION_MINOR}

/// @brief The Soprano release version number at compile time
#define SOPRANO_VERSION_RELEASE ${CMAKE_SOPRANO_VERSION_RELEASE}

namespace Soprano {
    /**
     * @brief Returns the major number of Soprano's version, e.g.
     * 1 for %Soprano 1.0.2. 
     * @return the major version number at runtime.
     */
    SOPRANO_EXPORT unsigned int versionMajor();

    /**
     * @brief Returns the minor number of Soprano's version, e.g.
     * 0 for %Soprano 1.0.2. 
     * @return the minor version number at runtime.
     */
    SOPRANO_EXPORT unsigned int versionMinor();

    /**
     * @brief Returns the release of Soprano's version, e.g.
     * 2 for %Soprano 1.0.2. 
     * @return the release number at runtime.
     */
    SOPRANO_EXPORT unsigned int versionRelease();

    /**
     * @brief Returns the %Soprano version as string, e.g. "1.0.2".
     *
     * On contrary to the macro SOPRANO_VERSION_STRING this function returns
     * the version number of Soprano at runtime.
     * @return the %Soprano version. You can keep the string forever
     */
    SOPRANO_EXPORT const char* versionString();
}

#endif
