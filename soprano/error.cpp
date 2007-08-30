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

#include "error.h"
#include "locator.h"

#include <QtCore/QHash>
#include <QtCore/QThread>
#include <QtCore/QDebug>


namespace Soprano {
    namespace Error {
        class ErrorData : public QSharedData
        {
        public:
            ErrorData( const QString& m = QString(), int c = ERROR_NONE )
                : message( m ),
                  code( c ) {
            }

            virtual ~ErrorData() {
            }

            QString message;
            int code;
        };
    }
}


Soprano::Error::Error::Error()
    : d( new ErrorData() )
{
}


Soprano::Error::Error::Error( ErrorData* data )
    : d( data )
{
}


Soprano::Error::Error::Error( const QString& message, int code )
    : d( new ErrorData( message,  code ) )
{
    if ( d->message.isEmpty() && code < ERROR_UNKNOWN ) {
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
    return d->message;
}


int Soprano::Error::Error::code() const
{
    return d->code;
}


namespace Soprano {
    namespace Error {
        class ParserErrorData : public Soprano::Error::ErrorData
        {
        public:
            ParserErrorData( const Locator& loc = Locator(), const QString& message = QString(), int code = ERROR_NONE )
                : ErrorData( message, code ),
                  locator( loc ) {
            }

            Locator locator;
        };
    }
}

bool Soprano::Error::Error::isParserError() const
{
    return dynamic_cast<const ParserErrorData*>( d.constData() ) != 0;
}


Soprano::Error::ParserError::ParserError()
    : Error( new ParserErrorData() )
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
    const ParserErrorData* data = dynamic_cast<const ParserErrorData*>( d.constData() );
    if ( data ) {
        return data->locator;
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
    QHash<Qt::HANDLE, Error> errorMap;
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
    return d->errorMap[QThread::currentThreadId()];
}


void Soprano::Error::ErrorCache::setError( const Error& error ) const
{
    if ( error ) {
        qDebug() << "(Soprano) Error occured: " << error;
    }

    d->errorMap[QThread::currentThreadId()] = error;
}


void Soprano::Error::ErrorCache::setError( const QString& errorMessage, int code ) const
{
    setError( Error( errorMessage, code ) );
}


void Soprano::Error::ErrorCache::clearError() const
{
    d->errorMap[QThread::currentThreadId()] = Error();
}


static const char* s_errorMessages[] = {
  "Success",
  "Invalid statement",
  "Unsupported operation",
  "Parsing failed",
  "Unknown error"
};


QString Soprano::Error::errorMessage( ErrorCode code )
{
  // FIXME: translate the strings.
  return s_errorMessages[(int)code];
}


QDebug operator<<( QDebug s, const Soprano::Error::Error& error )
{
    if ( error.code() < Soprano::Error::ERROR_UNKNOWN ) {
        s.nospace() << errorMessage( ( Soprano::Error::ErrorCode )error.code() ) << ": ";
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
    if ( error.code() < Soprano::Error::ERROR_UNKNOWN ) {
        s << errorMessage( ( Soprano::Error::ErrorCode )error.code() ) << ": ";
    }
    s << error.message();

    if ( error.isParserError() ) {
        Soprano::Error::ParserError pe( error );
        s << " (line: " << pe.locator().line() << ", column: " << pe.locator().column() << ")";
    }

    return s;
}
