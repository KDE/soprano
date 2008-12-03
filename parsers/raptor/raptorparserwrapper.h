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

#ifndef _SOPRANO_RAPTOR_PARSER_WRAPPER_H_
#define _SOPRANO_RAPTOR_PARSER_WRAPPER_H_

#include "error.h"
#include "sopranotypes.h"

#include <raptor.h>

#include <QtCore/QUrl>


namespace Soprano {
    namespace Raptor {
        /**
         * A wrapper class around raptor_parser. It takes care of properly initializing
         * and shutting down the raptor lib (thread-safe). Error messages from raptor
         * are converted to Soprano::Error::Error instances and can be accessed via
         * Error::ErrorCache::lastError().
         *
         * \author Sebastian Trueg <trueg@kde.org>
         */
        class ParserWrapper : public Error::ErrorCache
        {
        public:
            ~ParserWrapper();

            void setRaptorError( const Error::Error& );

            raptor_parser* parser() const { return m_parser; }

            static ParserWrapper* createParser( const QUrl& baseUri, 
                                                RdfSerialization serialization,
                                                const QString& userSerialization );

        private:
            ParserWrapper( raptor_parser* parser, raptor_uri* baseUri );
            void grabRaptorMessages( raptor_parser* parser );
            static void raptorMessageHandler( void* userData, raptor_locator* locator, const char* message );

            raptor_parser* m_parser;
            raptor_uri* m_raptorBaseUri;
        };
    }
}

#endif
