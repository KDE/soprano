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

#include "iodbctools.h"

#include <QtCore/QString>

// we have no support for a real default/empty graph, thus we fake one, hoping that nobody will ever use this otherwise
static const char* s_defaultGraph = "sopranofakes:/DEFAULTGRAPH";
static const char* s_openlinkVirtualGraph = "http://www.openlinksw.com/schemas/virtrdf#";


Soprano::Error::Error Soprano::IODBC::convertSqlError( SQLSMALLINT handleType, SQLHANDLE handle )
{
    SQLTCHAR buf[513];
    buf[512] = 0;
    SQLTCHAR sqlstate[15];
    SQLINTEGER nativeError = 0;
    SQLSMALLINT len = 0;

    // find last error
    Error::Error err;
    int i = 0;
    while ( SQL_SUCCEEDED( SQLGetDiagRec( handleType,
                                          handle,
                                          ++i,
                                          sqlstate,
                                          &nativeError,
                                          buf,
                                          512,
                                          &len ) ) ) {
        err = Soprano::Error::Error( "iODBC Error: " + QString::fromLatin1( ( const char* )buf, len ) );
    }

    if ( !err ) {
        return Soprano::Error::Error( "Failed to retrieve error information from iODBC" );
    }
    else {
        return err;
    }
}


QUrl Soprano::IODBC::defaultGraph()
{
    return QUrl::fromEncoded( s_defaultGraph, QUrl::StrictMode );
}


QUrl Soprano::IODBC::openlinkVirtualGraph()
{
    return QUrl::fromEncoded( s_openlinkVirtualGraph, QUrl::StrictMode );
}


QUrl Soprano::IODBC::fakeDateTimeType()
{
    return QUrl::fromEncoded( "sopranofakes:/dateTimeHackUntilVirtuosoProblemIsResolved", QUrl::StrictMode );
}


QUrl Soprano::IODBC::fakeTimeType()
{
    return QUrl::fromEncoded( "sopranofakes:/timeHackUntilVirtuosoProblemIsResolved", QUrl::StrictMode );
}


QUrl Soprano::IODBC::fakeBooleanType()
{
    return QUrl::fromEncoded( "sopranofakes:/booleanHackUntilVirtuosoProblemIsResolved", QUrl::StrictMode );
}
