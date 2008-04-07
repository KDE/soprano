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

        class Command
        {
        public:
            enum Type {
                ReadCommand,
                WriteCommand
            };

            Command( Type type );
            virtual ~Command();

            Type type() const { return m_type; }

            virtual void execute( Model* ) = 0;

        private:
            Type m_type;
        };


        class StatementCountCommand : public Command
        {
        public:
            StatementCountCommand( AsyncResult* result );

            void execute( Model* );

        private:
            AsyncResult* m_result;
        };


        class IsEmptyCommand : public Command
        {
        public:
            IsEmptyCommand( AsyncResult* result );

            void execute( Model* );

        private:
            AsyncResult* m_result;
        };


        class StatementCommand : public Command
        {
        public:
            virtual ~StatementCommand() {}

            Statement statement() const { return m_statement; }

        protected:
            StatementCommand( const Statement&, Type type );

        private:
            Statement m_statement;
        };


        class AddStatementCommand : public StatementCommand
        {
        public:
            AddStatementCommand( AsyncResult* result, const Statement& );

            void execute( Model* );

        private:
            AsyncResult* m_result;
        };


        class RemoveStatementCommand : public StatementCommand
        {
        public:
            RemoveStatementCommand( AsyncResult* result, const Statement& );

            void execute( Model* );

        private:
            AsyncResult* m_result;
        };


        class RemoveAllStatementsCommand : public StatementCommand
        {
        public:
            RemoveAllStatementsCommand( AsyncResult* result, const Statement& );

            void execute( Model* );

        private:
            AsyncResult* m_result;
        };


        class ContainsStatementCommand : public StatementCommand
        {
        public:
            ContainsStatementCommand( AsyncResult* result, const Statement& );

            void execute( Model* );

        private:
            AsyncResult* m_result;
        };


        class ContainsAnyStatementCommand : public StatementCommand
        {
        public:
            ContainsAnyStatementCommand( AsyncResult* result, const Statement& );

            void execute( Model* );

        private:
            AsyncResult* m_result;
        };


        class ListStatementsCommand : public StatementCommand
        {
        public:
            ListStatementsCommand( AsyncModelPrivate*, AsyncResult* result, const Statement& );

            void execute( Model* );

        private:
            AsyncResult* m_result;

            AsyncModelPrivate* m_asyncModelPrivate;
        };


        class ListContextsCommand : public Command
        {
        public:
            ListContextsCommand( AsyncModelPrivate*, AsyncResult* result );

            void execute( Model* );

        private:
            AsyncResult* m_result;

            AsyncModelPrivate* m_asyncModelPrivate;
        };


        class ExecuteQueryCommand : public Command
        {
        public:
            ExecuteQueryCommand( AsyncModelPrivate*, AsyncResult* result, const QString&, Query::QueryLanguage lang, const QString& userQueryLang );

            void execute( Model* );

        private:
            AsyncResult* m_result;

            QString m_query;
            Query::QueryLanguage m_queryLanguage;
            QString m_userQueryLanguage;

            AsyncModelPrivate* m_asyncModelPrivate;
        };


        class CreateBlankNodeCommand : public Command
        {
        public:
            CreateBlankNodeCommand( AsyncResult* result );

            void execute( Model* );

        private:
            AsyncResult* m_result;
        };
    }
}

#endif
