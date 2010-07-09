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

#include "sesame2utils.h"
#include "jniwrapper.h"
#include "sesame2types.h"
#include "jniobjectwrapper.h"

#include "statement.h"
#include "node.h"

#include <QtCore/QDebug>


QUrl Soprano::Sesame2::convertURI( const JObjectRef& uri )
{
    JNIObjectWrapper uriWrapper( uri );
    JStringRef uriString = uriWrapper.callObjectMethod( uriWrapper.getMethodID( "toString", "()L"JAVA_LANG_STRING";" ) );
    return QUrl::fromEncoded( uriString.toAscii() );
}


Soprano::Node Soprano::Sesame2::convertNode( const JObjectRef& resource )
{
    JNIObjectWrapper resourceWrapper( resource );

    JClassRef classURI = JNIWrapper::instance()->env()->FindClass( ORG_OPENRDF_MODEL_URI );
    JClassRef classBNode = JNIWrapper::instance()->env()->FindClass( ORG_OPENRDF_MODEL_BNODE );
    JClassRef classLiteral = JNIWrapper::instance()->env()->FindClass( ORG_OPENRDF_MODEL_LITERAL );

    if ( !resource ) {
        // empty node
        return Node();
    }
    else if ( JNIWrapper::instance()->env()->IsInstanceOf( resource, classURI ) ) {
        return convertURI( resource );
    }
    else if ( JNIWrapper::instance()->env()->IsInstanceOf( resource, classBNode ) ) {
        JStringRef uri = resourceWrapper.callObjectMethod( resourceWrapper.getMethodID( "getID", "()L"JAVA_LANG_STRING";" ) );
        return Node( uri.toQString() );
    }
    else if ( JNIWrapper::instance()->env()->IsInstanceOf( resource, classLiteral ) ) {
        JStringRef value = resourceWrapper.callObjectMethod( resourceWrapper.getMethodID( "getLabel", "()L"JAVA_LANG_STRING";" ) );
        JStringRef lang = resourceWrapper.callObjectMethod( resourceWrapper.getMethodID( "getLanguage", "()L"JAVA_LANG_STRING";" ) );
        JObjectRef dataType = resourceWrapper.callObjectMethod( resourceWrapper.getMethodID( "getDatatype", "()L"ORG_OPENRDF_MODEL_URI";" ) );

        if ( dataType ) {
            return Node( LiteralValue::fromString( value.toQString(), convertURI( dataType ) ) );
        }
        else {
            return Node( LiteralValue::createPlainLiteral( value.toQString(), lang.toQString() ) );
        }
    }
    else {
        qDebug() << "Unknown resource type!";
        return Node();
    }
}


Soprano::Statement Soprano::Sesame2::convertStatement( const JObjectRef& o )
{
    JNIObjectWrapper statementWrapper( o );

    JObjectRef subject = statementWrapper.callObjectMethod( statementWrapper.getMethodID( "getSubject", "()L"ORG_OPENRDF_MODEL_RESOURCE";" ) );
    JObjectRef predicate = statementWrapper.callObjectMethod( statementWrapper.getMethodID( "getPredicate", "()L"ORG_OPENRDF_MODEL_URI";" ) );
    JObjectRef object = statementWrapper.callObjectMethod( statementWrapper.getMethodID( "getObject", "()L"ORG_OPENRDF_MODEL_VALUE";" ) );
    JObjectRef context = statementWrapper.callObjectMethod( statementWrapper.getMethodID( "getContext", "()L"ORG_OPENRDF_MODEL_RESOURCE";" ) );

    return Statement( convertNode( subject ),
                      convertNode( predicate ),
                      convertNode( object ),
                      convertNode( context ) );
}
