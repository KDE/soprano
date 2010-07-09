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

#ifndef _SOPRANO_NRL_MODEL_H_
#define _SOPRANO_NRL_MODEL_H_

#include "filtermodel.h"
#include "soprano_export.h"

#ifndef USING_SOPRANO_NRLMODEL_UNSTABLE_API
#error The Soprano::NrlModel API is subject to change. This has to be acnowledged by defining USING_SOPRANO_NRLMODEL_UNSTABLE_API
#endif

namespace Soprano {
    /**
     * \class NRLModel nrlmodel.h Soprano/NRLModel
     *
     * \brief Model filter that makes working with NRL data simpler.
     *
     * The NRLModel enforces NRL cardinality rules. That means predicates with a cardinality
     * of maximum 1 are always udpated while statements that define predicates with a maximum
     * cardinality bigger than 1 are rejected once the maximum is reached (future versions
     * might remove an earlier defined statement based on the time the old statements were
     * added).
     *
     * Thus, at the moment NRLModel is mostly usable for handling properties with a maximum
     * cardinality of 1.
     *
     * NRLModel also provides automatic query prefix expansion and named graph creation and
     * removal with automatic metadata graph handling.
     *
     * THE API AND EVEN THE COMPLETE CLASS IS SUBJECT TO CHANGE!
     *
     * \author Sebastian Trueg <trueg@kde.org>
     *
     * \sa Vocabulary::NRL
     */
    class SOPRANO_EXPORT NRLModel : public FilterModel
    {
        Q_OBJECT

    public:
        NRLModel();
        NRLModel( Model* parent );
        ~NRLModel();

        /**
         * When enforcing the NRL cardinality rules NRLModel can either
         * ignore the context of statements or treat different contexts
         * as separate sets, each resetting the cardinality.
         *
         * \param b If \p true (the default) NRLModel does ignore the context
         * when enforcing rules. If \p false the NRL rules can be violated
         * across contexts.
         *
         * By default contexts are ignored.
         *
         * \sa ignoreContext()
         */
        void setIgnoreContext( bool b );

        /**
         * \return \p true if contexts should be ignored when enforcing NRL
         * rules.
         *
         * \sa setIgnoreContext()
         */
        bool ignoreContext() const;

        /**
         * Enable automatic query prefix expansion based on nao:hasDefaultNamespaceAbbreviation.
         * This will trigger reading all prefixes stored in the underlying model.
         *
         * By default query prefix expansion is disabled.
         *
         * \since 2.4
         */
        void setEnableQueryPrefixExpansion( bool enable );

        /**
         * \return \p true if query prefix expansion is enabled.
         *
         * \since 2.4
         */
        bool queryPrefixExpansionEnabled() const;

        /**
         * The prefixes that have been read from the underlying model.
         *
         * \return A list of prefixes mapping to the namespaces read from
         * the model. This list is empty if queryPrefixExpansion is disabled.
         *
         * \since 2.4
         */
        QHash<QString, QUrl> queryPrefixes() const;

        /**
         * Add a statement.
         *
         * \param s The statement containing the property to be set.
         * If the predicate has NRL cardinality restrictions existing
         * statements will be updated. Otherwise this method has the
         * same effect as Model::addStatement().
         *
         * Adding a statement that defines a predicate with a maximum
         * cardinality bigger than 1 which has already been reached
         * fails with an error.
         *
         * \return Error::ErrorNone on success.
         */
        Error::ErrorCode addNrlStatement( const Statement& s );

        /**
         * Create a new graph of type \p type.
         *
         * This will actually create two graphs: the requested one
         * and its metadata graph which will already contain basic
         * metadata like creation date.
         *
         * \return The URI of the newly created graph.
         *
         * \since 2.3
         */
        QUrl createGraph( const QUrl& type, QUrl* metadataGraph = 0 );

        /**
         * Remove a complete graph including its metadata graph
         *
         * This method can be seen as the counterpart to createGraph
         *
         * \since 2.3
         */
        Error::ErrorCode removeGraph( const QUrl& graph );

        /**
         * If queryPrefixExpansionEnabled is \p true query prefixes will be expanded before sending the
         * query to the underlying model.
         *
         * \since 2.4
         */
        virtual QueryResultIterator executeQuery( const QString& query, Query::QueryLanguage language, const QString& userQueryLanguage = QString() ) const;

        /**
         * If the only node defined in \p statement is the context the graph including its
         * metadata is removed.
         *
         * \sa removeGraph
         *
         * \since 2.4
         */
        virtual Error::ErrorCode removeAllStatements( const Statement& statement );

    private:
        class Private;
        Private* const d;
    };
}

#endif
