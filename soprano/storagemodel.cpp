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

#include "storagemodel.h"

#include "statement.h"
#include "statementiterator.h"

#include <QtCore/QList>


class Soprano::StorageModel::Private
{
public:
};

Soprano::StorageModel::StorageModel()
    : d( new Private() )
{
}

Soprano::StorageModel::~StorageModel()
{
    delete d;
}

Soprano::ErrorCode Soprano::StorageModel::add( const Model &model )
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

Soprano::ErrorCode Soprano::StorageModel::add( const StatementIterator &iter )
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

Soprano::ErrorCode Soprano::StorageModel::add( const QList<Statement> &statements )
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

bool Soprano::StorageModel::isEmpty() const
{
    return size() == 0;
}

bool Soprano::StorageModel::contains( const Statement &statement ) const
{
    return !listStatements( statement ).isEmpty();
}


bool Soprano::StorageModel::contains( const Node &context ) const
{
    return !listStatements( context ).isEmpty();
}

Soprano::StatementIterator Soprano::StorageModel::listStatements() const
{
    return listStatements( Statement() );
}


Soprano::StatementIterator Soprano::StorageModel::listStatements( const Node &context ) const
{
    return listStatements( Statement( Node(), Node(), Node(), context ) );
}


Soprano::ErrorCode Soprano::StorageModel::removeAll( const Statement &statement )
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

Soprano::ErrorCode Soprano::StorageModel::removeAll()
{
    return removeAll( Statement() );
}

Soprano::ErrorCode Soprano::StorageModel::remove( const QList<Statement> &statements )
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


Soprano::ErrorCode Soprano::StorageModel::write( QTextStream &os ) const
{
    StatementIterator it = listStatements();
    while ( it.hasNext() ) {
        os << it.next() << endl;
    }
}


Soprano::ErrorCode Soprano::StorageModel::print() const
{
    QTextStream os( stdout );
    write( os );
}

#include "storagemodel.moc"
