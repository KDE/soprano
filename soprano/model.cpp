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
    : d( new Private() )
{
}

Soprano::Model::~Model()
{
    delete d;
}

Soprano::ErrorCode Soprano::Model::add( const Model &model )
{
    StatementIterator stmi = model.listStatements();
    if ( !stmi.isValid() ) {
        return ERROR_UNKNOW;
    }

    while ( stmi.hasNext() ) {
        ErrorCode c = add( stmi.next() );
        if ( ERROR_NONE != c ) {
            return c;
        }
    }

    return ERROR_NONE;
}

Soprano::ErrorCode Soprano::Model::add( const StatementIterator &iter, const Node &context )
{
    if ( !iter.isValid() ) {
        return ERROR_UNKNOW;
    }

    while ( iter.hasNext() ) {
        ErrorCode c = add( iter.next(), context );
        if ( ERROR_NONE != c ) {
            return c;
        }
    }

    return ERROR_NONE;
}

Soprano::ErrorCode Soprano::Model::add( const StatementIterator &iter )
{
    if ( !iter.isValid() ) {
        return ERROR_UNKNOW;
    }

    while ( iter.hasNext() ) {
        ErrorCode c = add( iter.next() );
        if ( ERROR_NONE != c ) {
            return c;
        }
    }

    return ERROR_NONE;
}

Soprano::ErrorCode Soprano::Model::add( const QList<Statement> &statements, const Node &context )
{
    for( QList<Statement>::const_iterator it = statements.constBegin();
         it != statements.constEnd(); ++it ) {
        ErrorCode c = add( *it, context );
        if ( ERROR_NONE != c ) {
            return c;
        }
    }

    return ERROR_NONE;
}

Soprano::ErrorCode Soprano::Model::add( const QList<Statement> &statements )
{
    // FIXME: why not call add with an empty context here?

    QListIterator<Statement> iter(statements);
    while ( iter.hasNext() ) {
        ErrorCode c = add( iter.next() );
        if ( ERROR_NONE != c ) {
            return c;
        }
    }

    return ERROR_NONE;
}

bool Soprano::Model::isEmpty() const
{
    return size() == 0;
}

Soprano::StatementIterator Soprano::Model::listStatements() const
{
    return listStatements( Statement() );
}

Soprano::StatementIterator Soprano::Model::listStatements( const Node &subject, const Node &predicate, const Node &object ) const
{
    return listStatements( Statement(subject, predicate, object) );
}

Soprano::ErrorCode Soprano::Model::removeAll( const Statement &statement, const Node &context )
{
    Statement s( statement );
    s.setContext( context );
    return removeAll( s );
}


Soprano::ErrorCode Soprano::Model::removeAll( const Statement &statement )
{
    StatementIterator iter = listStatements(statement);
    while ( iter.hasNext() ) {
       ErrorCode c = remove( iter.next() );
        if ( c != ERROR_NONE ) {
            return c;
        }
    }
    return ERROR_NONE;
}

Soprano::ErrorCode Soprano::Model::removeAll( const Node &subject, const Node &predicate, const Node &object )
{
    return removeAll( Statement( subject, predicate, object ) );
}

Soprano::ErrorCode Soprano::Model::removeAll()
{
    return removeAll( Statement() );
}

Soprano::ErrorCode Soprano::Model::remove( const QList<Statement> &statements )
{
    for ( QList<Statement>::const_iterator it = statements.constBegin();
          it != statements.constEnd(); ++it ) {
        ErrorCode c = remove( *it );
        if ( c != ERROR_NONE ) {
            return c;
        }
    }
    return ERROR_NONE;
}
