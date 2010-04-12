/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2008-2010 Sebastian Trueg <trueg@kde.org>
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


namespace {
    // we have no support for a real default/empty graph, thus we fake one, hoping that nobody will ever use this otherwise
    const char* s_defaultGraph = "sopranofakes:/DEFAULTGRAPH";
    const char* s_openlinkVirtualGraph = "http://www.openlinksw.com/schemas/virtrdf#";
    const char* s_fakeBooleanType = "sopranofakes:/booleanHackUntilVirtuosoProblemIsResolved";
    const char* s_fakeBase64Type = "sopranofakes:/base64BinaryHackUntilVirtuosoProblemIsResolved";

    class VirtuosoUriCache {
    public:
        VirtuosoUriCache()
            : defaultGraph( QUrl::fromEncoded( s_defaultGraph, QUrl::StrictMode ) ),
              openlinkVirtualGraph( QUrl::fromEncoded( s_openlinkVirtualGraph, QUrl::StrictMode ) ),
              fakeBooleanType( QUrl::fromEncoded( s_fakeBooleanType, QUrl::StrictMode ) ),
              fakeBase64BinaryType( QUrl::fromEncoded( s_fakeBase64Type, QUrl::StrictMode ) ) {
        }

        QUrl defaultGraph;
        QUrl openlinkVirtualGraph;
        QUrl fakeBooleanType;
        QUrl fakeBase64BinaryType;
    };
}

Q_GLOBAL_STATIC( VirtuosoUriCache, virtuosoUriCache )


QUrl Soprano::Virtuoso::defaultGraph()
{
    return virtuosoUriCache()->defaultGraph;
}


QUrl Soprano::Virtuoso::openlinkVirtualGraph()
{
    return virtuosoUriCache()->openlinkVirtualGraph;
}


QUrl Soprano::Virtuoso::fakeBooleanType()
{
    return virtuosoUriCache()->fakeBooleanType;
}


QUrl Soprano::Virtuoso::fakeBase64BinaryType()
{
    return virtuosoUriCache()->fakeBase64BinaryType;
}


const char* Soprano::Virtuoso::defaultGraphString()
{
    return s_defaultGraph;
}


const char* Soprano::Virtuoso::openlinkVirtualGraphString()
{
    return s_openlinkVirtualGraph;
}


const char* Soprano::Virtuoso::fakeBooleanTypeString()
{
    return s_fakeBooleanType;
}


const char* Soprano::Virtuoso::fakeBase64BinaryTypeString()
{
    return s_fakeBase64Type;
}
