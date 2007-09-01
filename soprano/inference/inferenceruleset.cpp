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

#include "config.h"

#include "inferenceruleset.h"
#include "inferencerule.h"
#include "inferenceruleparser.h"

#include <QtCore/QHash>
#include <QtCore/QList>


class Soprano::Inference::RuleSet::Private : public QSharedData
{
public:
    QHash<QString, Rule> ruleMap;
    mutable QList<Rule> rules;
};


Soprano::Inference::RuleSet::RuleSet()
    : d( new Private() )
{
}


Soprano::Inference::RuleSet::RuleSet( const RuleSet& other )
{
    d = other.d;
}

Soprano::Inference::RuleSet::~RuleSet()
{
}


Soprano::Inference::RuleSet& Soprano::Inference::RuleSet::operator=( const RuleSet& other )
{
    d = other.d;
    return *this;
}


void Soprano::Inference::RuleSet::insert( const QString& name, const Rule& rule )
{
    d->ruleMap[name] = rule;
    d->rules.clear();
}


int Soprano::Inference::RuleSet::count() const
{
    return d->ruleMap.count();
}


Soprano::Inference::Rule Soprano::Inference::RuleSet::at( int index ) const
{
    if ( d->rules.isEmpty() ) {
        d->rules = d->ruleMap.values();
    }
    return d->rules.at( index );
}


Soprano::Inference::Rule Soprano::Inference::RuleSet::operator[]( int index ) const
{
    return at( index );
}


Soprano::Inference::Rule Soprano::Inference::RuleSet::rule( const QString& name ) const
{
    return d->ruleMap[name];
}


Soprano::Inference::Rule Soprano::Inference::RuleSet::operator[]( const QString& name ) const
{
    return rule( name );
}


QList<Soprano::Inference::Rule> Soprano::Inference::RuleSet::allRules() const
{
    if ( d->rules.isEmpty() ) {
        d->rules = d->ruleMap.values();
    }
    return d->rules;
}


void Soprano::Inference::RuleSet::clear()
{
    d->ruleMap.clear();
    d->rules.clear();
}


QStringList Soprano::Inference::RuleSet::ruleNames() const
{
    return d->ruleMap.keys();
}


Soprano::Inference::RuleSet Soprano::Inference::RuleSet::standardRuleSet( StandardRuleSet set )
{
    QString path( SOPRANO_PREFIX );
    path += "/share/soprano/rules/";

    switch( set ) {
    case RDFS:
        path += "rdfs.rules";
        break;
    case NRL:
        path += "nrl.rules";
        break;
    }

    RuleParser parser;
    parser.parseFile( path );
    return parser.rules();
}
