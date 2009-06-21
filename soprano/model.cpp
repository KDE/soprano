/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
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

#include "model.h"

#include "node.h"
#include "statement.h"
#include "statementiterator.h"

#include <QtCore/QList>


class Soprano::Model::Private
{
public:
};

Soprano::Model::Model()
    : QObject( 0 ),
      Error::ErrorCache(),
      d( new Private() )
{
}

Soprano::Model::~Model()
{
    delete d;
}

// private no-op constructor not meant to be used
Soprano::Model::Model( const Model& )
    : QObject(),
      Error::ErrorCache(),
      d( 0 )
{
}


// private no-op operator not meant to be used
Soprano::Model& Soprano::Model::operator=( const Model& )
{
    return *this;
}


bool Soprano::Model::containsContext( const Node &context ) const
{
    return listStatementsInContext( context ).next();
}


Soprano::StatementIterator Soprano::Model::listStatements( const Node& subject, const Node& predicate, const Node& object, const Node& context ) const
{
    return listStatements( Statement( subject, predicate, object, context ) );
}


Soprano::StatementIterator Soprano::Model::listStatements() const
{
    return listStatements( Statement() );
}


Soprano::StatementIterator Soprano::Model::listStatementsInContext( const Node &context ) const
{
    return listStatements( Statement( Node(), Node(), Node(), context ) );
}


Soprano::Error::ErrorCode Soprano::Model::removeStatement( const Node& subject, const Node& predicate, const Node& object, const Node& context )
{
    return removeStatement( Statement( subject, predicate, object, context ) );
}


Soprano::Error::ErrorCode Soprano::Model::removeStatements( const QList<Statement> &statements )
{
    for ( QList<Statement>::const_iterator it = statements.constBegin();
          it != statements.constEnd(); ++it ) {
        Error::ErrorCode c = removeStatement( *it );
        if ( c != Error::ErrorNone ) {
            return c;
        }
    }
    return Error::ErrorNone;
}


Soprano::Error::ErrorCode Soprano::Model::removeContext( const Node& context )
{
    if( context.isValid() ) {
        return removeAllStatements( Statement( Node(), Node(), Node(), context ) );
    }
    else {
        setError( "Cannot remove the empty context", Error::ErrorInvalidArgument );
        return Error::ErrorInvalidArgument;
    }
}


Soprano::Error::ErrorCode Soprano::Model::removeAllStatements( const Node& subject, const Node& predicate, const Node& object, const Node& context )
{
    return removeAllStatements( Statement( subject, predicate, object, context ) );
}


Soprano::Error::ErrorCode Soprano::Model::removeAllStatements()
{
    return removeAllStatements( Statement() );
}


Soprano::Error::ErrorCode Soprano::Model::addStatement( const Node& subject, const Node& predicate, const Node& object, const Node& context )
{
    return addStatement( Statement( subject, predicate, object, context ) );
}


Soprano::Error::ErrorCode Soprano::Model::addStatements( const QList<Statement> &statements )
{
    for ( QList<Statement>::const_iterator it = statements.constBegin(); it != statements.constEnd(); ++it ) {
        Error::ErrorCode c = addStatement( *it );
        if ( Error::ErrorNone != c ) {
            return c;
        }
    }

    return Error::ErrorNone;
}


bool Soprano::Model::containsStatement( const Node& subject, const Node& predicate, const Node& object, const Node& context ) const
{
    return containsStatement( Statement( subject, predicate, object, context ) );
}


bool Soprano::Model::containsAnyStatement( const Node& subject, const Node& predicate, const Node& object, const Node& context ) const
{
    return containsAnyStatement( Statement( subject, predicate, object, context ) );
}


Soprano::Error::ErrorCode Soprano::Model::write( QTextStream &os ) const
{
    StatementIterator it = listStatements();
    while ( it.next() ) {
        os << *it << endl;
    }
    return Error::ErrorNone;
}

#include "model.moc"
