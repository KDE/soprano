/*
 * This file is part of Soprano Project
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

#include "node.h"

#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtCore/QDebug>



class Soprano::Node::Private : public QSharedData
{
public:
    Private()
        : type(EmptyNode) {}

    Type type;
    QUrl uri;
    LiteralValue value;
    QString language;
};


Soprano::Node::Node()
{
    d = new Private;
}

Soprano::Node::Node( const Node &other )
{
    d = other.d;
}

Soprano::Node::Node( const QUrl &uri, Type type )
{
    d = new Private;
    if( !uri.isEmpty() &&
        type != LiteralNode &&
        type != EmptyNode ) {
        d->uri = uri;
        d->type = type;
    }
}

Soprano::Node::Node( const LiteralValue& value, const QString& lang )
{
    d = new Private;
    if ( value.isValid() ) {
        d->type = LiteralNode;
        d->value = value;
        d->language = lang;
    }
}

Soprano::Node::~Node()
{
}

bool Soprano::Node::isEmpty() const
{
    return ( d->type == Soprano::Node::EmptyNode );
}

bool Soprano::Node::isValid() const
{
    return !isEmpty();
}

bool Soprano::Node::isLiteral() const
{
    return ( d->type == Soprano::Node::LiteralNode );
}

bool Soprano::Node::isResource() const
{
    return ( d->type == Soprano::Node::ResourceNode );
}

bool Soprano::Node::isBlank() const
{
    return ( d->type == Soprano::Node::BlankNode );
}

Soprano::Node::Type Soprano::Node::type() const
{
    return d->type;
}

QUrl Soprano::Node::uri() const
{
    // d->uri is only defined for Resource and blank Nodes
    return d->uri;
}


Soprano::LiteralValue Soprano::Node::literal() const
{
    if ( isLiteral() ) {
        return d->value;
    }
    else {
        return LiteralValue();
    }
}

QUrl Soprano::Node::dataType() const
{
    if ( isLiteral() ) {
        return d->value.dataTypeUri();
    }
    else {
        return QUrl();
    }
}

QString Soprano::Node::language() const
{
    return d->language;
}

QString Soprano::Node::toString() const
{
    if ( isLiteral() ) {
        return d->value.toString();
    }
    else if ( isResource() || isBlank() ) {
        return d->uri.toString();
    }
    else {
        return QString();
    }
}

Soprano::Node& Soprano::Node::operator=( const Node& other )
{
    d = other.d;
    return *this;
}

Soprano::Node& Soprano::Node::operator=( const QUrl& resource )
{
    d->uri = resource;
    d->type = resource.isEmpty() ? EmptyNode : ResourceNode;
    d->value = LiteralValue();
    d->language.truncate( 0 );
    return *this;
}

Soprano::Node& Soprano::Node::operator=( const LiteralValue& literal )
{
    d->value = literal;
    d->uri = QUrl();
    d->type = literal.isValid() ? LiteralNode : EmptyNode;
    return *this;
}

bool Soprano::Node::operator==( const Node& other ) const
{
    if (d->type != other.d->type) {
        return false;
    }
    else if (d->type == ResourceNode) {
        return d->uri == other.d->uri;
    }
    else {
        return ( d->value == other.d->value &&
                 d->language == other.d->language );
    }
}

bool Soprano::Node::operator!=( const Node& other ) const
{
    if (d->type != other.d->type) {
        return true;
    }

    else if (d->type == ResourceNode) {
        return d->uri != other.d->uri;
    }
    else {
        return ( d->value != other.d->value ||
                 d->language != other.d->language );
    }
}

bool Soprano::Node::matches( const Node& other ) const
{
    if ( isEmpty() || other.isEmpty() ) {
        return true;
    }
    else {
        return operator==( other );
    }
}


QDebug operator<<( QDebug s, const Soprano::Node& n )
{
    switch(n.type()) {
    case Soprano::Node::EmptyNode:
        s.nospace() << "(empty)";
        break;
//     case Soprano::Soprano::Node::BlankNode:
//         s.nospace() << "(blank)";
//         break;
    case Soprano::Node::LiteralNode:
        s.nospace() << n.literal().toString();
        if( !n.language().isEmpty() )
            s.nospace() << " (" << n.language() << ")";
        break;
    default:
        s.nospace() << n.uri();
        break;
    }
    return s.space();
}


QTextStream& operator<<( QTextStream& s, const Soprano::Node& n )
{
    switch(n.type()) {
    case Soprano::Node::EmptyNode:
        s << "(empty)";
        break;
//     case Soprano::Soprano::Node::BlankNode:
//         s.nospace() << "(blank)";
//         break;
    case Soprano::Node::LiteralNode:
        s << '\"' << n.literal().toString() << "\"^^<" << n.literal().dataTypeUri().toString() << '>';
        if( !n.language().isEmpty() )
            s << " (" << n.language() << ")";
        break;
    default:
        s << '<' << n.uri().toString() << '>';
        if ( n.isBlank() ) {
            s << " (blank)";
        }
        break;
    }
    return s;
}
