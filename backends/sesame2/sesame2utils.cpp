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

#include <soprano/statement.h>
#include <soprano/node.h>

#include <QtCore/QDebug>


class ClassCache
{
public:
    ClassCache() {
        classURI = JNIWrapper::instance()->env()->FindClass( ORG_OPENRDF_MODEL_URI );
        classBNode = JNIWrapper::instance()->env()->FindClass( ORG_OPENRDF_MODEL_BNODE );
        classLiteral = JNIWrapper::instance()->env()->FindClass( ORG_OPENRDF_MODEL_LITERAL );

        Q_ASSERT( classURI );
        Q_ASSERT( classBNode );
        Q_ASSERT( classLiteral );
    }

    jclass classURI;
    jclass classBNode;
    jclass classLiteral;
};

Q_GLOBAL_STATIC( ClassCache, classCache );



QUrl Soprano::Sesame2::convertURI( jobject uri )
{
    JNIObjectWrapper uriWrapper( uri );
    jobject uriString = uriWrapper.callObjectMethod( uriWrapper.getMethodID( "toString", "()L"JAVA_LANG_STRING";" ) );
    return QUrl( JNIWrapper::instance()->convertString( reinterpret_cast<jstring>( uriString ) ) );
}


Soprano::Node Soprano::Sesame2::convertNode( jobject resource )
{
    JNIObjectWrapper resourceWrapper( resource );

    if ( !resource ) {
        // empty node
        return Node();
    }
    else if ( JNIWrapper::instance()->env()->IsInstanceOf( resource, classCache()->classURI ) ) {
        return convertURI( resource );
    }
    else if ( JNIWrapper::instance()->env()->IsInstanceOf( resource, classCache()->classBNode ) ) {
        jobject uri = resourceWrapper.callObjectMethod( resourceWrapper.getMethodID( "getID", "()L"JAVA_LANG_STRING";" ) );
        return QUrl( JNIWrapper::instance()->convertString( reinterpret_cast<jstring>( uri ) ) );
    }
    else if ( JNIWrapper::instance()->env()->IsInstanceOf( resource, classCache()->classLiteral ) ) {
        jobject value = resourceWrapper.callObjectMethod( resourceWrapper.getMethodID( "getLabel", "()L"JAVA_LANG_STRING";" ) );
        jobject lang = resourceWrapper.callObjectMethod( resourceWrapper.getMethodID( "getLanguage", "()L"JAVA_LANG_STRING";" ) );
        jobject dataType = resourceWrapper.callObjectMethod( resourceWrapper.getMethodID( "getDatatype", "()L"ORG_OPENRDF_MODEL_URI";" ) );

        if ( dataType ) {
            return Node( LiteralValue::fromString( JNIWrapper::instance()->convertString( reinterpret_cast<jstring>( value ) ), convertURI( dataType ) ),
                         JNIWrapper::instance()->convertString( reinterpret_cast<jstring>( lang ) ) );
        }
        else {
            return Node( LiteralValue( JNIWrapper::instance()->convertString( reinterpret_cast<jstring>( value ) ) ),
                         JNIWrapper::instance()->convertString( reinterpret_cast<jstring>( lang ) ) );
        }
    }
    else {
        qDebug() << "Unknown resource type!";
        return Node();
    }
}


Soprano::Statement Soprano::Sesame2::convertStatement( jobject o )
{
    Q_ASSERT( JNIWrapper::instance()->env()->IsInstanceOf( o, JNIWrapper::instance()->env()->FindClass( ORG_OPENRDF_MODEL_STATEMENT ) ) );

    JNIObjectWrapper statementWrapper( o );

    jobject subject = statementWrapper.callObjectMethod( statementWrapper.getMethodID( "getSubject", "()L"ORG_OPENRDF_MODEL_RESOURCE";" ) );
    jobject predicate = statementWrapper.callObjectMethod( statementWrapper.getMethodID( "getPredicate", "()L"ORG_OPENRDF_MODEL_URI";" ) );
    jobject object = statementWrapper.callObjectMethod( statementWrapper.getMethodID( "getObject", "()L"ORG_OPENRDF_MODEL_VALUE";" ) );
    jobject context = statementWrapper.callObjectMethod( statementWrapper.getMethodID( "getContext", "()L"ORG_OPENRDF_MODEL_RESOURCE";" ) );

    return Statement( convertNode( subject ),
                      convertNode( predicate ),
                      convertNode( object ),
                      convertNode( context ) );
}
