/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007 Daniele Galdi <daniele.galdi@gmail.com>
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

#ifndef SOPRANO_QUERY_QUERYPARSER_H
#define SOPRANO_QUERY_QUERYPARSER_H

#include "soprano/plugin.h"
#include "soprano/sopranotypes.h"
#include "soprano/soprano_export.h"

#include <QtCore/QObject>

namespace Soprano {

    class Locator;

    namespace Query {

    class Query;

	/**
	 * \brief Soprano::Query::Parser defines the interface for a Soprano query parser plugin.
	 *
	 * Each parser plugin may support multiple query languages (supportedQueryLanguages() and supportedUserQueryLanguages()).
	 * 
	 * To create a new query parser plugin simply create a class that implements this interface
	 * and is derived from QObject. Then use the Q_INTERFACES macro to define that it
	 * is in fact a Backend plugin and export the plugin via the Q_EXPORT_PLUGIN2 macro.
	 *
	 * \code
	 * class MyQueryParser : public Soprano::Query::Parser
	 * {
	 *   Q_INTERFACES(Soprano::Query::Parser)
	 *
	 *  public:
	 *   Query parseQuery( const QString &query, QueryLanguage lang, const QString& userQueryLanguage = QString() );
	 *   QueryLanguages supportedQueryLanguages() const;
	 * };
	 * \endcode
	 *
	 * In the implementation file export the plugin so it can be picked up by the
	 * plugin loading framework:
	 *
	 * \code
	 * Q_EXPORT_PLUGIN2(soprano_myqueryparser, MyBackend)
	 * \endcode
	 *
	 * \author Daniele Galdi <daniele.galdi@gmail.com><br>Sebastian Trueg <trueg@kde.org>
	 */
	class SOPRANO_EXPORT Parser : public QObject, public Plugin
	{
    Q_OBJECT
	public:
	    virtual ~Parser();

	    /**
	     * Translate the given query into a proper Soprano query-object.
	     *
	     * \param query The %query string to parse.
	     * \param lang The language the %query is written in.
	     * \param userQueryLanguage If lang is set to Query::QUERY_LANGUAGE_USER this parameter specifies the
	     *       query language to use. It allows the extension of the %Soprano Query interface with new
	     *       query languages that are not officially supported by %Soprano.
	     *
	     * \return A Query object representing the parsed %query or an empty invalid Query object.
	     */
	    virtual Query parseQuery( const QString &query, QueryLanguage lang, const QString& userQueryLanguage = QString() ) const = 0;

	    /**
	     * A query parser can support different query languages.
	     *
	     * \return A combination of Query::QueryLanguage values representing the supported query languages. If
	     * the list contains Query::QUERY_LANGUAGE_USER the query parser supports additional query languages not
	     * officially supported by %Soprano.
	     *
	     * \sa supportedUserQueryLanguages()
	     */
	    virtual QueryLanguages supportedQueryLanguages() const = 0;

	    /**
	     * Check if the parser supports a specific query language.
	     *
	     * \param lang The query language to support.
	     * \param userQueryLanguage If lang is set to Query::QUERY_LANGUAGE_USER this parameter specifies the
	     *       requested query language. It allows the extension of the %Soprano Query interface with new
	     *       query languages that are not officially supported by %Soprano.
	     *
	     * \return \p true if the parser support query lanaguge lang.
	     */
	    bool supportsQueryLanguage( QueryLanguage lang, const QString& userQueryLanguage = QString() ) const;
	    
	    /**
	     * A query parser can support additional query languages that are not defined in Query::QueryLanguage.
	     * In that case supportedQueryLanguages() has to include Query::QUERY_LANGUAGE_USER.
	     *
	     * The default implementation returns an empty list.
	     *
	     * \return A list of supported query languages.
	     */
	    virtual QStringList supportedUserQueryLanguages() const;

    signals:
        void syntaxError( Locator &locator, QString& message );

	protected:
	    Parser( const QString& name );

        void emitSyntaxError( Locator& locator, QString& message );

	private:
	    class Private;
	    Private* const d;
	};
    }
}

Q_DECLARE_INTERFACE(Soprano::Query::Parser, "org.soprano.plugins.QueryParser/1.0")

#endif // SOPRANO_QUERY_QUERYPARSER_H
