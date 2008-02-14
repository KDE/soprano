/*
 * This file is part of Soprano Project.
 *
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

#include "operators.h"

#include "node.h"
#include "statement.h"
#include "backend.h"
#include "literalvalue.h"
#include "locator.h"
#include "bindingset.h"

#include <QtCore/QVariant>


QDataStream& operator<<( QDataStream& stream, const Soprano::Error::Locator& loc )
{
    return stream << ( qint32 )loc.line() << ( qint32 )loc.column() << ( qint32 )loc.byte() << loc.fileName();
}


QDataStream& operator<<( QDataStream& stream, const Soprano::Error::Error& error )
{
    stream << error.isParserError() << ( qint32 )error.code() << error.message();
    if ( error.isParserError() ) {
        Soprano::Error::ParserError e( error );
        stream << e.locator();
    }
    return stream;
}


QDataStream& operator<<( QDataStream& stream, const Soprano::Error::ErrorCode& error )
{
    return stream << ( quint16 )error;
}


QDataStream& operator<<( QDataStream& stream, const Soprano::BackendSetting& setting )
{
    return stream << ( quint16 )setting.option() << setting.userOptionName() << setting.value();
}


QDataStream& operator<<( QDataStream& stream, const Soprano::LiteralValue& value )
{
    return stream << value.variant();
}


QDataStream& operator<<( QDataStream& stream, const Soprano::Node& node )
{
    stream << ( quint8 )node.type();
    if ( node.type() == Soprano::Node::LiteralNode ) {
        stream << node.literal() << node.language();
    }
    else if ( node.type() == Soprano::Node::ResourceNode ) {
        stream << node.uri();
    }
    else if ( node.type() == Soprano::Node::BlankNode ) {
        stream << node.identifier();
    }

    return stream;
}


QDataStream& operator<<( QDataStream& stream, const Soprano::Statement& statement )
{
    return stream << statement.subject()
                  << statement.predicate()
                  << statement.object()
                  << statement.context();
}


QDataStream& operator<<( QDataStream& stream, const Soprano::BindingSet& set )
{
    stream << set.bindingNames();
    for ( int i = 0; i < set.count(); ++i ) {
        stream << set[i];
    }
    return stream;
}


QDataStream& operator>>( QDataStream& stream, Soprano::Error::Locator& loc )
{
    qint32 line, column, byte;
    QString fileName;
    stream >> line >> column >> byte >> fileName;
    loc = Soprano::Error::Locator( line, column, byte, fileName );
    return stream;
}


QDataStream& operator>>( QDataStream& stream, Soprano::Error::Error& error )
{
    bool isParserError = false;
    QString message;
    qint32 code;
    stream >> isParserError >> code >> message;
    if ( isParserError ) {
        Soprano::Error::Locator loc;
        stream >> loc;
        error = Soprano::Error::ParserError( loc, message, code );
    }
    else {
        error = Soprano::Error::Error( message, code );
    }
    return stream;
}


QDataStream& operator>>( QDataStream& stream, Soprano::Error::ErrorCode& error )
{
    quint16 code;
    stream >> code;
    if ( code < Soprano::Error::ErrorUnknown ) {
        error = ( Soprano::Error::ErrorCode )code;
    }
    else {
        error = Soprano::Error::ErrorUnknown;
    }
    return stream;
}


QDataStream& operator>>( QDataStream& stream, Soprano::BackendSetting& setting )
{
    quint16 option;
    QString userOption;
    QVariant value;
    stream >> option >> userOption >> value;
    if ( option == Soprano::BackendOptionUser ) {
        setting = Soprano::BackendSetting( userOption, value );
    }
    else {
        setting = Soprano::BackendSetting( ( Soprano::BackendOption )option, value );
    }
    return stream;
}


QDataStream& operator>>( QDataStream& stream, Soprano::LiteralValue& value )
{
    QVariant v;
    stream >> v;
    value = v;
    return stream;
}


QDataStream& operator>>( QDataStream& stream, Soprano::Node& node )
{
    quint8 type;
    stream >> type;
    if ( type == Soprano::Node::LiteralNode ) {
        Soprano::LiteralValue v;
        QString lang;
        stream >> v >> lang;
        node = Soprano::Node( v, lang );
    }
    else if ( type == Soprano::Node::ResourceNode ) {
        QUrl url;
        stream >> url;
        node = Soprano::Node( url );
    }
    else if ( type == Soprano::Node::BlankNode ) {
        QString id;
        stream >> id;
        node = Soprano::Node( id );
    }
    else {
        node = Soprano::Node();
    }

    return stream;
}


QDataStream& operator>>( QDataStream& stream, Soprano::Statement& statement )
{
    Soprano::Node node;
    stream >> node;
    statement.setSubject( node );
    stream >> node;
    statement.setPredicate( node );
    stream >> node;
    statement.setObject( node );
    stream >> node;
    statement.setContext( node );
    return stream;
}


QDataStream& operator>>( QDataStream& stream, Soprano::BindingSet& set )
{
    QStringList varNames;
    stream >> varNames;
    Q_FOREACH( QString var, varNames ) {
        Soprano::Node node;
        stream >> node;
        set.insert( var, node );
    }
    return stream;
}
