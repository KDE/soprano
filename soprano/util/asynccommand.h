/* 
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2008 Sebastian Trueg <trueg@kde.org>
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

#ifndef _SOPRANO_UTIL_ASYNC_COMMAND_H_
#define _SOPRANO_UTIL_ASYNC_COMMAND_H_

#include <QtCore/QRunnable>

#include "asyncmodel.h"
#include "statement.h"
#include "node.h"

namespace Soprano {

    class StatementIterator;
    class NodeIterator;
    class QueryResultIterator;
    class Model;

    namespace Util {

        class AsyncModelPrivate;

        class Command : public QRunnable
        {
        public:
            enum Type {
                ReadCommand,
                WriteCommand
            };

            Command( AsyncResult* result, Model* model, Type type );
            virtual ~Command();

            Type type() const { return m_type; }
            AsyncResult* result() const { return m_result; }
            Model* model() const { return m_model; }

            virtual void execute() = 0;

            // reimplemented from QRunnable
            void run() { execute(); }

        private:
            AsyncResult* m_result;
            Model* m_model;
            Type m_type;
        };


        class StatementCountCommand : public Command
        {
        public:
            StatementCountCommand( AsyncResult* result, Model* model );

            void execute();
        };


        class IsEmptyCommand : public Command
        {
        public:
            IsEmptyCommand( AsyncResult* result, Model* model );

            void execute();
        };


        class StatementCommand : public Command
        {
        public:
            virtual ~StatementCommand() {}

            Statement statement() const { return m_statements.first(); }
            QList<Statement> statements() const { return m_statements; }

        protected:
            StatementCommand( AsyncResult* result, Model* model, const Statement&, Type type );
            StatementCommand( AsyncResult* result, Model* model, const QList<Statement>&, Type type );

        private:
            QList<Statement> m_statements;
        };


        class AddStatementCommand : public StatementCommand
        {
        public:
            AddStatementCommand( AsyncResult* result, Model* model, const QList<Statement>& );

            void execute();
        };


        class RemoveStatementCommand : public StatementCommand
        {
        public:
            RemoveStatementCommand( AsyncResult* result, Model* model, const QList<Statement>& );

            void execute();
        };


        class RemoveAllStatementsCommand : public StatementCommand
        {
        public:
            RemoveAllStatementsCommand( AsyncResult* result, Model* model, const Statement& );

            void execute();
        };


        class ContainsStatementCommand : public StatementCommand
        {
        public:
            ContainsStatementCommand( AsyncResult* result, Model* model, const Statement& );

            void execute();
        };


        class ContainsAnyStatementCommand : public StatementCommand
        {
        public:
            ContainsAnyStatementCommand( AsyncResult* result, Model* model, const Statement& );

            void execute();
        };


        class ListStatementsCommand : public StatementCommand
        {
        public:
            ListStatementsCommand( AsyncModelPrivate*, AsyncResult* result, Model* model, const Statement& );

            void execute();

        private:
            AsyncModelPrivate* m_asyncModelPrivate;
        };


        class ListContextsCommand : public Command
        {
        public:
            ListContextsCommand( AsyncModelPrivate*, AsyncResult* result, Model* model );

            void execute();

        private:
            AsyncModelPrivate* m_asyncModelPrivate;
        };


        class ExecuteQueryCommand : public Command
        {
        public:
            ExecuteQueryCommand( AsyncModelPrivate*, AsyncResult* result, Model* model, const QString&, Query::QueryLanguage lang, const QString& userQueryLang );

            void execute();

        private:
            QString m_query;
            Query::QueryLanguage m_queryLanguage;
            QString m_userQueryLanguage;

            AsyncModelPrivate* m_asyncModelPrivate;
        };


        class CreateBlankNodeCommand : public Command
        {
        public:
            CreateBlankNodeCommand( AsyncResult* result, Model* model );

            void execute();
        };
    }
}

#endif
