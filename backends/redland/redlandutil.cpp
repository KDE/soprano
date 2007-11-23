/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
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
 * You should have received a copy of the GNU Library General Public
 * License along with this library; see the file COPYING.LIB.  If
 * not, write to the Free Software Foundation, Inc., 51 Franklin
 * Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "redlandutil.h"

#include "node.h"
#include "statement.h"
#include "redlandworld.h"

// Hint: all strings in redland are Utf8, except for the literal language tags.

Soprano::Node Soprano::Redland::Util::createNode( librdf_node *node )
{
    if ( librdf_node_is_resource( node ) ) {
        librdf_uri *uri = librdf_node_get_uri( node );
        return Soprano::Node( QUrl::fromEncoded( (const char *)librdf_uri_as_string(uri), QUrl::StrictMode ) );
    }
    else if ( librdf_node_is_blank( node ) ) {
        return Soprano::Node( QString::fromUtf8( (const char *)librdf_node_get_blank_identifier( node ), QUrl::StrictMode ) );
    }
    else if ( librdf_node_is_literal( node ) ) {
        librdf_uri* datatype = librdf_node_get_literal_value_datatype_uri( node );
        if ( !datatype ) {
            return Soprano::Node( Soprano::LiteralValue( (const char *)librdf_node_get_literal_value( node ) ),
                                  QString::fromAscii( librdf_node_get_literal_value_language( node ) ) );
        }
        return Soprano::Node( Soprano::LiteralValue::fromString( QString::fromUtf8( (const char *)librdf_node_get_literal_value( node ) ),
                                                                 QUrl::fromEncoded( (const char *)librdf_uri_as_string( datatype ),
                                                                                    QUrl::StrictMode ) ),
                              QString::fromAscii( librdf_node_get_literal_value_language( node ) ) );
    }

    return Soprano::Node();
}

librdf_node *Soprano::Redland::Util::createNode( const Node &node )
{
    librdf_world *world = World::self()->worldPtr();

    if ( node.isResource() ) {
        return librdf_new_node_from_uri_string( world, (unsigned char *)node.uri().toEncoded().data() );
    }
    else if ( node.isBlank() ) {
        return librdf_new_node_from_blank_identifier( world, (unsigned char *) node.identifier().toUtf8().data() );
    }
    else if ( node.isLiteral() ) {
        if ( node.literal().isString() ) {
            return librdf_new_node_from_typed_literal( world,
                                                       (unsigned char *)node.literal().toString().toUtf8().data(),
                                                       node.language().toUtf8().data(),
                                                       0 );
        }
        else {
            return librdf_new_node_from_typed_literal( world,
                                                       (unsigned char *)node.literal().toString().toUtf8().data(),
                                                       0,
                                                       librdf_new_uri( world, (const unsigned char*)node.dataType().toEncoded().data() ) );
        }
    }

    return 0;
}

librdf_statement *Soprano::Redland::Util::createStatement( const Statement &statement )
{
    librdf_world *world = World::self()->worldPtr();

    librdf_node *subject = createNode( statement.subject() );
    librdf_node *predicate = createNode( statement.predicate() );
    librdf_node *object = createNode( statement.object() );

    return librdf_new_statement_from_nodes( world, subject, predicate, object );
}

Soprano::Statement Soprano::Redland::Util::createStatement( librdf_statement *st )
{
    librdf_node *subject = librdf_statement_get_subject( st );
    librdf_node *predicate = librdf_statement_get_predicate( st );
    librdf_node *object = librdf_statement_get_object( st );

    return Soprano::Statement( createNode( subject), createNode( predicate), createNode( object ) );
}

void Soprano::Redland::Util::freeNode( librdf_node* node )
{
    if( node ) {
        librdf_free_node( node );
    }
}


void Soprano::Redland::Util::freeStatement( librdf_statement* statement )
{
    if( statement ) {
        librdf_free_statement( statement );
    }
}
