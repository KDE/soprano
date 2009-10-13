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

#include "virtuosotools.h"

#include <QtCore/QString>

// we have no support for a real default/empty graph, thus we fake one, hoping that nobody will ever use this otherwise
static const char* s_defaultGraph = "sopranofakes:/DEFAULTGRAPH";
static const char* s_openlinkVirtualGraph = "http://www.openlinksw.com/schemas/virtrdf#";


Soprano::Error::Error Soprano::Virtuoso::convertSqlError( SQLSMALLINT handleType, SQLHANDLE handle, const QString& extraMessage )
{
    SQLTCHAR buf[513];
    buf[512] = 0;
    SQLTCHAR sqlstate[15];
    SQLINTEGER nativeError = 0;
    SQLSMALLINT len = 0;

    // find last error
    QString msg;
    int i = 0;
    while ( SQL_SUCCEEDED( SQLGetDiagRec( handleType,
                                          handle,
                                          ++i,
                                          sqlstate,
                                          &nativeError,
                                          buf,
                                          512,
                                          &len ) ) ) {
        msg = QLatin1String( "iODBC Error: " ) + QString::fromLatin1( ( const char* )buf, len );
    }

    if ( msg.isEmpty() ) {
        return msg = "Failed to retrieve error information from iODBC";
    }

    if ( !extraMessage.isEmpty() ) {
        msg = extraMessage + QLatin1String( " (" ) + msg + ')';
    }

    return Error::Error( msg );
}


QUrl Soprano::Virtuoso::defaultGraph()
{
    return QUrl::fromEncoded( s_defaultGraph, QUrl::StrictMode );
}


QUrl Soprano::Virtuoso::openlinkVirtualGraph()
{
    return QUrl::fromEncoded( s_openlinkVirtualGraph, QUrl::StrictMode );
}


QUrl Soprano::Virtuoso::fakeBooleanType()
{
    return QUrl::fromEncoded( "sopranofakes:/booleanHackUntilVirtuosoProblemIsResolved", QUrl::StrictMode );
}


QUrl Soprano::Virtuoso::fakeBase64BinaryType()
{
    return QUrl::fromEncoded( "sopranofakes:/base64BinaryHackUntilVirtuosoProblemIsResolved", QUrl::StrictMode );
}
