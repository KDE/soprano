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

#ifndef _SOPRANO_ERROR_H_
#define _SOPRANO_ERROR_H_

#include <soprano/soprano_export.h>

#include <QString>

namespace Soprano {
  enum ErrorCode {
    ERROR_NONE = 0,
    ERROR_INVALID_STATEMENT,
    ERROR_UNKNOW
  };

  /**
   * Translate an error code into a human-readable error message.
   */
  SOPRANO_EXPORT QString errorMessage( ErrorCode );
}

#endif
