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

#include "nodepattern.h"
#include "node.h"

#include <QtCore/QDebug>


class Soprano::Inference::NodePattern::Private : public QSharedData
{
public:
    Private()
        : variable( false ) {
    }

    bool variable;
    Node node;
    QString name;
};

Soprano::Inference::NodePattern::NodePattern()
    : d( new Private() )
{
}


Soprano::Inference::NodePattern::NodePattern( const Node& node )
    : d( new Private() )
{
    d->node = node;
}


Soprano::Inference::NodePattern::NodePattern( const QString& varname )
    : d( new Private() )
{
    d->name = varname;
    d->variable = true;
}


Soprano::Inference::NodePattern::NodePattern( const NodePattern& other )
{
    d = other.d;
}


Soprano::Inference::NodePattern::~NodePattern()
{
}


Soprano::Inference::NodePattern& Soprano::Inference::NodePattern::operator=( const NodePattern& other )
{
    d = other.d;
    return *this;
}


bool Soprano::Inference::NodePattern::isVariable() const
{
    return d->variable;
}


Soprano::Node Soprano::Inference::NodePattern::resource() const
{
    return d->node;
}


QString Soprano::Inference::NodePattern::variableName() const
{
    return d->name;
}


bool Soprano::Inference::NodePattern::match( const Node& node ) const
{
    return ( isVariable() || node == d->node );
}


QString Soprano::Inference::NodePattern::createSparqlNodePattern() const
{
    if ( isVariable() ) {
        return '?' + d->name;
    }
    else {
        // FIXME: creating a SPARQL representation of a Node is something that should be available globally
        // FIXME: use more quotes if the strings contain quotes
        if ( d->node.isLiteral() ) {
            if ( !d->node.language().isEmpty() ) {
                return QString( "\"%1\"@%2" ).arg( d->node.toString() ).arg( d->node.language() );
            }
            else {
                return QString( "\"%1\"^^<%2>" ).arg( d->node.toString() ).arg( d->node.dataType().toString() );
            }
        }
        else {
            return '<' + d->node.toString() + '>';
        }
    }
}


QDebug operator<<( QDebug s, const Soprano::Inference::NodePattern& pattern )
{
    if ( pattern.isVariable() ) {
        s.nospace() << QString( "?%1" ).arg( pattern.variableName() );
    }
    else {
        s.nospace() << pattern.resource();
    }
    return s;
}
