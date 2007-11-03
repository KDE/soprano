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

#include "plugin.h"
#include "sopranotypes.h"
#include "soprano_export.h"
#include "error.h"

namespace Soprano {
    namespace Query {

	class Query;

	/**
	 * \class Parser queryparser.h Soprano/Query/Parser
	 *
	 * \brief Soprano::Query::Parser defines the interface for a Soprano query parser plugin.
	 *
	 * Each parser plugin may support multiple query languages (supportedQueryLanguages() and supportedUserQueryLanguages()).
	 *
	 * \sa \ref soprano_writing_plugins
	 *
	 * \author Daniele Galdi <daniele.galdi@gmail.com><br>Sebastian Trueg <trueg@kde.org>
	 */
	class SOPRANO_EXPORT Parser : public Plugin, public Error::ErrorCache
	{
	public:
	    virtual ~Parser();

	    /**
	     * Translate the given query into a proper Soprano query-object.
	     *
	     * \param query The %query string to parse.
	     * \param lang The language the %query is written in.
	     * \param userQueryLanguage If lang is set to Query::QueryLanguageUser this parameter specifies the
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
	     * the list contains Query::QueryLanguageUser the query parser supports additional query languages not
	     * officially supported by %Soprano.
	     *
	     * \sa supportedUserQueryLanguages()
	     */
	    virtual QueryLanguages supportedQueryLanguages() const = 0;

	    /**
	     * Check if the parser supports a specific query language.
	     *
	     * \param lang The query language to support.
	     * \param userQueryLanguage If lang is set to Query::QueryLanguageUser this parameter specifies the
	     *       requested query language. It allows the extension of the %Soprano Query interface with new
	     *       query languages that are not officially supported by %Soprano.
	     *
	     * \return \p true if the parser support query lanaguge lang.
	     */
	    bool supportsQueryLanguage( QueryLanguage lang, const QString& userQueryLanguage = QString() ) const;
	    
	    /**
	     * A query parser can support additional query languages that are not defined in Query::QueryLanguage.
	     * In that case supportedQueryLanguages() has to include Query::QueryLanguageUser.
	     *
	     * The default implementation returns an empty list.
	     *
	     * \return A list of supported query languages.
	     */
	    virtual QStringList supportedUserQueryLanguages() const;

	protected:
	    Parser( const QString& name );

	private:
	    class Private;
	    Private* const d;
	};
    }
}

Q_DECLARE_INTERFACE(Soprano::Query::Parser, "org.soprano.plugins.QueryParser/1.0")

#endif // SOPRANO_QUERY_QUERYPARSER_H
