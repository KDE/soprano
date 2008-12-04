/*
 * This file is part of Soprano Project
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

#ifndef _SOPRANO_IODBC_TOOLS_H_
#define _SOPRANO_IODBC_TOOLS_H_

#include <iodbc/sql.h>
#include <iodbc/sqlext.h>
#include <iodbc/sqlucode.h>
#include <iodbc/iodbcext.h>

#include "error.h"

#define TEXT(x)   	(SQLWCHAR *) L##x

namespace Soprano {
    namespace IODBC {
        Soprano::Error::Error convertSqlError( SQLSMALLINT handleType, SQLHANDLE handle );
    }
}

#endif