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
#include "nodepattern.h"

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QDebug>


class Soprano::Inference::Rule::Private : public QSharedData
{
public:
    QList<StatementPattern> preconditions;
    StatementPattern effect;
    Statement bindingStatement;
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


void Soprano::Inference::Rule::bindToStatement( const Statement& statement )
{
    d->bindingStatement = statement;
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


QString Soprano::Inference::Rule::createSparqlQuery( bool bindStatement ) const
{
    QString query = "SELECT * WHERE { ";

    if ( !bindStatement || !d->bindingStatement.isValid() ) {
        for ( QList<StatementPattern>::const_iterator it = d->preconditions.constBegin();
              it != d->preconditions.constEnd(); ++it ) {
            query += it->createSparqlGraphPattern( QMap<QString, Node>() ) + " . ";
        }
    }
    else {
        QStringList subQueries;

        // bind the statement to the variables in the query. If multiple bindings are possible use a SPARQL UNION
        for ( QList<StatementPattern>::const_iterator it = d->preconditions.constBegin();
              it != d->preconditions.constEnd(); ++it ) {
            const StatementPattern& p = *it;
            if ( p.match( d->bindingStatement ) ) {
                QMap<QString, Node> bindings;
                if ( p.subjectPattern().isVariable() ) {
                    bindings.insert( p.subjectPattern().variableName(), d->bindingStatement.subject() );
                }
                if ( p.predicatePattern().isVariable() ) {
                    bindings.insert( p.predicatePattern().variableName(), d->bindingStatement.predicate() );
                }
                if ( p.objectPattern().isVariable() ) {
                    bindings.insert( p.objectPattern().variableName(), d->bindingStatement.object() );
                }

                // create a whole new subquery
                QString subQuery;
                for ( QList<StatementPattern>::const_iterator it2 = d->preconditions.constBegin();
                      it2 != d->preconditions.constEnd(); ++it2 ) {
                    // skip the one that is fully bound
                    if ( it2 != it ) {
                        subQuery += it2->createSparqlGraphPattern( bindings ) + " . ";
                    }
                }

                subQueries.append( subQuery );
            }
        }

        if ( subQueries.count() > 1 ) {
            query += "{ " + subQueries.join( " } UNION { " ) + " }";
        }
        else {
            query += subQueries[0];
        }
    }

    query += "}";

    return query;
}


Soprano::Statement Soprano::Inference::Rule::bindStatementPattern( const StatementPattern& pattern, const QMap<QString, Node>& bindings ) const
{
    // If a variable in one of the patterns cannot be bound from bindings
    // the variable was pre-bound during the query in createSparqlGraphPattern
    // thus, we have to match d->bindingStatement (hoping that it was not changed)
    //
    // Some bindings could even be invalid since our UNION queries use different vars

    Statement s;
    if ( pattern.subjectPattern().isVariable() ) {
        QMap<QString, Node>::const_iterator it = bindings.find( pattern.subjectPattern().variableName() );
        if ( it != bindings.constEnd() && it.value().isValid() ) {
            s.setSubject( it.value() );
        }
        else {
            s.setSubject( d->bindingStatement.subject() );
        }
    }
    else {
        s.setSubject( pattern.subjectPattern().resource() );
    }

    if ( pattern.predicatePattern().isVariable() ) {
        QMap<QString, Node>::const_iterator it = bindings.find( pattern.predicatePattern().variableName() );
        if ( it != bindings.constEnd() && it.value().isValid() ) {
            s.setPredicate( it.value() );
        }
        else {
            s.setPredicate( d->bindingStatement.predicate() );
        }
    }
    else {
        s.setPredicate( pattern.predicatePattern().resource() );
    }

    if ( pattern.objectPattern().isVariable() ) {
        QMap<QString, Node>::const_iterator it = bindings.find( pattern.objectPattern().variableName() );
        if ( it != bindings.constEnd() && it.value().isValid() ) {
            s.setObject( it.value() );
        }
        else {
            s.setObject( d->bindingStatement.object() );
        }
    }
    else {
        s.setObject( pattern.objectPattern().resource() );
    }


    return s;
}


Soprano::Statement Soprano::Inference::Rule::bindEffect( const QMap<QString, Node>& bindings ) const
{
    return bindStatementPattern( d->effect, bindings );
}


QList<Soprano::Statement> Soprano::Inference::Rule::bindPreconditions( const QMap<QString, Node>& bindings ) const
{
    QList<Statement> sl;
    for ( QList<StatementPattern>::const_iterator it = d->preconditions.constBegin();
          it != d->preconditions.constEnd(); ++it ) {
        sl.append( bindStatementPattern( *it, bindings ) );
    }

    return sl;
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
