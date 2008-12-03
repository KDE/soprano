/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2008 Sebastian Trueg <trueg@kde.org>
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

#include "indextransaction.h"
#include "indexfiltermodel_p.h"


Soprano::Index::IndexTransaction::IndexTransaction( Transaction* parent, IndexFilterModel* model, IndexFilterModelPrivate* p )
    : FilterTransaction( parent, model ),
      m_modelPrivate( p )
{
}


Soprano::Index::IndexTransaction::~IndexTransaction()
{
}


Error::ErrorCode Soprano::Index::IndexTransaction::addStatement( const Statement &statement )
{
    if( m_modelPrivate->indexStatement( statement ) &&
        !parentTransaction()->containsStatement( statement ) ) {
        m_addedStatements << statement;
    }
    m_removedStatements.removeAll( statement );
    return FilterTransaction::addStatement( statement );
}


Error::ErrorCode Soprano::Index::IndexTransaction::removeStatement( const Statement &statement )
{
    if( m_modelPrivate->indexStatement( statement ) &&
        parentTransaction()->containsStatement( statement ) ) {
        m_removedStatements << statement;
    }
    m_addedStatements.removeAll( statement );
    return FilterTransaction::removeStatement( statement );
}


Error::ErrorCode Soprano::Index::IndexTransaction::removeAllStatements( const Statement &statement )
{
    // clean addedStatements
    QSet<Statement>::iterator it = m_addedStatements.begin();
    while( it != m_addedStatements.end() ) {
        if( it->matches( statement ) ) {
            it = m_addedStatements.erase( it );
        }
        else {
            ++it;
        }
    }

    // see IndexFilterModel::removeAllStatements for details
    Soprano::StatementIterator it = parentTransaction()->listStatements( statement );
    while ( it.next() ) {
        Statement s = *it;
        if ( m_modelPrivate->indexStatement( s ) ) {
            m_removedStatements << s;
            m_addedStatements.removeAll( statement );
        }
    }
    it.close();

    return FilterTransaction::removeAllStatements( statement );
}


Error::ErrorCode Soprano::Index::IndexTransaction::doRollback()
{
    m_addedStatements.clear();
    m_removedStatements.clear();
    return FilterTransaction::doRollback();
}


Error::ErrorCode Soprano::Index::IndexTransaction::doCommit()
{
    if( FilterTransaction::doCommit() == Error::ErrorNone ) {
        m_modelPrivate->startTransaction();
        forach( const Statement& s, m_addedStatements ) {
            d->index->addStatement( statement );
        }
            d->closeTransaction();

    }
}
