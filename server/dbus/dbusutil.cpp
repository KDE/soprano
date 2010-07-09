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

#include "dbusutil.h"

#include <QtDBus/QtDBus>
#include <QtCore/QMutex>

#include "soprano/error.h"
#include "soprano/locator.h"


void Soprano::DBus::sendErrorReply( const QDBusMessage& m, const Soprano::Error::Error& e )
{
    // we use an encoded error name to get all our error info through
    QString errorName, errorMessage;
    if ( e.isParserError() ) {
        errorName = "org.soprano.ParserError";
        errorMessage = QString( "%1l%2c%3/%4" )
                       .arg( e.code() )
                       .arg( e.toParserError().locator().line() )
                       .arg( e.toParserError().locator().column() )
                       .arg( e.message() );
    }
    else {
        errorName = "org.soprano.Error";
        errorMessage = QString( "%1/%2" )
                       .arg( e.code() )
                       .arg( e.message() );
    }

    m.setDelayedReply( true );
    QDBusConnection::sessionBus().send( m.createErrorReply( errorName, errorMessage ) );
}


Soprano::Error::Error Soprano::DBus::convertError( const QDBusError& e )
{
    if ( e.isValid() ) {
        QString errorName = e.name();
        QString errorMessage = e.message();

        if ( errorName == "org.soprano.ParserError" ) {
            int l = errorMessage.indexOf( 'l', 1 );
            int c = errorMessage.indexOf( 'c', l );
            int m = errorMessage.indexOf( '/', c );
            return Error::ParserError( Error::Locator( errorMessage.mid( l+1, c-l-1 ).toInt(),
                                                       errorMessage.mid( c+1, m-c-1 ).toInt() ),
                                       errorMessage.mid( m+1 ),
                                       errorMessage.mid( 0, l ).toInt() );
        }
        else if ( errorName == "org.soprano.Error" ) {
            int m = errorMessage.indexOf( '/', 1 );
            return Error::Error( errorMessage.mid( m+1 ),
                                 errorMessage.mid( 0, m ).toInt() );
        }
        else {
            return Error::Error( errorName + " - " + errorMessage );
        }
    }
    else {
        return Error::Error();
    }
}
