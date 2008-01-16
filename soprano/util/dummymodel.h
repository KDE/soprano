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

#ifndef SOPRANO_DUMMY_MODEL_H
#define SOPRANO_DUMMY_MODEL_H

#include "soprano_export.h"
#include "model.h"


namespace Soprano {
    namespace Util {
        /**
         * \class DummyModel dummymodel.h Soprano/Util/DummyModel
         *
         * \brief A dummy model. All its methods do nothing and always
         * fail with a default error.
         *
         * A dummy model might be useful in situations where a null pointer
         * could lead to crashes if creating a real model fails.
         *
         * \author Sebastian Trueg <trueg@kde.org>
         */
        class SOPRANO_EXPORT DummyModel : public Model
        {
            Q_OBJECT

        public:
            /**
             * Create a new dummy model.
             */
            DummyModel();

            /**
             * destructor
             */
            ~DummyModel();

            //@{
            Error::ErrorCode addStatement( const Statement &statement );
            Error::ErrorCode addStatement( const Node& subject, const Node& predicate, const Node& object, const Node& context = Node() );
            Error::ErrorCode addStatements( const QList<Statement> &statements );
            //@}

            //@{
            Error::ErrorCode removeStatement( const Statement &statement );
            Error::ErrorCode removeStatement( const Node& subject, const Node& predicate, const Node& object, const Node& context = Node() );
            Error::ErrorCode removeAllStatements( const Statement &statement );
            Error::ErrorCode removeStatements( const QList<Statement> &statements );
            Error::ErrorCode removeContext( const Node& );
            Error::ErrorCode removeAllStatements();
            //@}

            //@{
            StatementIterator listStatements( const Statement &partial ) const;
            StatementIterator listStatements( const Node& subject, const Node& predicate, const Node& object, const Node& context = Node() ) const;
            StatementIterator listStatements() const;
            StatementIterator listStatementsInContext( const Node &context ) const;
            NodeIterator listContexts() const;
//    QueryResultIterator executeQuery( const Query::Query& query ) const;
            QueryResultIterator executeQuery( const QString& query, Query::QueryLanguage language, const QString& userQueryLanguage = QString() ) const;
            //@}

            //@{
            bool containsAnyStatement( const Statement &statement ) const;
            bool containsAnyStatement( const Node& subject, const Node& predicate, const Node& object, const Node& context = Node() ) const;
            bool containsStatement( const Statement &statement ) const;
            bool containsStatement( const Node& subject, const Node& predicate, const Node& object, const Node& context = Node() ) const;
            bool containsContext( const Node &context ) const;
            bool isEmpty() const;
            int statementCount() const;
            //@}

            //@{
            Error::ErrorCode write( QTextStream &os ) const;
            //@}

            //@{
            Node createBlankNode();
            //@}

        private:
            class Private;
            Private* const d;
        };
    }
}

#endif
