/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007 Daniele Galdi <daniele.galdi@gmail.com>
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

namespace Soprano {
    namespace Query {
        namespace Parser {
        
            class QueryParser {
            public:

                /**
                 * Translate the given query into a proper Soprano query-object or NULL on Error.
                 */
                virtual QueryObject *parseQuery( const QString &query ) = 0;

            //public signal:
                //void syntaxErrorFounded(  );

            };
        };
    };
};

#endif // SOPRANO_QUERY_QUERYPARSER_H
