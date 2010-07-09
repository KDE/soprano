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

#include "soprano_export.h"

#include <QtCore/QString>
#include <QtCore/QSharedDataPointer>


namespace Soprano {
    namespace Error {
        /**
         * %Soprano defines a number of error codes that are
         * used to provide a quick success status check in methods
         * such as Model::addStatement().
         *
         * \sa Error::Error::code(), Error::convertErrorCode
         */
        enum ErrorCode {
            ErrorNone = 0x0,             /**< No error occured, i.e. success. */
            ErrorInvalidArgument = 0x1,  /**< Error indicating that a method argument was invalid. For example an invalid Statement in Model::addStatement(). */
            ErrorInvalidStatement = ErrorInvalidArgument, /**< \deprecated: use ErrorInvalidArgument */
            ErrorNotSupported = 0x2,     /**< Error indicating that a certain functionality is not supported. */
            ErrorParsingFailed = 0x3,    /**< Parsing a query or an RDF serialization failed. */
            ErrorPermissionDenied = 0x4, /**< Permission is denied. \since 2.1 */
            ErrorUnknown = 0x1000        /**< An unknown error occured. */
        };

        /**
         * Translate an error code into a human-readable error message.
         */
        SOPRANO_EXPORT QString errorMessage( ErrorCode );

        /**
         * Converts a plain error code (as for example used in Error::code())
         * into an ErrorCode value.
         *
         * \return \p code converted to ErrorCode. If it is an unknown value,
         * ErrorUnknown will be returned.
         */
        SOPRANO_EXPORT ErrorCode convertErrorCode( int code );

        class ErrorData;
        class ParserError;

        /**
         * \class Error error.h Soprano/Error/Error
         *
         * \brief Represents an error in %Soprano.
         *
         * \author Sebastian Trueg <trueg@kde.org>
         *
         * \sa \ref soprano_error_handling
         */
        class SOPRANO_EXPORT Error
        {
        public:
            /**
             * Create an Error object, representing success, i.e. no error.
             */
            Error();

            /**
             * Create an Error object.
             *
             * \param message A human-readable error message.
             * \param code An optional machine-readable error code. Can be one of Soprano::ErrorCode
             * or a user defined value which has to be bigger than ErrorUnknown. The redland backend
             * for example uses error codes <i>ErrorUnknown + redlandCode</i>.
             */
            Error( const QString& message, int code = ErrorUnknown );

            /**
             * Copy constructor
             */
            Error( const Error& );

            /**
             * Destructor
             */
            virtual ~Error();

            Error& operator=( const Error& );

            /**
             * An %Error evalutes to a boolean, indicating if an %Error is "set".
             *
             * \return \p false if code() == #ErrorNone, \p true otherwise.
             *
             * Thus, an Error object can easily be checked as follows:
             *
             * \code
             * model.addStatement( s );
             * if( model.lastError() ) {
             *    displayError( model.lastError() );
             * }
             * \endcode
             */
            operator bool() const { return code() != ErrorNone; }

            /**
             * A string explaining the error in detail.
             * This string is not necessarily translated (this
             * depends on the plugin implementation).
             *
             * \return An error message describing the error or an empty string
             * for no-error (i.e. success) instances.
             */
            QString message() const;

            /**
             * An error code. If the error code is #ErrorNone the
             * Error instance represents success.
             * The code can either be one of the values of ErrorCode
             * or a value above #ErrorUnknown.
             *
             * Example: The redland backend defines the error code
             * as:
             * \code
             * ErrorUnknown + librdf_log_message_code()
             * \endcode
             *
             * \sa ErrorCode, Error::convertErrorCode
             */
            int code() const;

            /**
             * \return \p true if this Error instance represents a parser error.
             * In that case the error can be converted to a ParserError.
             */
            bool isParserError() const;

            /**
             * Converts this error into a ParserError.
             * This has the same effect as
             * \code
             * Error e;
             * ParserError p1( e );
             * \endcode
             *
             * \return If isParserError() returns true a ParserError
             * with a valid Locator value, otherwise a ParserError with
             * an empty Locator.
             */
            ParserError toParserError() const;

        protected:
            /** \cond protected_error_members */
            Error( ErrorData* );
            QSharedDataPointer<ErrorData> d;
            /** \endcond */
        };

        class Locator;

        /**
         * \class ParserError error.h Soprano/Error/ParserError
         *
         * \brief Represents a parser error in %Soprano.
         *
         * ParserError represents an error during parsing of either a query string (Soprano::Query::Parser::parseQuery())
         * or an RDF serialization (Soprano::Parser::parseStream()).
         * Error and ParserError can be used together and copied without loosing information.
         *
         * The following code is perfectly valid and works:
         *
         * \code
         * ParserError pe( 3, 4 );
         * Error e = pe;
         * ParserError otherPe = e;
         * qDebug() << "Parsing failed at line " << otherPe.line() << " and column " << otherPe.column();
         * \endcode
         *
         * \author Sebastian Trueg <trueg@kde.org>
         *
         * \sa \ref soprano_error_handling
         */
        class SOPRANO_EXPORT ParserError : public Error
        {
        public:
            /**
             * Create an Error object, representing success, i.e. no error.
             */
            ParserError();

            ParserError( const Locator&, const QString& message = QString(), int code = ErrorParsingFailed );

            ParserError( const Error& );

            ~ParserError();

            ParserError& operator=( const Error& );

            Locator locator() const;
        };

        /**
         * \class ErrorCache error.h Soprano/Error/ErrorCache
         *
         * \brief Core class of %Soprano's exception system.
         *
         * The ErrorCache caches Error instances for different threads.
         * Each thread has its own last error. This mechanism tries to
         * replace the missing exceptions for methods that do not return
         * an error code or another value that can state the success of the
         * method's operation.
         *
         * \author Sebastian Trueg <trueg@kde.org>
         *
         * \sa \ref soprano_error_handling
         */
        class SOPRANO_EXPORT ErrorCache
        {
        public:
            virtual ~ErrorCache();

            /**
             * Get the last error that occured in the current thread.
             */
            virtual Error lastError() const;

        protected:
            ErrorCache();

            /**
             * Reset the error for the current thread to no error.
             */
            void clearError() const;

            /**
             * Set the last occured error. This method is const to allow setting
             * of errors in all types of methods. The last error is as such a
             * mutable property.
             */
            void setError( const Error& ) const;

            /**
             * Convenience method to set simple string error messages with a default
             * error code ErrorUnknown.
             */
            void setError( const QString& errorMessage, int code = ErrorUnknown ) const;

        private:
            class Private;
            Private* const d;
        };
    }
}

class QDebug;
class QTextStream;

SOPRANO_EXPORT QDebug operator<<( QDebug s, const Soprano::Error::Error& );
SOPRANO_EXPORT QTextStream& operator<<( QTextStream& s, const Soprano::Error::Error& );

#endif
