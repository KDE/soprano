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

#include "nrlmodel.h"
#include "error.h"
#include "vocabulary/nrl.h"
#include "queryresultiterator.h"
#include "statementiterator.h"
#include "node.h"
#include "statement.h"

#include <QtCore/QDebug>


class Soprano::NRLModel::Private
{
public:
    Private()
        : ignoreContext( true ) {
    }

    bool ignoreContext;
};

Soprano::NRLModel::NRLModel()
    : FilterModel(),
      d( new Private() )
{
}


Soprano::NRLModel::NRLModel( Model* parent )
    : FilterModel( parent ),
      d( new Private() )
{
}


Soprano::NRLModel::~NRLModel()
{
    delete d;
}


void Soprano::NRLModel::setIgnoreContext( bool b )
{
    d->ignoreContext = b;
}


bool Soprano::NRLModel::ignoreContext() const
{
    return d->ignoreContext;
}


Soprano::Error::ErrorCode Soprano::NRLModel::addStatement( const Statement& statement )
{
    // 1. check if any cardinality restrictions are defined for s.predicate()
    // 2. if so -> enforce
    // 3. if not -> check if some for superproperties are defined (optional advanced feature)

    QString query = QString( "select ?min ?max ?c where { { <%1> <%2> ?min } UNION { <%1> <%3> ?max } UNION { <%1> <%4> ?c } }" )
                    .arg( statement.predicate().toString() )
                    .arg( Vocabulary::NRL::minCardinality().toString() )
                    .arg( Vocabulary::NRL::maxCardinality().toString() )
                    .arg( Vocabulary::NRL::cardinality().toString() );

    QueryResultIterator it = FilterModel::executeQuery( query, Query::QUERY_LANGUAGE_SPARQL );
    if ( !it.isValid() ) {
        setError( QString( "Query failed: '%1'" ).arg( query ) );
        return Error::ERROR_UNKNOWN;
    }

    int min = -1;
    int max = -1;
    int c = -1;

    while ( it.next() ) {
        if ( it.binding( "min" ).isLiteral() ) {
            min = it.binding( "min" ).literal().toInt();
        }
        else if ( it.binding( "max" ).isLiteral() ) {
            max = it.binding( "max" ).literal().toInt();
        }
        if ( it.binding( "c" ).isLiteral() ) {
            c = it.binding( "c" ).literal().toInt();
        }
    }

    if ( min >= 0 || max >= 0 || c >= 0 ) {
        qDebug() << "Predicate " << statement.predicate() << " has cardinalities: " << min << "; " << max << "; " << c;

        // the simple case (and also the most frequently used I suppose)
        if ( c == 1 || max == 1 ) {
            if ( ignoreContext() ) {
                FilterModel::removeAllStatements( Statement( statement.subject(), statement.predicate(), Node() ) );
            }
            else {
                FilterModel::removeAllStatements( Statement( statement.subject(), statement.predicate(), Node(), statement.context() ) );
            }
            return FilterModel::addStatement( statement );
        }

        // the general case
        else if ( max > 1 || c > 1 ) {
            if ( c > 1 )
                max = c;

            StatementIterator sit;
            if ( ignoreContext() ) {
                sit = FilterModel::listStatements( Statement( statement.subject(), statement.predicate(), Node() ) );
            }
            else {
                sit = FilterModel::listStatements( Statement( statement.subject(), statement.predicate(), statement.context() ) );
            }

            QList<Statement> matchingStatements;
            while ( sit.next() ) {
                Statement s = *sit;
                if ( ignoreContext() || s.context() == statement.context() ) {
                    matchingStatements.append( s );
                }
            }

            if ( matchingStatements.count() >= max ) {
                qDebug() << "Found " << matchingStatements.count() << " statements that define " << statement.predicate() << " for " << statement.subject() << endl
                         << "  -> need to remove " << ( matchingStatements.count()-max+1 ) << " before adding the new statement.";
                if ( matchingStatements.count() == 1 ) {
                    qDebug() << "Removing one statement is easy...";
                    FilterModel::removeStatement( matchingStatements[0] );
                }
                else {
                    qDebug() << "FIXME: which statements to remove? Random? Best would be to remove the oldest...";
                    setError( "Max cardinality for predicate reached" );
                    return Error::ERROR_UNKNOWN;
                }
            }

            return FilterModel::addStatement( statement );
        }
        else {
            qDebug()  << "Predicate " << statement.predicate() << " has unusable cardinality restrictions";
            return FilterModel::addStatement( statement );
        }
    }
    else {
        qDebug() << "Predicate " << statement.predicate() << " has no cardinality restrictions";
        return FilterModel::addStatement( statement );
    }
}
