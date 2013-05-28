/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2006-2007 Daniele Galdi <daniele.galdi@gmail.com>
 * Copyright (C) 2007-2009 Sebastian Trueg <trueg@kde.org>
 * Copyright (C) 2012 Vishesh Handa <me@vhanda.in>
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
#include "n3nodeparser.h"

#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtCore/QDebug>
#include <QMutex>



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


// static
Soprano::Node Soprano::Node::createEmptyNode()
{
    return Node();
}


// static
Soprano::Node Soprano::Node::createResourceNode( const QUrl& uri )
{
    return Node( uri );
}


// static
Soprano::Node Soprano::Node::createBlankNode( const QString& id )
{
    return Node( id );
}


// static
Soprano::Node Soprano::Node::createLiteralNode( const LiteralValue& val )
{
    return Node( val );
}


// static
Soprano::Node Soprano::Node::createLiteralNode( const LiteralValue& val, const QString& language )
{
    return Node( LiteralValue::createPlainLiteral( val.toString(), language ) );
}


// static
QString Soprano::Node::resourceToN3( const QUrl& uri )
{
    QByteArray a = uri.toEncoded();
    return '<' + QString::fromLatin1( a ) + '>';
}


// static
QString Soprano::Node::blankToN3( const QString& blank )
{
    if ( blank.isEmpty() )
        return blank;
    else
        return "_:" + blank;
}

namespace {
    QHash<QChar, QString> createEscapeHash() {
        QHash<QChar, QString> escapeHash;
        escapeHash.insert( QChar::fromLatin1('\\'), QString::fromLatin1("\\\\") );
        escapeHash.insert( QChar::fromLatin1('\n'), QString::fromLatin1("\\n") );
        escapeHash.insert( QChar::fromLatin1('\r'), QString::fromLatin1("\\r") );
        escapeHash.insert( QChar::fromLatin1('\b'), QString::fromLatin1("\\b") );
        escapeHash.insert( QChar::fromLatin1('\t'), QString::fromLatin1("\\t") );
        escapeHash.insert( QChar::fromLatin1('\f'), QString::fromLatin1("\\f") );
        escapeHash.insert( QChar::fromLatin1('\"'), QString::fromLatin1("\\\"") );
        escapeHash.insert( QChar::fromLatin1('\''), QString::fromLatin1("\\\'") );

        return escapeHash;
    }

    QHash<QVariant::Type, QString> typeHash;
    QMutex typeMutex;
}

typedef QHash<QChar, QString> CharStringHash;
Q_GLOBAL_STATIC_WITH_ARGS( CharStringHash, g_escapeHash, (createEscapeHash()) );

// static
QString Soprano::Node::literalToN3( const LiteralValue& literal )
{
    QString str = literal.toString();

    QString s;
    s.reserve( str.size() );

    //
    // Escape control chars:  \t \b \n \r \f \\ \" \'
    //
    foreach(const QChar& ch, str) {
        QHash< QChar, QString >::const_iterator it = g_escapeHash()->constFind( ch );
        if( it == g_escapeHash()->constEnd() ) {
            s.append( ch );
        }
        else {
            s.append( it.value() );
        }
    }

    if( literal.isPlain() ) {
        if ( literal.language().isEmpty() ) {
            return '\"' + s + '\"';
        }
        else {
            return '\"' + s + "\"@" + literal.language().toString();
        }
    }
    else {
        QString type;

        QMutexLocker lock( &typeMutex );
        QHash<QVariant::Type, QString>::const_iterator it = typeHash.constFind( literal.type() );
        if( it == typeHash.constEnd() ) {
            type = QString::fromLatin1( literal.dataTypeUri().toEncoded() );
            typeHash.insert( literal.type(), type );
        }
        else {
            type = it.value();
        }

        return QString::fromLatin1( "\"%1\"^^<%2>" ) .arg( s, type );
    }
}


// static
Soprano::Node Soprano::Node::fromN3( const QString& s, N3ParserFlags flags )
{
    QString str( s );
    QTextStream stream( &str );
    return fromN3Stream( stream, flags );
}


// static
Soprano::Node Soprano::Node::fromN3Stream( QTextStream& stream, N3ParserFlags flags )
{
    N3NodeParser p;
    return p.parseNode( stream, flags );
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


QTextStream& operator>>( QTextStream& s, Soprano::Node& n )
{
    Soprano::N3NodeParser p;
    n = p.parseNode( s );
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
#elif QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        // implementation from Qt 4.7 to ease transition
        return qHash(url.toEncoded(QUrl::FormattingOption(0x100)));
#else
        return qHash(url.toEncoded(QUrl::UrlFormattingOption(0x100)));
#endif
    }
}
