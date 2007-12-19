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



class Soprano::Node::NodeData : public QSharedData
{
public:
    NodeData( Type type_ = EmptyNode )
        : type( type_ ) {}

    virtual ~NodeData() {}

    Type type;

    virtual QString toString() const {
        return QString();
    }
};

class Soprano::Node::ResourceNodeData : public NodeData
{
public:
    ResourceNodeData( const QUrl& uri_ = QUrl() )
        : NodeData( ResourceNode ),
          uri( uri_ ){
    }

    QUrl uri;

    QString toString() const {
        return uri.toString();
    }
};

class Soprano::Node::BNodeData : public NodeData
{
public:
    BNodeData( const QString& id = QString() )
        : NodeData( BlankNode ),
          identifier( id ) {
    }

    QString identifier;

    QString toString() const {
        return identifier;
    }
};

class Soprano::Node::LiteralNodeData : public NodeData
{
public:
    LiteralNodeData( const LiteralValue& val = LiteralValue(), const QString& lang = QString() )
        : NodeData( LiteralNode ),
          value( val ),
          language( lang ) {
    }

    LiteralValue value;
    QString language;

    QString toString() const {
        return value.toString();
    }
};


Soprano::Node::Node()
{
    d = new NodeData();
}

Soprano::Node::Node( const Node &other )
{
    d = other.d;
}

Soprano::Node::Node( const QUrl &uri )
{
    if( !uri.isEmpty() ) {
        d = new ResourceNodeData( uri );
    }
    else {
        d = new NodeData();
    }
}

Soprano::Node::Node( const QString &id )
{
    if( !id.isEmpty() ) {
        d = new BNodeData( id );
    }
    else {
        d = new NodeData();
    }
}

Soprano::Node::Node( const LiteralValue& value, const QString& lang )
{
    if ( value.isValid() ) {
        d = new LiteralNodeData( value, lang );
    }
    else {
        d = new NodeData();
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
    if ( const ResourceNodeData* rnd = dynamic_cast<const ResourceNodeData*>( d.constData() ) ) {
        return rnd->uri;
    }
    else {
        return QUrl();
    }
}


QString Soprano::Node::identifier() const
{
    if ( const BNodeData* bnd = dynamic_cast<const BNodeData*>( d.constData() ) ) {
        return bnd->identifier;
    }
    else {
        return QString();
    }
}


Soprano::LiteralValue Soprano::Node::literal() const
{
    if ( const LiteralNodeData* lnd = dynamic_cast<const LiteralNodeData*>( d.constData() ) ) {
        return lnd->value;
    }
    else {
        return LiteralValue();
    }
}

QUrl Soprano::Node::dataType() const
{
    if ( const LiteralNodeData* lnd = dynamic_cast<const LiteralNodeData*>( d.constData() ) ) {
        return lnd->value.dataTypeUri();
    }
    else {
        return QUrl();
    }
}

QString Soprano::Node::language() const
{
    if ( const LiteralNodeData* lnd = dynamic_cast<const LiteralNodeData*>( d.constData() ) ) {
        return lnd->language;
    }
    else {
        return QString();
    }
}

QString Soprano::Node::toString() const
{
    return d->toString();
}

Soprano::Node& Soprano::Node::operator=( const Node& other )
{
    d = other.d;
    return *this;
}

Soprano::Node& Soprano::Node::operator=( const QUrl& resource )
{
    if ( !resource.isEmpty() ) {
        d = new ResourceNodeData( resource );
    }
    else {
        d = new NodeData();
    }
    return *this;
}

Soprano::Node& Soprano::Node::operator=( const LiteralValue& literal )
{
    if ( literal.isValid() ) {
        d = new LiteralNodeData( literal );
    }
    else {
        d = new NodeData();
    }
    return *this;
}

bool Soprano::Node::operator==( const Node& other ) const
{
    if ( d->type != other.d->type ) {
        return false;
    }
    else if ( d->type == ResourceNode ) {
        return(  dynamic_cast<const ResourceNodeData*>( d.constData() )->uri ==
                 dynamic_cast<const ResourceNodeData*>( other.d.constData() )->uri );
    }
    else if ( d->type == BlankNode ) {
        return( dynamic_cast<const BNodeData*>( d.constData() )->identifier ==
                dynamic_cast<const BNodeData*>( other.d.constData() )->identifier );
    }
    else if ( d->type == LiteralNode ) {
        return ( dynamic_cast<const LiteralNodeData*>( d.constData() )->value ==
                 dynamic_cast<const LiteralNodeData*>( other.d.constData() )->value &&
                 dynamic_cast<const LiteralNodeData*>( d.constData() )->language ==
                 dynamic_cast<const LiteralNodeData*>( other.d.constData() )->language );
    }
    else {
        // emppty nodes are always equal
        return true;
    }
}

bool Soprano::Node::operator!=( const Node& other ) const
{
    if ( d->type != other.d->type ) {
        return true;
    }

    else if ( d->type == ResourceNode ) {
        return(  dynamic_cast<const ResourceNodeData*>( d.constData() )->uri
                 != dynamic_cast<const ResourceNodeData*>( other.d.constData() )->uri );
    }
    else if ( d->type == BlankNode ) {
        return( dynamic_cast<const BNodeData*>( d.constData() )->identifier !=
                dynamic_cast<const BNodeData*>( other.d.constData() )->identifier );
    }
    else if ( d->type == LiteralNode ) {
        return ( dynamic_cast<const LiteralNodeData*>( d.constData() )->value !=
                 dynamic_cast<const LiteralNodeData*>( other.d.constData() )->value ||
                 dynamic_cast<const LiteralNodeData*>( d.constData() )->language !=
                 dynamic_cast<const LiteralNodeData*>( other.d.constData() )->language );
    }
    else {
        // emppty nodes are always equal
        return false;
    }
}

bool Soprano::Node::operator==( const QUrl& other ) const
{
    return uri() == other;
}

bool Soprano::Node::operator==( const LiteralValue& other ) const
{
    return literal() == other;
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


Soprano::Node Soprano::Node::createEmptyNode()
{
    return Node();
}


Soprano::Node Soprano::Node::createResourceNode( const QUrl& uri )
{
    return Node( uri );
}


Soprano::Node Soprano::Node::createBlankNode( const QString& id )
{
    return Node( id );
}


Soprano::Node Soprano::Node::createLiteralNode( const LiteralValue& val, const QString& language )
{
    return Node( val, language );
}


QDebug operator<<( QDebug s, const Soprano::Node& n )
{
    switch( n.type() ) {
    case Soprano::Node::EmptyNode:
        s.nospace() << "(empty)";
        break;
//     case Soprano::Soprano::Node::BlankNode:
//         s.nospace() << "(blank)";
//         break;
    case Soprano::Node::LiteralNode:
        s.nospace() << '\"' << n.literal().toString() << "\"";
        if ( n.literal().isString() && !n.language().isEmpty() ) {
            s.nospace() << "@" << n.language();
        }
        else {
            s.nospace() << "^^<" << n.literal().dataTypeUri().toString() << '>';
        }
        break;
    case Soprano::Node::BlankNode:
        s.nospace() << "_:" << n.identifier();
        break;
    default:
        s.nospace() << n.uri().toString();
        break;
    }
    return s;
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
        s << '\"' << n.literal().toString() << "\"";
        if ( n.literal().isString() && !n.language().isEmpty() ) {
            s << "@" << n.language();
        }
        else {
            s << "^^<" << n.literal().dataTypeUri().toString() << '>';
        }
        break;
    case Soprano::Node::BlankNode:
        s << "_:" << n.identifier();
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
