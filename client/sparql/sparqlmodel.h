/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007 Rajeev J Sebastian <rajeev.sebastian@gmail.com>
 * Copyright (C) 2008-2009 Sebastian Trueg <trueg@kde.org>
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

#ifndef _SOPRANO_SPARQL_MODEL_H_
#define _SOPRANO_SPARQL_MODEL_H_

#include "model.h"
#include "soprano_export.h"
#include "asyncresult.h"


namespace Soprano {
    namespace Client {
        /**
         * \class SparqlModel sparqlmodel.h Soprano/Client/SparqlModel
         *
         * \brief Remote client Model for Http SPARQL end points.
         *
         * The SparqlModel provides a very simple way of accessing remote
         * <a href="http://www.w3.org/TR/rdf-sparql-protocol/">SPARQL (SPARQL Protocol and RDF Query Language)</a>
         * web services via Http.
         *
         * Its usage is simple: set hostname and optionally user credentials, then
         * call the well known Model methods like Model::executeQuery to work with the remote
         * repository.
         *
         * \author Rajeev J Sebastian <rajeev.sebastian@gmail.com><br>Sebastian Trueg <trueg@kde.org>
         *
         * \since 2.2
         */
        class SOPRANO_CLIENT_EXPORT SparqlModel : public Soprano::Model
        {
            Q_OBJECT

        public:
            /**
             * Create a new SparqlModel instance.
             *
             * \param host The host to connect to (example: dbpedia.org)
             * \param port The port on which to connect the host (most
             *        Http services run on port 80.
             * \param user The userName in case the host does not allow
             *        anonymous access.
             * \param password The password for \p user in case the host
             * does not allow anonymous access.
             */
            SparqlModel( const QString& host = QString(),
                         quint16 port = 80,
                         const QString& user = QString(),
                         const QString& password = QString() );

            /**
             * Destructor.
             */
            ~SparqlModel();

            //@{
            /**
             * Set the host to connect to.
             *
             * \param host The host to connect to (example: dbpedia.org)
             * \param port The port on which to connect the host (most
             *        Http services run on port 80.
             */
            void setHost( const QString& host, quint16 port = 80 );

            /**
             * Set the user name and password to access the host.
             *
             * \param user The userName in case the host does not allow
             *        anonymous access.
             * \param password The password for \p user in case the host
             */
            void setUser( const QString& user, const QString& password = QString() );

            /**
             * Set the path to where the SPARQL endpoint is exposed on the server.
             *
             * For historical reasons the default path is set to "/sparql".
             *
             * \since 2.2.1
             */
            void setPath( const QString& path );
            //@}

            //@{
            /**
             * Add a statement to the remote model.
             *
             * This method is realized using the <a href="http://jena.hpl.hp.com/~afs/SPARQL-Update.html">SPARQL/Update</a>
             * language extension. Thus, it will only work on services supporting this extension.
             *
             * \param statement The Statement to add.
             *
             * \return Error::ErrorNone on success and an error code if statement was invalid or an error
             * occured. Check Error::ErrorCache::lastError for detailed error information.
             */
            Error::ErrorCode addStatement( const Statement& statement );

            /**
             * Removed a statement from the remote model.
             *
             * This method is realized using the <a href="http://jena.hpl.hp.com/~afs/SPARQL-Update.html">SPARQL/Update</a>
             * language extension. Thus, it will only work on services supporting this extension.
             *
             * \param statement The Statement to remove.
             *
             * \return Error::ErrorNone on success and an error code if statement was invalid or an error
             * occured. Check Error::ErrorCache::lastError for detailed error information.
             */
            Error::ErrorCode removeStatement( const Statement& statement );

            /**
             * Remove all statements that match the partial statement. For removing
             * one specific statement see removeStatement().
             *
             * This method is realized using the <a href="http://jena.hpl.hp.com/~afs/SPARQL-Update.html">SPARQL/Update</a>
             * language extension. Thus, it will only work on services supporting this extension.
             *
             * \param statement A possible partially defined statement that serves as
             * a filter for all statements that should be removed.
             *
             * \return Error::ErrorNone on success and an error code if statement was invalid or an error
             * occured. Check Error::ErrorCache::lastError for detailed error information.
             */
            Error::ErrorCode removeAllStatements( const Statement& statement );
            //@}

            //@{
            /**
             * \reimpl
             */
            Soprano::StatementIterator listStatements( const Statement& partial ) const;

            /**
             * Asyncroneously list statements.
             *
             * \param statement The partial Statement to match.
             *
             * \return an AsyncResult with result type StatementIterator
             * object which will signal when the result is ready.
             *
             * \since 2.4
             */
            Soprano::Util::AsyncResult* listStatementsAsync( const Statement& statement ) const;

            /**
             * Execute a query on the SPARQL endpoint.
             *
             * \param query The query to evaluate.
             * \param language The query language used to encode \p query. Be aware that
             *        the SparqlModel does only support one query language: Query::QueryLanguageSparql.
             * \param userQueryLanguage unused since \p language needs to be set to Query::QueryLanguageSparql.
             *
             * \return An iterator over all results matching the query,
             * on error an invalid iterator is returned.
             */
            Soprano::QueryResultIterator executeQuery( const QString& query,
                                                       Query::QueryLanguage language = Query::QueryLanguageSparql,
                                                       const QString& userQueryLanguage = QString() ) const;



            /**
             * Asyncroneously execute the given query over the Model.
             *
             * \param query The query to evaluate.
             * \param language The %query language used to encode \p query.
             * \param userQueryLanguage If \p language equals Query::QueryLanguageUser
             * userQueryLanguage defines the language to use.
             *
             * \sa executeQuery
             *
             * \return an AsyncResult with result type QueryResultIterator
             * object which will signal when the result is ready.
             */
            Soprano::Util::AsyncResult* executeQueryAsync( const QString& query,
                                                           Query::QueryLanguage language = Query::QueryLanguageSparql,
                                                           const QString& userQueryLanguage = QString() ) const;

            /**
             * \reimpl
             */
            NodeIterator listContexts() const;

            /**
             * Asyncroneously list all contexts.
             *
             * \param statement The partial Statement to match.
             *
             * \return an AsyncResult with result type NodeIterator
             * object which will signal when the result is ready.
             *
             * \since 2.4
             */
            Soprano::Util::AsyncResult* listContextsAsync() const;

            /**
             * \reimpl
             */
            bool containsStatement( const Statement& statement ) const;

            /**
             * \reimpl
             */
            bool containsAnyStatement( const Statement& statement ) const;
            //@}

            //@{
            /**
             * Retrieving the number of statements is not supported by the SparqlModel.
             *
             * \return -1
             */
            int statementCount() const;

            /**
             * Not supported by the SparqlModel.
             *
             * \return false
             */
            bool isEmpty() const;
            //@}

            /**
             * Creation of blank nodes is not supported by the SparqlModel.
             *
             * \return an invalid Node
             */
            Node createBlankNode();

            using Model::addStatement;
            using Model::removeStatement;
            using Model::removeAllStatements;
            using Model::listStatements;
            using Model::containsStatement;
            using Model::containsAnyStatement;

        private Q_SLOTS:
            void slotRequestFinished( int id, bool error, const QByteArray& data );

        private:
            class Private;
            Private* const d;
        };
    }
}

#endif
