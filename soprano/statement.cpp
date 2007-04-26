/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
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

#include "statement.h"

#include <QtCore/QDebug>


using namespace Soprano;

class Statement::Private : public QSharedData
{
public:
    Node subject;
    Node predicate;
    Node object;
    Node context;
};

Statement::Statement()
{
    d = new Private;
}

Statement::Statement( const Statement &other )
{
    d = other.d;
}

Statement::Statement( const Node &subject, const Node &predicate, const Node &object, const Node &context)
{
    d = new Private;
    d->subject = subject;
    d->predicate = predicate;
    d->object = object;
    d->context = context;
}

Statement::~Statement()
{
}

Statement& Statement::operator=( const Statement& other )
{
    d = other.d;
    return *this;
}

bool Statement::operator==( const Statement& other )
{
    return ( d->subject == other.subject() &&
             d->predicate == other.predicate() &&
             d->object == other.object() &&
             d->context == other.context() );
}

bool Soprano::Statement::matches( const Statement& other ) const
{
    return ( d->subject.matches( other.subject() ) &&
             d->predicate.matches( other.predicate() ) &&
             d->object.matches( other.object() ) &&
             d->context.matches( other.context() ) );
}

void Statement::setSubject( const Node &subject )
{
    // d->detach() is called automatically
    d->subject = subject;
}

Node Statement::subject() const
{
    return d->subject;
}

void Statement::setPredicate( const Node &predicate )
{
    // d->detach() is called automatically
    d->predicate = predicate;
}

Node Statement::predicate() const
{
    return d->predicate;
}

void Statement::setObject( const Node &object )
{
    // d->detach() is called automatically
    d->object = object;
}

Node Statement::object() const
{
    return d->object;
}

void Statement::setContext( const Node &context )
{
    // d->detach() is called automatically
    d->context = context;
}

Node Statement::context() const
{
    return d->context;
}

bool Statement::isValid() const
{
    bool valid = d->subject.isValid() && d->predicate.isValid() && d->object.isValid();

    if ( d->context.isValid() )
    {
        return valid && d->context.isResource();
    }
    return valid;
}


bool Soprano::Statement::operator==( const Statement& other ) const
{
    return ( d->subject == other.d->subject &&
             d->predicate == other.d->predicate &&
             d->object == other.d->object &&
             d->context == other.d->context );
}


QDebug operator<<( QDebug s, const Soprano::Statement& sm )
{
    s.nospace() << "[(" << sm.subject() << "," << sm.predicate() << "," << sm.object() << "," << sm.context() << ")]";
    return s.space();
}
