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

#include "statementpattern.h"
#include "nodepattern.h"
#include "statement.h"
#include "bindingset.h"

#include <QtCore/QDebug>
#include <QtCore/QStringList>


class Soprano::Inference::StatementPattern::Private : public QSharedData
{
public:
    NodePattern subject;
    NodePattern predicate;
    NodePattern object;
};


Soprano::Inference::StatementPattern::StatementPattern()
    : d( new Private() )
{
}


Soprano::Inference::StatementPattern::StatementPattern( const NodePattern& subject, const NodePattern& predicate, const NodePattern& object )
    : d( new Private() )
{
    d->subject = subject;
    d->predicate = predicate;
    d->object = object;
}


Soprano::Inference::StatementPattern::StatementPattern( const StatementPattern& other )
{
    d = other.d;
}


Soprano::Inference::StatementPattern::~StatementPattern()
{
}


Soprano::Inference::StatementPattern Soprano::Inference::StatementPattern::operator=( const StatementPattern& other )
{
    d = other.d;
    return *this;
}


Soprano::Inference::NodePattern Soprano::Inference::StatementPattern::subjectPattern() const
{
    return d->subject;
}


Soprano::Inference::NodePattern Soprano::Inference::StatementPattern::predicatePattern() const
{
    return d->predicate;
}


Soprano::Inference::NodePattern Soprano::Inference::StatementPattern::objectPattern() const
{
    return d->object;
}


bool Soprano::Inference::StatementPattern::match( const Statement& s ) const
{
    return( d->subject.match( s.subject() ) &&
            d->predicate.match( s.predicate() ) &&
            d->object.match( s.object() ) );
}


QString Soprano::Inference::StatementPattern::createSparqlGraphPattern( const BindingSet& bindings ) const
{
    return QString( "%1 %2 %3" )
        .arg( d->subject.createSparqlNodePattern( bindings ) )
        .arg( d->predicate.createSparqlNodePattern( bindings ) )
        .arg( d->object.createSparqlNodePattern( bindings ) );
}


bool Soprano::Inference::StatementPattern::isValid() const
{
    return d->subject.isValid() && d->predicate.isValid() && d->object.isValid();
}


QDebug operator<<( QDebug s, const Soprano::Inference::StatementPattern& pattern )
{
    s.nospace() << "(" << pattern.subjectPattern() << " " << pattern.predicatePattern() << " " << pattern.objectPattern() << ")";
    return s;
}
