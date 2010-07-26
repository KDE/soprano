/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2010 Sebastian Trueg <trueg@kde.org>
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

#ifndef _SOPRANO_N3_NODE_PARSER_H_
#define _SOPRANO_N3_NODE_PARSER_H_

#include <QtCore/QRegExp>
#include <QtCore/QHash>
#include <QtCore/QUrl>
#include <QtCore/QFlags>

#include "error.h"
#include "node.h"

namespace Soprano {

    class Node;

    /**
     * \brief Parser which converts N3 string representation of nodes into
     * Node objects.
     *
     * This class is NOT PUBLIC API and probably never will be.
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class N3NodeParser : public Error::ErrorCache
    {
    public:
        /**
         * Create a new N3 node parser
         */
        N3NodeParser();

        /**
         * Destructor
         */
        ~N3NodeParser();

        void setNamespacePrefixes( const QHash<QString, QUrl>& bnames );
        void addNamespacePrefix( const QString& abbr, const QUrl& ns );

        /**
         * Parse a node from its N3 representation.
         *
         * \param s The stream to read the node from.
         * \param flags An optional set of flags  which influence the behaviour of the
         * parser.
         *
         * \return \p The parsed node if it could be successfully parsed or an invalid
         * node otherwise. In the latter case Error::lastError() provides more details
         * on the parsing error. Be aware that the empty string will be treated as an
         * empty node and seen as a successful parse.
         */
        Soprano::Node parseNode( QTextStream& s, Node::N3ParserFlags flags = Node::NoFlags ) const;

    private:
        class Private;
        Private* const d;
    };
}

#endif
