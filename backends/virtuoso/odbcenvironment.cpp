/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2009 Sebastian Trueg <trueg@kde.org>
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

#include "odbcenvironment.h"
#include "odbcenvironment_p.h"
#include "virtuosotools.h"

#include <sql.h>


Soprano::ODBC::Environment::Environment()
    : d( new EnvironmentPrivate(this) )
{
}


Soprano::ODBC::Environment::~Environment()
{
    if ( d->m_henv ) {
        SQLFreeEnv( d->m_henv );
    }
    delete d;
}


HENV Soprano::ODBC::Environment::henv() const
{
    return d->m_henv;
}


// static
Soprano::ODBC::Environment* Soprano::ODBC::Environment::createEnvironment()
{
    HENV henv;

    // allocate sql handle
    if ( SQLAllocEnv( &henv ) != SQL_SUCCESS ) {
        return 0;
    }

    // set odbc version
    SQLSetEnvAttr( henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, SQL_IS_UINTEGER );

    Environment* env = new Environment();
    env->d->m_henv = henv;
    return env;
}
