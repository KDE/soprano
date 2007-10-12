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

#include "sesame2valuefactory.h"
#include "sesame2types.h"
#include "jniwrapper.h"

#include <soprano/node.h>
#include <soprano/statement.h>
#include <soprano/literalvalue.h>

#include <QtCore/QDebug>


class Soprano::Sesame2::ValueFactory::Private
{
public:
    Private( ValueFactory* parent )
        : m_parent( parent ),
          m_IDcreateURI( 0 ),
          m_IDcreateBNode( 0 ),
          m_IDcreateBNodeFromString( 0 ),
          m_IDcreateLiteralWithLang( 0 ),
          m_IDcreateLiteralWithDataType( 0 ),
          m_IDcreateStatement( 0 ),
          m_IDcreateStatementWithContext( 0 ) {
    }

    jmethodID IDcreateURI() {
        if ( !m_IDcreateURI ) {
            m_IDcreateURI = m_parent->getMethodID( "createURI",
                                                   "(L"JAVA_LANG_STRING";)L"ORG_OPENRDF_MODEL_URI";" );
            JNIWrapper::instance()->debugException();
        }
        return m_IDcreateURI;
    }

    jmethodID IDcreateBNode() {
        if ( !m_IDcreateBNode ) {
            m_IDcreateBNode = m_parent->getMethodID( "createBNode",
                                                     "()L"ORG_OPENRDF_MODEL_BNODE";" );
            JNIWrapper::instance()->debugException();
        }
        return m_IDcreateBNode;
    }

    jmethodID IDcreateBNodeFromString() {
        if ( !m_IDcreateBNodeFromString ) {
            m_IDcreateBNodeFromString = m_parent->getMethodID( "createBNode",
                                                               "(L"JAVA_LANG_STRING";)L"ORG_OPENRDF_MODEL_BNODE";" );
            JNIWrapper::instance()->debugException();
        }
        return m_IDcreateBNodeFromString;
    }

    jmethodID IDcreateLiteralWithLang() {
        if ( !m_IDcreateLiteralWithLang ) {
            m_IDcreateLiteralWithLang = m_parent->getMethodID( "createLiteral",
                                                               "(L"JAVA_LANG_STRING";L"JAVA_LANG_STRING";)L"ORG_OPENRDF_MODEL_LITERAL";" );
            JNIWrapper::instance()->debugException();
        }
        return m_IDcreateLiteralWithLang;
    }

    jmethodID IDcreateLiteralWithDataType() {
        if ( !m_IDcreateLiteralWithDataType ) {
            m_IDcreateLiteralWithDataType = m_parent->getMethodID( "createLiteral",
                                                                   "(L"JAVA_LANG_STRING";L"ORG_OPENRDF_MODEL_URI";)L"ORG_OPENRDF_MODEL_LITERAL";" );
            JNIWrapper::instance()->debugException();
        }
        return m_IDcreateLiteralWithDataType;
    }

    jmethodID IDcreateStatement() {
        if ( !m_IDcreateStatement ) {
            m_IDcreateStatement = m_parent->getMethodID( "createStatement",
                                                         "(L"ORG_OPENRDF_MODEL_RESOURCE";L"ORG_OPENRDF_MODEL_URI";L"ORG_OPENRDF_MODEL_VALUE";)L"ORG_OPENRDF_MODEL_STATEMENT";" );
            JNIWrapper::instance()->debugException();
        }
        return m_IDcreateStatement;
    }

    jmethodID IDcreateStatementWithContext() {
        if ( !m_IDcreateStatementWithContext ) {
            m_IDcreateStatementWithContext = m_parent->getMethodID( "createStatement",
                                                                    "(L"ORG_OPENRDF_MODEL_RESOURCE";L"ORG_OPENRDF_MODEL_URI";L"ORG_OPENRDF_MODEL_VALUE";L"ORG_OPENRDF_MODEL_RESOURCE";)L"ORG_OPENRDF_MODEL_STATEMENT";" );
            JNIWrapper::instance()->debugException();
        }
        return m_IDcreateStatementWithContext;
    }

private:
    ValueFactory* m_parent;

    jmethodID m_IDcreateURI;
    jmethodID m_IDcreateBNode;
    jmethodID m_IDcreateBNodeFromString;
    jmethodID m_IDcreateLiteralWithLang;
    jmethodID m_IDcreateLiteralWithDataType;
    jmethodID m_IDcreateStatement;
    jmethodID m_IDcreateStatementWithContext;
};


Soprano::Sesame2::ValueFactory::ValueFactory( const JObjectRef& o )
    : JNIObjectWrapper( o ),
      d( new Private( this ) )
{
}


Soprano::Sesame2::ValueFactory::~ValueFactory()
{
    delete d;
}


JObjectRef Soprano::Sesame2::ValueFactory::createBNode()
{
    return callObjectMethod( d->IDcreateBNode() );
}


JObjectRef Soprano::Sesame2::ValueFactory::convertNode( const Node& node )
{
    switch( node.type() ) {
    case Node::ResourceNode: {
        JStringRef s( node.uri().toEncoded() );
        return callObjectMethod( d->IDcreateURI(), s.data() );
    }

    case Node::BlankNode:
        return callObjectMethod( d->IDcreateBNodeFromString(), JStringRef( node.identifier() ).data() );

    case Node::LiteralNode:
        // FIXME: is it more performant to create the instances directly from the values instead of strings?
        if ( node.literal().isString() && !node.language().isEmpty() ) {
            return callObjectMethod( d->IDcreateLiteralWithLang(),
                                     JStringRef( node.toString() ).data(),
                                     JStringRef( node.language() ).data() );
        }
        else{
            JStringRef ns( node.toString() );
            JStringRef dtus( node.dataType().toEncoded() );
            jobject dataTypeUri = callObjectMethod( d->IDcreateURI(), dtus.data() );
            return callObjectMethod( d->IDcreateLiteralWithDataType(),
                                     ns.data(),
                                     dataTypeUri );
        }

    default:
        // FIXME: unsure about this
        return 0;
    }
}


JObjectRef Soprano::Sesame2::ValueFactory::convertStatement( const Statement& statement )
{
    JObjectRef subject = convertNode( statement.subject() );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        qDebug() << "(Soprano::Sesame2::ValueFactory::convertStatement) conversion of subject failed";
        return 0;
    }

    JObjectRef predicate = convertNode( statement.predicate() );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        qDebug() << "(Soprano::Sesame2::ValueFactory::convertStatement) conversion of predicate failed";
        return 0;
    }

    JObjectRef object = convertNode( statement.object() );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        qDebug() << "(Soprano::Sesame2::ValueFactory::convertStatement) conversion of object failed";
        return 0;
    }

    JObjectRef context = convertNode( statement.context() );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        qDebug() << "(Soprano::Sesame2::ValueFactory::convertStatement) conversion of context failed";
        return 0;
    }

    if ( context ) {
        return callObjectMethod( d->IDcreateStatementWithContext(), subject.data(), predicate.data(), object.data(), context.data() );
    }
    else {
        return callObjectMethod( d->IDcreateStatement(), subject.data(), predicate.data(), object.data() );
    }
}
