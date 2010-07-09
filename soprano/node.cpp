/*
 * This file is part of Soprano Project
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

#include "node.h"

#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtCore/QDebug>



class Soprano::Node::NodeData : public QSharedData
{
public:
    NodeData() {}
    virtual ~NodeData() {}

    virtual Type type() const = 0;
    virtual QString toString() const = 0;
    virtual QString toN3() const = 0;
};

class Soprano::Node::ResourceNodeData : public NodeData
{
public:
    ResourceNodeData( const QUrl& uri_ = QUrl() )
        : NodeData(),
          uri( uri_ ){
    }

    QUrl uri;

    Type type() const { return ResourceNode; }

    QString toString() const {
        return uri.toString();
    }

    QString toN3() const {
        return Node::resourceToN3( uri );
    }
};

class Soprano::Node::BNodeData : public NodeData
{
public:
    BNodeData( const QString& id = QString() )
        : NodeData(),
          identifier( id ) {
    }

    QString identifier;

    Type type() const { return BlankNode; }

    QString toString() const {
        return identifier;
    }

    QString toN3() const {
        return Node::blankToN3( identifier );
    }
};

class Soprano::Node::LiteralNodeData : public NodeData
{
public:
    LiteralNodeData( const LiteralValue& val = LiteralValue() )
        : NodeData(),
          value( val )
    {
    }

    LiteralValue value;

    Type type() const { return LiteralNode; }

    QString toString() const {
        return value.toString();
    }

    QString toN3() const {
        return Node::literalToN3( value );
    }
};


Soprano::Node::Node()
{
    d = 0;
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
        d = 0;
    }
}

Soprano::Node::Node( const QString &id )
{
    if( !id.isEmpty() ) {
        d = new BNodeData( id );
    }
    else {
        d = 0;
    }
}

Soprano::Node::Node( const LiteralValue& value )
{
    if ( value.isValid() ) {
        d = new LiteralNodeData( value );
    }
    else {
        d = 0;
    }
}

Soprano::Node::Node( const LiteralValue& value, const QString& lang )
{
    if ( value.isValid() ) {
        LiteralValue lit;
        if ( lang.isEmpty() ) {
            lit = value;
        } else {
            lit = LiteralValue::createPlainLiteral( value.toString(), lang );
        }
        d = new LiteralNodeData( lit );
    }
    else {
        d = 0;
    }
}

Soprano::Node::~Node()
{
}

bool Soprano::Node::isEmpty() const
{
    return ( !d || d->type() == Soprano::Node::EmptyNode );
}

bool Soprano::Node::isValid() const
{
    return !isEmpty();
}

bool Soprano::Node::isLiteral() const
{
    return ( d && d->type() == Soprano::Node::LiteralNode );
}

bool Soprano::Node::isResource() const
{
    return ( d && d->type() == Soprano::Node::ResourceNode );
}

bool Soprano::Node::isBlank() const
{
    return ( d && d->type() == Soprano::Node::BlankNode );
}

Soprano::Node::Type Soprano::Node::type() const
{
    return d ? d->type() : EmptyNode;
}

QUrl Soprano::Node::uri() const
{
    if ( isResource() ) {
        return static_cast<const ResourceNodeData*>( d.constData() )->uri;
    }
    else {
        return QUrl();
    }
}


QString Soprano::Node::identifier() const
{
    if ( isBlank() ) {
        return static_cast<const BNodeData*>( d.constData() )->identifier;
    }
    else {
        return QString();
    }
}


Soprano::LiteralValue Soprano::Node::literal() const
{
    if ( isLiteral() ) {
        return static_cast<const LiteralNodeData*>( d.constData() )->value;
    }
    else {
        return LiteralValue();
    }
}

QUrl Soprano::Node::dataType() const
{
    if ( isLiteral() ) {
        return static_cast<const LiteralNodeData*>( d.constData() )->value.dataTypeUri();
    }
    else {
        return QUrl();
    }
}

QString Soprano::Node::language() const
{
    return literal().language().toString();
}

QString Soprano::Node::toString() const
{
    return d ? d->toString() : QString();
}

QString Soprano::Node::toN3() const
{
    return d ? d->toN3() : QString();
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
        d = 0;
    }
    return *this;
}

Soprano::Node& Soprano::Node::operator=( const LiteralValue& literal )
{
    if ( literal.isValid() ) {
        d = new LiteralNodeData( literal );
    }
    else {
        d = 0;
    }
    return *this;
}

bool Soprano::Node::operator==( const Node& other ) const
{
    if ( type() != other.type() ) {
        return false;
    }
    else if ( type() != EmptyNode ) {
        if ( d->type() == ResourceNode ) {
            return(  static_cast<const ResourceNodeData*>( d.constData() )->uri ==
                     static_cast<const ResourceNodeData*>( other.d.constData() )->uri );
        }
        else if ( d->type() == BlankNode ) {
            return( static_cast<const BNodeData*>( d.constData() )->identifier ==
                    static_cast<const BNodeData*>( other.d.constData() )->identifier );
        }
        else if ( d->type() == LiteralNode ) {
            return ( static_cast<const LiteralNodeData*>( d.constData() )->value ==
                     static_cast<const LiteralNodeData*>( other.d.constData() )->value );
        }
    }

    // empty nodes are always equal
    return true;
}

bool Soprano::Node::operator!=( const Node& other ) const
{
    if ( type() != other.type() ) {
        return true;
    }

    else if ( type() == ResourceNode ) {
        return(  static_cast<const ResourceNodeData*>( d.constData() )->uri
                 != static_cast<const ResourceNodeData*>( other.d.constData() )->uri );
    }
    else if ( type() == BlankNode ) {
        return( static_cast<const BNodeData*>( d.constData() )->identifier !=
                static_cast<const BNodeData*>( other.d.constData() )->identifier );
    }
    else if ( type() == LiteralNode ) {
        return ( static_cast<const LiteralNodeData*>( d.constData() )->value !=
                 static_cast<const LiteralNodeData*>( other.d.constData() )->value );
    }
    else {
        // empty nodes are always equal
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
    // other is a template
    if ( other.isEmpty() ) {
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

Soprano::Node Soprano::Node::createLiteralNode( const LiteralValue& val )
{
    return Node( val );
}

Soprano::Node Soprano::Node::createLiteralNode( const LiteralValue& val, const QString& language )
{
    return Node( LiteralValue::createPlainLiteral( val.toString(), language ) );
}


QString Soprano::Node::resourceToN3( const QUrl& uri )
{
    QByteArray a = uri.toEncoded();
    return '<' + QString::fromAscii( a ) + '>';
}


QString Soprano::Node::blankToN3( const QString& blank )
{
    if ( blank.isEmpty() )
        return blank;
    else
        return "_:" + blank;
}


QString Soprano::Node::literalToN3( const LiteralValue& literal )
{
    //
    // Escape control chars:  \t \b \n \r \f \\ \" \'
    //
    QString s = literal.toString();

    // FIXME: this can be done faster by running through the string only once.
    s.replace( '\\', "\\\\" );
    s.replace( '\"', "\\\"" );
    s.replace( '\'', "\\\'" );
    s.replace( '\n', "\\n" );
    s.replace( '\r', "\\r" );
    s.replace( '\b', "\\b" );
    s.replace( '\t', "\\t" );
    s.replace( '\f', "\\f" );

    if( literal.isPlain() ) {
        if ( literal.language().isEmpty() ) {
            return '\"' + s + '\"';
        }
        else {
            return '\"' + s + "\"@" + literal.language().toString();
        }
    }
    else {
        return QString( "\"%1\"^^<%2>" )
            .arg( s, QString::fromAscii( literal.dataTypeUri().toEncoded() ) );
    }
}


QDebug operator<<( QDebug s, const Soprano::Node& n )
{
    switch( n.type() ) {
    case Soprano::Node::EmptyNode:
        s.nospace() << "(empty)";
        break;
    default:
        s.nospace() << n.toN3();
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
    default:
        s << n.toN3();
        break;
    }
    return s;
}


uint Soprano::qHash( const Soprano::Node& node )
{
    uint hashVal;
    switch ( node.type() ) {
    case Soprano::Node::EmptyNode:
        hashVal = 0;
        break;
    case Soprano::Node::ResourceNode:
        hashVal = qHash( node.uri() );
        break;
    case Soprano::Node::LiteralNode:
        hashVal = qHash( node.literal() );
        break;
    case Soprano::Node::BlankNode:
        hashVal = qHash( node.identifier() );
        break;
    default:
        // Should never get here
        hashVal = 0;
        break;
    }

    // Rotate bits so as to further distinguish node types with the
    // same string
    uint typeInt( ((uint)node.type()) & 0x1F );
    hashVal = (hashVal << typeInt) | (hashVal >> (0x20 - typeInt));

    return hashVal;
}


namespace Soprano {
    // see qhashqurlcompat.cpp
    uint soprano_qHash_QUrl(const QUrl &url)
    {
#if QT_VERSION < 0x040602
        // implementation taken from KUrl by David Faure
        return qHash(url.scheme()) ^ qHash(url.path()) ^ qHash(url.fragment()) ^ qHash(url.encodedQuery());
#else
        // implementation from Qt 4.7 to ease transition
        return qHash(url.toEncoded(QUrl::FormattingOption(0x100)));
#endif
    }
}
