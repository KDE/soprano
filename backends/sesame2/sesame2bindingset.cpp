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

#include "sesame2bindingset.h"
#include "sesame2types.h"
#include "jniwrapper.h"


class Soprano::Sesame2::BindingSet::Private
{
public:
    Private( BindingSet* parent )
        : m_parent( parent ) {
    }

    jmethodID IDgetValue() {
        return m_parent->getMethodID( "getValue", "(L"JAVA_LANG_STRING";)L"ORG_OPENRDF_MODEL_VALUE";" );
    }

private:
    BindingSet* m_parent;
};


Soprano::Sesame2::BindingSet::BindingSet( jobject o )
    : JNIObjectWrapper( o ),
      d( new Private( this ) )
{
}


Soprano::Sesame2::BindingSet::~BindingSet()
{
    delete d;
}


jobject Soprano::Sesame2::BindingSet::getValue( jstring s )
{
    return callObjectMethod( d->IDgetValue(), s );
}
