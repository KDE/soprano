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

#include "sesame2sopranowrapper.h"
#include "jniwrapper.h"
#include "sesame2types.h"


class Soprano::Sesame2::SopranoWrapper::Private
{
public:
    Private( SopranoWrapper* parent )
        : m_parent( parent ),
          m_IDremoveFromDefaultContext( 0 ) {
    }

    jmethodID IDremoveFromDefaultContext() {
        if ( !m_IDremoveFromDefaultContext ) {
            m_IDremoveFromDefaultContext = m_parent->getMethodID( "removeFromDefaultContext",
                                                                  "(L" ORG_OPENRDF_MODEL_RESOURCE ";L" ORG_OPENRDF_MODEL_URI ";L" ORG_OPENRDF_MODEL_VALUE ";)V" );
            JNIWrapper::instance()->debugException();
        }
        return m_IDremoveFromDefaultContext;
    }

private:
    SopranoWrapper* m_parent;

    jmethodID m_IDremoveFromDefaultContext;
};



Soprano::Sesame2::SopranoWrapper::SopranoWrapper( const JObjectRef& object )
    : JNIObjectWrapper( object ),
      d( new Private( this ) )
{
}


Soprano::Sesame2::SopranoWrapper::~SopranoWrapper()
{
    delete d;
}


void Soprano::Sesame2::SopranoWrapper::removeFromDefaultContext( const JObjectRef& subject, const JObjectRef& predicate, const JObjectRef& object )
{
    callVoidMethod( d->IDremoveFromDefaultContext(), subject.data(), predicate.data(), object.data() );
}
