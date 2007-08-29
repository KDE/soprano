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

#include <QtCore/QString>
#include <QtCore/QSharedDataPointer>


namespace Soprano {
    namespace Error {
	enum ErrorCode {
	    ERROR_NONE = 0,          /**< No error occured, i.e. success. */
	    ERROR_INVALID_ARGUMENT,  /**< Error indicating that a method argument was invalid. For example an invalid Statement in Model::addStatement(). */
	    ERROR_INVALID_STATEMENT = ERROR_INVALID_ARGUMENT, /**< \deprecated: use ERROR_INVALID_ARGUMENT */
	    ERROR_NOT_SUPPORTED,     /**< Error indicating that a certain functionality is not supported. */
	    ERROR_PARSING_FAILED,    /**< Parsing a query or an RDF serialization failed. */
	    ERROR_UNKNOWN            /**< An unknown error occured. */
	};

	/**
	 * Translate an error code into a human-readable error message.
	 */
	SOPRANO_EXPORT QString errorMessage( ErrorCode );

	class ErrorData;

	/**
	 * Represents an error in %Soprano.
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
	     * or a user defined value which has to be bigger than ERROR_UNKNOWN. The redland backend
	     * for example uses error codes <i>ERROR_UNKNOWN + redlandCode</i>.
	     */
	    Error( const QString& message, int code = ERROR_UNKNOWN );

	    Error( const Error& );

	    virtual ~Error();

	    Error& operator=( const Error& );

	    /**
	     * An %Error evalutes to a boolean, indicating if an %Error is "set".
	     *
	     * \return \p false if code() == ERROR_NONE, \p true otherwise.
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
	    operator bool() const { return code() != ERROR_NONE; }

	    QString message() const;
	    int code() const;

	    bool isParserError() const;
	
	protected:
	    Error( ErrorData* );
	    QSharedDataPointer<ErrorData> d;
	};

	class Locator;

	/**
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
	 */
	class SOPRANO_EXPORT ParserError : public Error
	{
	public:
	    /**
	     * Create an Error object, representing success, i.e. no error.
	     */
	    ParserError();

	    ParserError( const Locator&, const QString& message = QString(), int code = ERROR_PARSING_FAILED );

	    ParserError( const Error& );

	    ~ParserError();

	    ParserError& operator=( const Error& );

	    Locator locator() const;
	};

	/**
	 * The ErrorCache caches Error instances for different threads.
	 * Each thread has its own last error. This mechanism tries to
	 * replace the missing exceptions for methods that do not return
	 * an error code or another value that can state the success of the
	 * method's operation.
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
	     * Convinience method to set simple string error messages with a default
	     * error code ERROR_UNKNOWN.
	     */
	    void setError( const QString& errorMessage, int code = ERROR_UNKNOWN ) const;

	private:
	    class Private;
	    Private* const d;
	};
    }
}

#endif
