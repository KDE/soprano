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

#ifndef _SOPRANO_INFERENCE_RULE_H_
#define _SOPRANO_INFERENCE_RULE_H_

#include <QtCore/QSharedDataPointer>
#include <QtCore/QList>

#include "statementpattern.h"
#include "soprano_export.h"


namespace Soprano {
    namespace Inference {
	class SOPRANO_EXPORT Rule
	{
	public:
	    Rule();
	    Rule( const Rule& other );
	    ~Rule();

	    Rule& operator=( const Rule& other );

	    /**
	     * The list of preconditions for the rule. Each condition
	     * is a StatementPattern which can contain variables.
	     * These variables are identified by their name and matched
	     * accordingly.
	     */
	    QList<StatementPattern> preConditions() const;

	    void addPrecondition( const StatementPattern& );

	    /**
	     * The effect of a rule is a pattern that repeats variables from
	     * the preconditions and thus, identifies the statements that
	     * are to be infered from the preconditions.
	     */
	    StatementPattern effect() const;

	    void setEffect( const StatementPattern& );

	    /**
	     * Check if a statement matches any of the statement patterns
	     * in this rule.
	     * \return true if statement matches any of the patterns, false otherwise.
	     */
	    bool match( const Statement& statement ) const;

	    /**
	     * Create a SPARQL query that retrieves all resources matching this rule.
	     */
	    QString createSparqlQuery() const;

	    // FIXME: add something like:
	    // static QList<Rule> parseRuleFile( const QString& path );

	private:
	    class Private;
	    QSharedDataPointer<Private> d;
	};
    }
}

#endif
