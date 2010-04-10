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

#include "error.h"
#include "locator.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QHash>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QThread>


namespace Soprano {
    namespace Error {
        class ErrorData : public QSharedData
        {
        public:
            ErrorData( const QString& m = QString(), int c = ErrorNone )
                : message( m ),
                  code( c ) {
            }

            virtual ~ErrorData() {
            }

            virtual bool isParserError() const {
                return false;
            }

            QString message;
            int code;
        };
    }
}


Soprano::Error::Error::Error()
    : d( 0 )
{
}


Soprano::Error::Error::Error( ErrorData* data )
    : d( data )
{
}


Soprano::Error::Error::Error( const QString& message, int code )
    : d( new ErrorData( message, code ) )
{
    if ( d->message.isEmpty() && code < ErrorUnknown ) {
        d->message = errorMessage( ( ErrorCode )code );
    }
}


Soprano::Error::Error::Error( const Error& other )
{
    d = other.d;
}


Soprano::Error::Error::~Error()
{
}


Soprano::Error::Error& Soprano::Error::Error::operator=( const Error& other )
{
    d = other.d;
    return *this;
}


QString Soprano::Error::Error::message() const
{
    return d ? d->message : QString();
}


int Soprano::Error::Error::code() const
{
    return d ? d->code : ErrorNone;
}


namespace Soprano {
    namespace Error {
        class ParserErrorData : public Soprano::Error::ErrorData
        {
        public:
            ParserErrorData( const Locator& loc = Locator(), const QString& message = QString(), int code = ErrorNone )
                : ErrorData( message, code ),
                  locator( loc ) {
            }

            bool isParserError() const {
                return true;
            }

            Locator locator;
        };
    }
}

bool Soprano::Error::Error::isParserError() const
{
    return d ? d->isParserError() : false;
}


Soprano::Error::ParserError Soprano::Error::Error::toParserError() const
{
    return ParserError( *this );
}


Soprano::Error::ParserError::ParserError()
    : Error( 0 )
{
}


Soprano::Error::ParserError::ParserError( const Locator& loc, const QString& message, int code )
    : Error( new ParserErrorData( loc, message, code ) )
{
}


Soprano::Error::ParserError::ParserError( const Error& other )
    : Error( other )
{
}


Soprano::Error::ParserError::~ParserError()
{
}


Soprano::Error::ParserError& Soprano::Error::ParserError::operator=( const Error& other )
{
    Error::operator=( other );
    return *this;
}


Soprano::Error::Locator Soprano::Error::ParserError::locator() const
{
    if ( isParserError() ) {
        return static_cast<const ParserErrorData*>( d.constData() )->locator;
    }
    else {
        return Locator();
    }
}



// /////////////////////////////////////
// ERROR CACHE
// /////////////////////////////////////


class Soprano::Error::ErrorCache::Private
{
public:
    QHash<QThread*, Error> errorMap;
    QMutex errorMapMutex;
};


Soprano::Error::ErrorCache::ErrorCache()
    : d( new Private() )
{
}


Soprano::Error::ErrorCache::~ErrorCache()
{
    delete d;
}


Soprano::Error::Error Soprano::Error::ErrorCache::lastError() const
{
    QMutexLocker( &d->errorMapMutex );
    return d->errorMap.value( QThread::currentThread() );
}


void Soprano::Error::ErrorCache::setError( const Error& error ) const
{
    if ( error ) {
        QCoreApplication* app = QCoreApplication::instance();
        qDebug() << ( app
#if QT_VERSION >= 0x040400
                      ? QString( "%1(%2)" ).arg( app->applicationFilePath() ).arg( app->applicationPid() )
#else
                      ? app->applicationFilePath()
#endif
                      : QString( "(Soprano)" ) )
                 << "Error in thread" << QThread::currentThreadId() << ":" << error;
        QMutexLocker( &d->errorMapMutex );
        d->errorMap[QThread::currentThread()] = error;
    }
    else {
        clearError();
    }
}


void Soprano::Error::ErrorCache::setError( const QString& errorMessage, int code ) const
{
    setError( Error( errorMessage, code ) );
}


void Soprano::Error::ErrorCache::clearError() const
{
    QMutexLocker( &d->errorMapMutex );
    if ( !d->errorMap.isEmpty() )
        d->errorMap[QThread::currentThread()] = Error();
}


namespace {
    const int s_maxErr = 4;
    const char* s_errorMessages[] = {
        "Success",
        "Invalid argument",
        "Unsupported operation",
        "Parsing failed",
        "Permission denied",
        0
    };
}

QString Soprano::Error::errorMessage( ErrorCode code )
{
    // FIXME: translate the strings.
    if ( code >= 0 && code <= s_maxErr ) {
        return s_errorMessages[(int)code];
    }
    else {
        return "Unknown error";
    }
}


Soprano::Error::ErrorCode Soprano::Error::convertErrorCode( int code )
{
    if ( code >= 0 && code <= s_maxErr ) {
        return ( ErrorCode )code;
    }
    else {
        return ErrorUnknown;
    }
}


QDebug operator<<( QDebug s, const Soprano::Error::Error& error )
{
    if ( error.code() < Soprano::Error::ErrorUnknown ) {
        s.nospace() << QString( "%1 (%2)" ).arg( errorMessage( ( Soprano::Error::ErrorCode )error.code() ) ).arg( error.code() ) << ": ";
    }
    s.nospace() << error.message();

    if ( error.isParserError() ) {
        Soprano::Error::ParserError pe( error );
        s.nospace() << " (line: " << pe.locator().line() << ", column: " << pe.locator().column() << ")";
    }

    return s;
}


QTextStream& operator<<( QTextStream& s, const Soprano::Error::Error& error )
{
    if ( error.code() < Soprano::Error::ErrorUnknown ) {
        s << QString( "%1 (%2)" ).arg( errorMessage( ( Soprano::Error::ErrorCode )error.code() ) ).arg( error.code() ) << ": ";
    }
    s << error.message();

    if ( error.isParserError() ) {
        Soprano::Error::ParserError pe( error );
        s << " (line: " << pe.locator().line() << ", column: " << pe.locator().column() << ")";
    }

    return s;
}
