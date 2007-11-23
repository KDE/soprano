/*
 * This file is part of Soprano Project
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

#include "nquadserializer.h"

#include "node.h"
#include "statement.h"
#include "statementiterator.h"
#include "sopranotypes.h"


#include <QtCore/QtPlugin>


Q_EXPORT_PLUGIN2(soprano_nquadserializer, Soprano::NQuadSerializer)

Soprano::NQuadSerializer::NQuadSerializer()
    : QObject(),
      Serializer( "nquads" )
{
}


Soprano::NQuadSerializer::~NQuadSerializer()
{
}


Soprano::RdfSerializations Soprano::NQuadSerializer::supportedSerializations() const
{
    return SerializationNQuads;
}


bool Soprano::NQuadSerializer::serialize( StatementIterator it,
                                          QTextStream& stream,
                                          RdfSerialization serialization,
                                          const QString& userSerialization ) const
{
    clearError();

    if ( serialization == SerializationNQuads ) {
        while ( it.next() ) {
            serializeStatement( *it, stream );
        }
        return true;
    }
    else {
        setError( "Unsupported serialization " + serializationMimeType( serialization, userSerialization ),
                  Error::ErrorInvalidArgument );
        return false;
    }
}


void Soprano::NQuadSerializer::serializeStatement( const Statement& s, QTextStream& stream ) const
{
    serializeNode( s.subject(), stream );
    stream << " ";
    serializeNode( s.predicate(), stream );
    stream << " ";
    serializeNode( s.object(), stream );
    stream << " ";
    if ( !s.context().isEmpty() ) {
        serializeNode( s.context(), stream );
        stream << " ";
    }
    stream << "." << endl;
}


void Soprano::NQuadSerializer::serializeNode( const Node& node, QTextStream& stream ) const
{
    switch( node.type() ) {
    case Soprano::Node::LiteralNode:
        stream << '\"' << node.literal().toString() << "\"";
        if ( node.literal().isString() && !node.language().isEmpty() ) {
            stream << "@" << node.language();
        }
        else {
            stream << "^^<" << node.literal().dataTypeUri().toString() << '>';
        }
        break;
    case Soprano::Node::BlankNode:
        stream << "_:" << node.identifier();
        break;
    case Soprano::Node::ResourceNode:
        stream << '<' << node.uri().toString() << '>';
        break;
    default:
        // do nothing
        break;
    }
}

#include "nquadserializer.moc"
