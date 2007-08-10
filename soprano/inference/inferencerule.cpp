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

#include "inferencerule.h"
#include "statementpattern.h"

#include <QtCore/QString>
#include <QtCore/QDebug>


class Soprano::Inference::Rule::Private : public QSharedData
{
public:
    QList<StatementPattern> preconditions;
    StatementPattern effect;
};


Soprano::Inference::Rule::Rule()
    : d( new Private() )
{
}


Soprano::Inference::Rule::Rule( const Rule& other )
{
    d = other.d;
}


Soprano::Inference::Rule::~Rule()
{
}


Soprano::Inference::Rule& Soprano::Inference::Rule::operator=( const Rule& other )
{
    d = other.d;
    return *this;
}


QList<Soprano::Inference::StatementPattern> Soprano::Inference::Rule::preconditions() const
{
    return d->preconditions;
}


void Soprano::Inference::Rule::addPrecondition( const StatementPattern& sp )
{
    d->preconditions.append( sp );
}


Soprano::Inference::StatementPattern Soprano::Inference::Rule::effect() const
{
    return d->effect;
}


void Soprano::Inference::Rule::setEffect( const StatementPattern& e )
{
    d->effect = e;
}


bool Soprano::Inference::Rule::match( const Statement& statement ) const
{
    for ( QList<StatementPattern>::const_iterator it = d->preconditions.constBegin();
          it != d->preconditions.constEnd(); ++it ) {
        if ( it->match( statement ) ) {
            return true;
        }
    }
    return false;
}


QString Soprano::Inference::Rule::createSparqlQuery() const
{
    QString query = "SELECT * WHERE { ";
    for ( QList<StatementPattern>::const_iterator it = d->preconditions.constBegin();
          it != d->preconditions.constEnd(); ++it ) {
        query += it->createSparqlGraphPattern() + " . ";
    }
    query += "}";

    return query;
}


QDebug operator<<( QDebug s, const Soprano::Inference::Rule& rule )
{
    s.nospace() << "[";
    QList<Soprano::Inference::StatementPattern> cl = rule.preconditions();
    QList<Soprano::Inference::StatementPattern>::const_iterator it = cl.constBegin();
    while ( it != cl.constEnd() ) {
        s.nospace() << *it;
        ++it;
        if ( it != cl.constEnd() ) {
            s.nospace() << ", ";
        }
    }
    return s.nospace() << " -> " << rule.effect() << "]";
}
