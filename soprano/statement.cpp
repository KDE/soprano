/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
 * Copyright (C) 2007-2009 Sebastian Trueg <trueg@kde.org>
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

#include "statement.h"
#include "node.h"

#include <QtCore/QDebug>


class Soprano::Statement::Private : public QSharedData
{
public:
    Node subject;
    Node predicate;
    Node object;
    Node context;
};

Soprano::Statement::Statement()
{
    d = new Private;
}

Soprano::Statement::Statement( const Statement &other )
{
    d = other.d;
}

Soprano::Statement::Statement( const Node &subject, const Node &predicate, const Node &object, const Node &context)
{
    d = new Private;
    d->subject = subject;
    d->predicate = predicate;
    d->object = object;
    d->context = context;
}

Soprano::Statement::~Statement()
{
}

Soprano::Statement& Soprano::Statement::operator=( const Statement& other )
{
    d = other.d;
    return *this;
}

bool Soprano::Statement::operator==( const Statement& other ) const
{
    return ( d->subject == other.subject() &&
             d->predicate == other.predicate() &&
             d->object == other.object() &&
             d->context == other.context() );
}

bool Soprano::Statement::operator!=( const Statement& other ) const
{
    return ( d->subject != other.subject() ||
             d->predicate != other.predicate() ||
             d->object != other.object() ||
             d->context != other.context() );
}

bool Soprano::Statement::matches( const Statement& other ) const
{
    return ( d->subject.matches( other.subject() ) &&
             d->predicate.matches( other.predicate() ) &&
             d->object.matches( other.object() ) &&
             d->context.matches( other.context() ) );
}

void Soprano::Statement::setSubject( const Node &subject )
{
    // d->detach() is called automatically
    d->subject = subject;
}

Soprano::Node Soprano::Statement::subject() const
{
    return d->subject;
}

void Soprano::Statement::setPredicate( const Node &predicate )
{
    // d->detach() is called automatically
    d->predicate = predicate;
}

Soprano::Node Soprano::Statement::predicate() const
{
    return d->predicate;
}

void Soprano::Statement::setObject( const Node &object )
{
    // d->detach() is called automatically
    d->object = object;
}

Soprano::Node Soprano::Statement::object() const
{
    return d->object;
}

void Soprano::Statement::setContext( const Node &context )
{
    // d->detach() is called automatically
    d->context = context;
}

Soprano::Node Soprano::Statement::context() const
{
    return d->context;
}

bool Soprano::Statement::isValid() const
{
    bool valid = ( ( d->subject.isResource() || d->subject.isBlank() ) &&
                   d->predicate.isResource() &&
                   d->object.isValid() );

    if ( d->context.isValid() ) {
        return valid && d->context.isResource();
    }

    return valid;
}


QDebug operator<<( QDebug s, const Soprano::Statement& sm )
{
    s.nospace() << "[" << sm.subject() << "," << sm.predicate() << "," << sm.object() << "," << sm.context() << "]";
    return s;
}


QTextStream& operator<<( QTextStream& s, const Soprano::Statement& sm )
{
    s << "[ " << sm.subject() << ", " << sm.predicate() << ", " << sm.object() << ", " << sm.context() << " ]";
    return s;
}
