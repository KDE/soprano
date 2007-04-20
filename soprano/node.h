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
#include <soprano/soprano_export.h>

namespace Soprano
{

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
	     * Default costructor.
	     * Creates an empty node.
	     */
	    Node();

	    /**
	     * Creates a resource or blank node.
	     *
	     * \param uri The URI of the node. If empty the type will be ignored
	     *            and an empty node will be created.
	     * \param type ResourceNode or BlankNode. Otherwise an empty node will
	     *             be created.
	     */
	    explicit Node( const QUrl &uri, Type type = ResourceNode );

	    /**
	     * Creates a literal node.
	     *
	     * \param value
	     *        The value of a node. Represent a uri string if
	     *        if type is Blank or Resource.
	     *
	     * \param datatype The type URI of the literal value.
	     *
	     * \param language The language of the literal value.
	     *
	     * Caution: This constructor allows to create Nodes with
	     *          empty uris or values.
	     */
	    explicit Node( const QString &value,
			   const QUrl& datatype = QUrl(),
			   const QString& language = QString() );

	    Node( const Node &other );

	    virtual ~Node();

	    Node &operator=( const Node& other );
	    bool operator==( const Node& other ) const;

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

	    /**
	     * \return The URI if the node is a Resource or blank node.
	     *         An null QUrl otherwise.
	     */
	    QUrl uri() const;

	    /**
	     * \return The Literal value if the node is a Literal node.
	     *         An null QString otherwise.
	     */
	    QString literal() const;

	    /**
	     * \return The datatype URI of a literal node, i.e. the XML schema type
	     *         or an empty value if the node is not a literal.
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

	    /**
	     *\return A String representation of the Node.
	     */
	    QString toString() const;

	private:
	    class Private;
	    QSharedDataPointer<Private> d;
	};

}

SOPRANO_EXPORT QDebug operator<<( QDebug s, const Soprano::Node& );

#endif // SOPRANO_NODE_H
