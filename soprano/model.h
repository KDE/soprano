/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
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

#ifndef SOPRANO_MODEL_H
#define SOPRANO_MODEL_H

#include <QtCore/QObject>
#include <QtCore/QList>

#include "soprano_export.h"
#include "error.h"
#include "sopranotypes.h"
#include "node.h"

class QTextStream;

namespace Soprano
{
    class QueryLegacy;
    class QueryResultIterator;
    class Statement;
    class StatementIterator;
    class NodeIterator;
    namespace Query {
        class Query;
    }

    /**
     * \class Model model.h Soprano/Model
     *
     * \brief A Model is the central class in %Soprano. It is a queryable
     * collection of RDF quadruples, i.e statements.
     *
     * Model itself is just an interface for numerous implementations. (If you are looking for a simple container
     * for statements, see Graph.) There are basically two types of Models in %Soprano:
     *
     * \li StorageModel is the base class for Model implementations that actually store RDF quadruples.
     *     StorageModels are created transparently by %Soprano backend plugins: Backend::createModel()
     * \li FilterModel is the base class for all filter models. FilterModels can be stacked on top of
     *     a StorageModel to perform arbitrary tasks like inference or actual content filtering. An
     *     important FilterModel is Inference::InferenceModel.
     *
     * The simplest way to create a memory Model is to use the default Backend:
     *
     * \code
     * Model* memModel = Soprano::createModel();
     * \endcode
     *
     * <b>%Error handling:</b>
     *
     * Model is based on %Soprano's own error handling system which tries to emulate exceptions to a certain extend.
     * Most methods in Model have a means of reporting if an operation was successful or not. For additional error
     * information Model inherits ErrorCache which provides the method lastError().
     *
     * Thus, advanced error handling would look as follows:
     *
     * \code
     * Soprano::Model* model = Soprano::createModel();
     * Soprano::Statement invalidStatement;
     * if( model->addStatement( invalidStatement ) != Error::ErrorNone ) {
     *    showErrorMessage( model->lastError().message() );
     * }
     * \endcode
     *
     * For this to work properly Model implementations have to reset the error in each method
     * by either calling clearError() or setError().
     *
     * \sa \ref soprano_error_handling
     *
     * Model is thread-safe when used with a thread-safe backend (all "official" %Soprano backends are thread-safe).
     * However, it is recommended to create Model instances in the main thread.
     *
     * \author Daniele Galdi <daniele.galdi@gmail.com><br>Sebastian Trueg <trueg@kde.org>
     */
    class SOPRANO_EXPORT Model : public QObject, public Error::ErrorCache
    {
        Q_OBJECT

    public:
        virtual ~Model();

        //@{
        /**
         * Add the Statement to the Model.
         *
         * \param statement The Statement to add.
         */
        virtual Error::ErrorCode addStatement( const Statement &statement ) = 0;

        /**
         * \overload
         */
        Error::ErrorCode addStatement( const Node& subject, const Node& predicate, const Node& object, const Node& context = Node() );

        /**
         * \overload
         */
        Error::ErrorCode addStatements( const QList<Statement> &statements );
        //@}

        //@{
        /**
         * Remove one statement. For removing statements with wildward matching see removeAllStatements().
         *
         * \param statement The statement that should be removed. This has to be a valid statement.
         *
         * \return Error::ErrorNone on success and an error code if statement was invalid or an error
         * occured.
         */
        virtual Error::ErrorCode removeStatement( const Statement &statement ) = 0;

        /**
         * \overload
         */
        Error::ErrorCode removeStatement( const Node& subject, const Node& predicate, const Node& object, const Node& context = Node() );

        /**
         * Remove all statements that match the partial statement. For removing
         * one specific statement see removeStatement().
         *
         * \param statement A possible partially defined statement that serves as
         * a filter for all statements that should be removed.
         */
        virtual Error::ErrorCode removeAllStatements( const Statement &statement ) = 0;

        /**
         * \overload
         *
         * \param subject The subject node to match. Can be empty as a wildcard.
         * \param predicate The predicate node to match. Can be empty as a wildcard.
         * \param object The object node to match. Can be empty as a wildcard.
         * \param context The context node to match. Can be empty as a wildcard.
         */
        Error::ErrorCode removeAllStatements( const Node& subject, const Node& predicate, const Node& object, const Node& context = Node() );

        /**
         * Convenience method which removes all %statements in statements.
         */
        Error::ErrorCode removeStatements( const QList<Statement> &statements );

        /**
         * Convenience method that removes all statements in the context.
         */
        Error::ErrorCode removeContext( const Node& );

        /**
         * Convenience method that clear the Model of all statements
         */
        Error::ErrorCode removeAllStatements();
        //@}


        //@{
        /**
         * Return an iterator over Model Statements that "partial"
         * match the input Statement.
         *
         * \param partial The partial Statement to match.
         *
         * \return An iterator for all the matched Statements, on error an invalid iterator is returned.
         */
        virtual StatementIterator listStatements( const Statement &partial ) const = 0;

        /**
         * \overload
         *
         * \param subject The subject node to match. Can be empty as a wildcard.
         * \param predicate The predicate node to match. Can be empty as a wildcard.
         * \param object The object node to match. Can be empty as a wildcard.
         * \param context The context node to match. Can be empty as a wildcard.
         *
         * \return An iterator for all the matched Statements, on error an invalid iterator is returned.
         */
        StatementIterator listStatements( const Node& subject, const Node& predicate, const Node& object, const Node& context = Node() ) const;

        /**
         * \overload
         *
         * \return An iterator for all statements in the model, on error an invalid iterator is returned.
         */
        StatementIterator listStatements() const;

        /**
         * Convenience method which lists all statements in context.
         *
         * \return An iterator for all the matched Statements, on error an invalid iterator is returned.
         */
        StatementIterator listStatementsInContext( const Node &context ) const;

        /**
         * List all contexts in the model, i.e. all named graphs.
         *
         * \return An iterator over context Nodes, on error an invalid iterator is returned.
         */
        virtual NodeIterator listContexts() const = 0;

        /** \cond query_api_disabled */

        /**
         * Execute the given query over the Model.
         *
         * \param query The query to evaluate.
         *
         * \return An iterator over all results matching the query,
         * on error an invalid iterator is returned.
         *
         * \sa Query::QueryParser
         */
//    virtual QueryResultIterator executeQuery( const Query::Query& query ) const = 0;

        /** \endcond query_api_disabled */

        /**
         * Execute the given query over the Model.
         *
         * This is a const read-only method. As such Model implementations should not
         * support SPARQL extensions such as INSERT or UPDATE through this method.
         * A future version of %Soprano will provide an additional API for queries
         * that change the Model.
         *
         * \param query The query to evaluate.
         * \param language The %query language used to encode \p query.
         * \param userQueryLanguage If \p language equals Query::QueryLanguageUser
         * userQueryLanguage defines the language to use.
         *
         * \return An iterator over all results matching the query,
         * on error an invalid iterator is returned.
         */
        virtual QueryResultIterator executeQuery( const QString& query, Query::QueryLanguage language, const QString& userQueryLanguage = QString() ) const = 0;
        //@}


        //@{
        /**
         * Check if the model contains certain statements.
         *
         * \param statement A partially defined statement that serves as
         * a pattern.
         *
         * \return true if the Model contains a Statement matching the given statement
         * pattern.
         */
        virtual bool containsAnyStatement( const Statement &statement ) const = 0;

        /**
         * \overload
         *
         * \param subject The subject node to match. Can be empty as a wildcard.
         * \param predicate The predicate node to match. Can be empty as a wildcard.
         * \param object The object node to match. Can be empty as a wildcard.
         * \param context The context node to match. Can be empty as a wildcard.
         */
        bool containsAnyStatement( const Node& subject, const Node& predicate, const Node& object, const Node& context = Node() ) const;

        /**
         * Check if the model contains a statements.
         *
         * \param statement The statement in question. This has to be a valid statement,
         * i.e. subject, predicate, and object need to be defined. If the context node
         * is empty the default graph is searched.
         *
         * \return \p true if the Model contains the Statement, \p false otherwise or
         * is statement is invalid.
         */
        virtual bool containsStatement( const Statement &statement ) const = 0;

        /**
         * \overload
         */
        bool containsStatement( const Node& subject, const Node& predicate, const Node& object, const Node& context = Node() ) const;

        /**
         * Convenience method which is based on containsAnyStatement
         */
        bool containsContext( const Node &context ) const;

        /**
         * \return true if the Model doesn't contains any Statement.
         */
        virtual bool isEmpty() const = 0;

        /**
         * The number of statements stored in this Model.
         * \return The size of the Model, or -1 on error.
         */
        virtual int statementCount() const = 0;
        //@}


        //@{
        /**
         * Write all statements in this Model to os.
         *
         * Default implementation is based on Model::listStatements
         */
        virtual Error::ErrorCode write( QTextStream &os ) const;
        //@}


        //@{
        /**
         * Creates a new blank node with a unique identifier.
         *
         * \return A blank node that can be used to create new statements.
         */
        virtual Node createBlankNode() = 0;
        //@}

    Q_SIGNALS:
        /**
         * Emitted when new statements have been added to the model.
         *
         * Implementations of this interface have to emit this signal.
         */
        void statementsAdded();

        /**
         * Emitted when statements have been removed from the model.
         *
         * Implementations of this interface have to emit this signal.
         */
        void statementsRemoved();

        /**
         * Notification signal for new statements. Model implementations
         * should emit this signal for each newly added statement.
         */
        void statementAdded( const Soprano::Statement& statement );

        /**
         * Notification signal for removed statements. Model implementations
         * should emit this signal for each removed statement.
         *
         * \warning Backends may choose not to emit this signal for each
         * removed statement but only for a statement pattern (i.e. an
         * invalid statement as used in removeAllStatements()) to
         * prevent massive performance loss.
         */
        void statementRemoved( const Soprano::Statement& statement );

    protected:
        Model();

    private:
        /**
         * Model instances are not meant to be copied.
         */
        Model( const Model& );
        Model& operator=( const Model& );

        class Private;
        Private* const d;
    };
}

#endif
