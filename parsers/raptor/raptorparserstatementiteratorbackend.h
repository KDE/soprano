/*
 * This file is part of Soprano Project
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

#ifndef _SOPRANO_RATOR_PARSER_STATEMENT_IT_BACKEND_H_
#define _SOPRANO_RATOR_PARSER_STATEMENT_IT_BACKEND_H_

#include "iteratorbackend.h"
#include "statement.h"

#include <raptor.h>

#include <QtCore/QTextStream>
#include <QtCore/QQueue>


namespace Soprano {
    namespace Raptor {

        class ParserWrapper;

        /**
         * iterator backend which calls raptor_parse_chunk on an already initialized
         * raptor_parser (initialized by ParserWrapper) to fill a queue whenever next()
         * is called.
         * The internal structures are automatically cleaned up once the parsing is
         * done or the iterator is closed.
         *
         * \author Sebastian Trueg <trueg@kde.org>
         */
        class ParserStatementIteratorBackend : public IteratorBackend<Soprano::Statement>
        {
        public:
            ParserStatementIteratorBackend( QTextStream* s, bool deleteStream, ParserWrapper* parser );
            ParserStatementIteratorBackend( QIODevice* file, bool deleteDevice, ParserWrapper* parser );
            ParserStatementIteratorBackend( const QString&, ParserWrapper* parser );
            ~ParserStatementIteratorBackend();

            bool next();
            Statement current() const;
            void close();
            
        private:
            void initStatementHandler();

            static void raptorTriplesHandler( void* userData, const raptor_statement* triple );
            static void raptorGraphHandler( void* userData, raptor_uri* graph );

            QQueue<Statement> m_parsedStatementsQueue;
            Node m_currentContext;

            ParserWrapper* m_parser;
            QTextStream* m_stream;
            QIODevice* m_ioDevice;
            QString m_dataString;

            bool m_deleteSource;
        };
    }
}

#endif
