/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2006-2007 Daniele Galdi <daniele.galdi@gmail.com>
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

#ifndef SOPRANO_NODE_H
#define SOPRANO_NODE_H

#include <QtCore/QUrl>
#include <QtCore/QSharedDataPointer>
#include <QtCore/QTextStream>

#include "soprano_export.h"
#include "literalvalue.h"

namespace Soprano
{
    /**
     * \class Node node.h Soprano/Node
     *
     * \brief A Node represents one RDF resource.
     *
     * \author Daniele Galdi <daniele.galdi@gmail.com><br>Sebastian Trueg <trueg@kde.org>
     */
    class SOPRANO_EXPORT Node
	{
	public:

	    enum Type {
		EmptyNode    = 0, /**< An empty node, can be used as a placeholder in queries. */
		ResourceNode = 1, /**< rdf:Resource (& rdf:Property) - has a URI */
		LiteralNode  = 2, /**< rdf:Literal - has an XML string, language, XML space */
		BlankNode    = 3  /**< blank node has an identifier string */
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
	     * \param language The language of the literal value.
	     *
	     * \sa createLiteralNode()
	     */
	    Node( const LiteralValue& value,
		  const QString& language = QString() );

	    Node( const Node &other );

	    ~Node();
	    //@}

	    /**
	     * \name Operators
	     */
	    //@{
	    Node& operator=( const Node& other );

	    /**
	     * Assigns resource to this node and makes it a ResourceNode.
	     */
	    Node& operator=( const QUrl& resource );

	    Node& operator=( const LiteralValue& literal );

	    bool operator==( const Node& other ) const;
	    bool operator!=( const Node& other ) const;

	    /**
	     * Match this node against other. The only difference
	     * to operator== is that empty nodes are matched as wildcards,
	     * i.e. they match any other node.
	     *
	     * \return true if this node matches other, false if not.
	     */
	    bool matches( const Node& other ) const;
	    //@}

	    /**
	     * \name Type information
	     */
	    //@{
	    /**
	     * \return The Node type.
	     */
	    Type type() const;

	    /**
	     * \return true if the Node is empty.
	     */
	    bool isEmpty() const;

	    /**
	     * \return true if the Node is a Resource,Literal or Blank.
	     */
	    bool isValid() const ;

	    /**
	     * \return true if the Node is a Literal.
	     */
	    bool isLiteral() const;

	    /**
	     * \return true if the Node is a Resource.
	     */
	    bool isResource() const;

	    /**
	     * \return true if the Node is a Blank node (anonymous).
	     */
	    bool isBlank() const;
	    //@}

	    /**
	     * \name Resource nodes
	     */
	    //@{
	    /**
	     * \return The URI if the node is a Resource node.
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
	     * \return The node's identifier if it is a blank node, a null
	     * string otherwise.
	     */
	    QString identifier() const;
	    //@}

	    /**
	     * \name Literal nodes
	     */
	    //@{
	    /**
	     * \return The Literal value if the node is a Literal node.
	     *         An null QString otherwise.
	     */
	    LiteralValue literal() const;

	    /**
	     * \return The datatype URI of a literal node, i.e. the XML schema type
	     *         or an empty value if the node is not a literal.
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
	     */
	    QString language() const;
	    //@}

	    //@{
	    /**
	     *\return A String representation of the Node.
	     */
	    QString toString() const;
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
	     * \param language The language of the literal value.
	     *
	     * \return A literal node or an empty node if the specified value
	     * was empty.
	     */
	    static Node createLiteralNode( const LiteralValue& value, const QString& language );

	private:
	    class NodeData;
	    class ResourceNodeData;
	    class BNodeData;
	    class LiteralNodeData;
	    QSharedDataPointer<NodeData> d;
	};

}

SOPRANO_EXPORT QDebug operator<<( QDebug s, const Soprano::Node& );

/**
 * Default Soprano::Node stream operator. The operator serializes the Node
 * based on the N-Triples standard, except that it uses Unicode strings.
 */
SOPRANO_EXPORT QTextStream& operator<<( QTextStream& s, const Soprano::Node& );

#endif // SOPRANO_NODE_H
