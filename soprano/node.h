/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2006-2007 Daniele Galdi <daniele.galdi@gmail.com>
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

#ifndef SOPRANO_NODE_H
#define SOPRANO_NODE_H

#include <QtCore/QUrl>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QTextStream>

#include "soprano_export.h"
#include "literalvalue.h"
#include "languagetag.h"
#include "sopranomacros.h"

namespace Soprano
{
    /**
     * \class Node node.h Soprano/Node
     *
     * \brief A Node represents one RDF resource.
     *
     * Nodes are the cornerstone of RDF data in %Soprano. Four Nodes form one Statement and a Model
     * is essentially a set of Statements.
     *
     * A Node can have one of four types: EmptyNode, ResourceNode, LiteralNode, and BlankNode.
     * Resource nodes are identified through their URI (uri()), literal nodes have a LiteralValue (literal()),
     * and blank nodes have a string identifier.
     *
     * Empty nodes can be used as wildcards in methods such as Model::listStatements.
     *
     * \author Daniele Galdi <daniele.galdi@gmail.com><br>Sebastian Trueg <trueg@kde.org>
     */
    class SOPRANO_EXPORT Node
    {
    public:
        enum Type {
            EmptyNode    = 0, /**< An empty node, can be used as a wildcard in commands like Model::listStatements. */
            ResourceNode = 1, /**< A resource node has a URI which can be accessed via uri() */
            LiteralNode  = 2, /**< A literal node has a literal value and an optional language. */
            BlankNode    = 3  /**< A blank node has an identifier string */
        };

        /**
         * \name Constructors
         */
        //@{
        /**
         * Default costructor.
         * Creates an empty node.
         *
         * \sa createEmptyNode()
         */
        Node();

        // This constructor is non-explicit for a reason: it makes creating
        // Statements much much easier and more readable
        /**
         * Creates a resource node.
         *
         * \param uri The URI of the node. If empty the type will be ignored
         *            and an empty node will be created.
         *
         * \sa createResourceNode()
         */
        Node( const QUrl &uri );

        /**
         * Creates a blank node.
         *
         * \param id An identifier for the blank node.
         *
         * \sa createBlankNode()
         */
        explicit Node( const QString& id );

        /**
         * Creates a literal node.
         *
         * \param value The value of a node. If empty the node will become
         *              an empty node.
         *
         * \sa createLiteralNode()
         *
         * \since 2.3
         */
        Node( const LiteralValue& value );

        /**
         * Creates a literal node.
         *
         * \param value The value of a node. If empty the node will become
         *              an empty node.
         * \param language The language of the literal value.
         *
         * \sa createLiteralNode()
         *
         * \deprecated Use Soprano::Node::Node( const LiteralValue& ) and
         *             Soprano::LiteralValue::createPlainLiteral( const QString&, const LanguageTag& )
         */
        SOPRANO_CONSTRUCTOR_DEPRECATED Node( const LiteralValue& value,
                                             const QString& language );

        /**
         * Copy constructor.
         */
        Node( const Node &other );

        ~Node();
        //@}

        /**
         * \name Operators
         */
        //@{
        Node& operator=( const Node& other );

        /**
         * Assigns \p resource to this node and makes it a ResourceNode.
         */
        Node& operator=( const QUrl& resource );

        /**
         * Assigns \p literal to this node and makes it a LiteralNode.
         */
        Node& operator=( const LiteralValue& literal );

        /**
         * Comparision operator.
         * \return \p true if this node and \p other are equal.
         */
        bool operator==( const Node& other ) const;

        /**
         * Comparision operator.
         * \return \p true if this node and \p other differ.
         */
        bool operator!=( const Node& other ) const;

        /**
         * Comparision operator.
         * \return \p true if this node is a ResourceNode and
         * has URI \p uri.
         */
        bool operator==( const QUrl& uri ) const;

        /**
         * Comparision operator.
         * \return \p true if this node is a LiteralNode and
         * has literal value \p other.
         */
        bool operator==( const LiteralValue& other ) const;

        /**
         * Match this node against template node \a other. The only difference
         * to operator== is that empty nodes are matched as wildcards,
         * i.e. they match any other node.
         *
         * Be aware that the following is NOT always true since only \a other
         * is treated a a wildcard:
         *
         * \code
         * // NOT always true:
         * a.matches(b) == b.matches(a)
         * \endcode
         *
         * \return \p true if this node matches other, \p false if not.
         *
         * \sa Statement::matches
         */
        bool matches( const Node& other ) const;
        //@}

        /**
         * \name Type information
         */
        //@{
        /**
         * \return The node type.
         */
        Type type() const;

        /**
         * \return \p true if the node is empty.
         */
        bool isEmpty() const;

        /**
         * \return \p true if the node is a ResourceNode, LiteralNode or BlankNode.
         */
        bool isValid() const ;

        /**
         * \return \p true if the node is a LiteralNode.
         */
        bool isLiteral() const;

        /**
         * \return \p true if the node is a ResourceNode.
         */
        bool isResource() const;

        /**
         * \return \p true if the node is a BlankNode (anonymous).
         */
        bool isBlank() const;
        //@}

        /**
         * \name Resource nodes
         */
        //@{
        /**
         * \return The URI if the node is a ResourceNode.
         *         An null QUrl otherwise.
         */
        QUrl uri() const;
        //@}

        /**
         * \name Blank nodes
         */
        //@{
        /**
         * Retrieve a blank node's identifier.
         * \return The node's identifier if it is a BlankNode, a null
         * string otherwise.
         */
        QString identifier() const;
        //@}

        /**
         * \name Literal nodes
         */
        //@{
        /**
         * \return The literal value if the node is a LiteralNode.
         *         An null QString otherwise.
         */
        LiteralValue literal() const;

        /**
         * \return The datatype URI of a literal node, i.e. the XML schema type
         *         or an empty value if the node is not a LiteralNode.
         * \sa LiteralValue::dataTypeUri
         */
        QUrl dataType() const;

        /**
         * Each literal value can have an associated language, thus each property
         * can be stored for different languages. An empty language refers to the
         * default language.
         *
         * \return A string representing the language of the literal value
         *         or an empty string if the node is not a literal.
         *
         * \deprecated Language exists on the Soprano::LiteralValue. Use Soprano::Node::literal() and
         *             Soprano::LiteralValue::language().
         */
        QString language() const;
        //@}

        /**
         * \name Conversion
         */
        //@{
        /**
         * Converts the Node to a string.
         *
         * \return A String representation of the Node, suitable for storage,
         * not really suitable for user readable strings.
         *
         * \sa LiteralValue::toString(), QUrl::toString(), toN3()
         */
        QString toString() const;

        /**
         * Convert a Node into N3 notation to be used in SPARQL graph patterns.
         *
         * Examples:
         * \code
         * <http://soprano.sourceforce.net/>
         * "Hello World"^^<http://www.w3.org/2001/XMLSchema#string>
         * "09-08-1977T17:42.234Z"^^<http://www.w3.org/2001/XMLSchema#dateTime>
         * _:blankNode
         * \endcode
         *
         * \return A string representing the node in N3 encoding or an empty
         * string for invalid nodes.
         *
         * \sa toString()
         *
         * \since 2.2
         */
        QString toN3() const;
        //@}

        /**
         * Convenience method to create an empty node.
         * Using this method instead of the default constructor
         * may result in better readable code.
         *
         * \return An empty Node.
         */
        static Node createEmptyNode();

        /**
         * Convenience method to create a resource node.
         * Using this method instead of the constructor
         * may result in better readable code.
         *
         * \param uri The URI of the node. If empty the type will be ignored
         *            and an empty node will be created.
         *
         * \return A resource Node or an empty Node if the specified URI is empty.
         */
        static Node createResourceNode( const QUrl& uri );

        /**
         * Convenience method to create a blank node.
         * Using this method instead of the constructor
         * may result in better readable code.
         *
         * If you need to create a new blank node which is not
         * used in the model yet and, thus, has a unique identifier
         * see Model::createBlankNode().
         *
         * \param id An identifier for the blank node.
         *
         * \return A blank node or an empty Node if the specified
         * identifier was empty.
         */
        static Node createBlankNode( const QString& id );

        /**
         * Convenience method to create a literal node.
         * Using this method instead of the constructor
         * may result in better readable code.
         *
         * \param value The value of a node. If empty the node will become
         *              an empty node.
         *
         * \return A literal node or an empty node if the specified value
         * was empty.
         *
         * \since 2.3
         */
        static Node createLiteralNode( const LiteralValue& value );

        /**
         * Convenience method to create a literal node.
         * Using this method instead of the constructor
         * may result in better readable code.
         *
         * \param value The value of a node. If empty the node will become
         *              an empty node.
         * \param language The language of the literal value.
         *
         * \return A literal node or an empty node if the specified value
         * was empty.
         *
         * \deprecated Use Soprano::Node::createLiteralNode( const LiteralValue& ) and
         *             Soprano::LiteralValue::createPlainLiteral( const QString&, const LanguageTag& )
         */
        static SOPRANO_DEPRECATED Node createLiteralNode( const LiteralValue& value, const QString& language );

        /**
         * Format a resource URI as N3 string to be used in SPARQL queries.
         *
         * \return A string representing the resource in N3 encoding or an empty
         * string for invalid URIs.
         *
         * Example:
         * \code
         * <http://soprano.sourceforce.net/>
         * \endcode
         *
         * \sa toN3
         *
         * \since 2.3
         */
        static QString resourceToN3( const QUrl& resource );

        /**
         * Format a blank node identifier as N3 string to be used in SPARQL queries.
         *
         * \return A string representing the blank identifier in N3 encoding or an empty
         * string for invalid/empty ids.
         *
         * Example:
         * \code
         * _:blankNode
         * \endcode
         *
         * \sa toN3
         *
         * \since 2.3
         */
        static QString blankToN3( const QString& blank );

        /**
         * Format a literal value as N3 string to be used in SPARQL queries.
         *
         * \return A string representing the literal in N3 encoding or an empty
         * string for invalid literals.
         *
         * Examples:
         * \code
         * "Hello World"^^<http://www.w3.org/2001/XMLSchema#string>
         * "09-08-1977T17:42.234Z"^^<http://www.w3.org/2001/XMLSchema#dateTime>
         * \endcode
         *
         * \sa toN3
         *
         * \since 2.3
         */
        static QString literalToN3( const LiteralValue& literal );

    private:
        class NodeData;
        class ResourceNodeData;
        class BNodeData;
        class LiteralNodeData;
        QSharedDataPointer<NodeData> d;
    };

    /**
     * \relates Soprano::Node
     */
    SOPRANO_EXPORT uint qHash( const Node& node );
}

/**
 * \relates Soprano::Node
 */
SOPRANO_EXPORT QDebug operator<<( QDebug s, const Soprano::Node& );

/**
 * Default Soprano::Node stream operator. The operator serializes the Node
 * based on the N-Triples standard, except that it uses Unicode strings.
 *
 * \sa Soprano::Node::toN3()
 *
 * \relates Soprano::Node
 */
SOPRANO_EXPORT QTextStream& operator<<( QTextStream& s, const Soprano::Node& );

#if QT_VERSION < 0x040700
SOPRANO_EXPORT uint qHash( const QUrl& url );
#endif

#endif // SOPRANO_NODE_H
