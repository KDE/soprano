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

#include "indexfiltermodel.h"
#include "cluceneindex.h"
#include "queryhitwrapperresultiteratorbackend.h"
#include <soprano/queryresultiterator.h>
#include <soprano/statementiterator.h>

#include <CLucene.h>

#include <QtCore/QThread>
#include <QtCore/QDebug>

class Soprano::Index::IndexFilterModel::Private
{
public:
    Private()
        : deleteIndex( false ),
          index( 0 ),
          transactionCacheSize( 1 ),
          transactionCacheId( 0 ),
          transactionCacheCount( 0 ) {
    }

    bool deleteIndex;
    CLuceneIndex* index;

    int transactionCacheSize;
    int transactionCacheId;
    int transactionCacheCount;

    void startTransaction() {
        if ( transactionCacheSize > 1 && !transactionCacheId ) {
            transactionCacheId = index->startTransaction();
            transactionCacheCount = 0;
        }

        ++transactionCacheCount;
    }

    void closeTransaction() {
        if ( transactionCacheCount >= transactionCacheSize && transactionCacheId ) {
            index->closeTransaction( transactionCacheId );
            transactionCacheCount = 0;
            transactionCacheId = 0;
        }
    }
};


Soprano::Index::IndexFilterModel::IndexFilterModel( const QString& dir, Soprano::Model* model )
    : FilterModel( model ),
      d( new Private() )
{
    d->index = new CLuceneIndex();
    d->index->open( dir );
    d->deleteIndex = true;
}


Soprano::Index::IndexFilterModel::IndexFilterModel( CLuceneIndex* index, Soprano::Model* model )
    : FilterModel( model ),
      d( new Private() )
{
    d->index = index;
    d->deleteIndex = false;
}


Soprano::Index::IndexFilterModel::~IndexFilterModel()
{
    if ( d->deleteIndex ) {
        delete d->index;
    }
    delete d;
}


Soprano::Index::CLuceneIndex* Soprano::Index::IndexFilterModel::index() const
{
    return d->index;
}

Soprano::Error::ErrorCode Soprano::Index::IndexFilterModel::addStatement( const Soprano::Statement &statement )
{
//    qDebug() << "IndexFilterModel::addStatement(" << statement << ") in thread " << QThread::currentThreadId();
    if ( !FilterModel::containsStatement( statement ) ) {
        Error::ErrorCode c = FilterModel::addStatement( statement );
        if ( c == Error::ErrorNone && statement.object().isLiteral() ) {
            d->startTransaction();
            c = d->index->addStatement( statement );
            d->closeTransaction();
            if ( c != Error::ErrorNone ) {
                setError( d->index->lastError() );
            }
        }
//        qDebug() << "IndexFilterModel::addStatement done in thread " << QThread::currentThreadId();
        return c;
    }
    else {
//        qDebug() << "IndexFilterModel::addStatement done in thread " << QThread::currentThreadId();
        return Error::ErrorNone;
    }
}


Soprano::Error::ErrorCode Soprano::Index::IndexFilterModel::removeStatement( const Soprano::Statement &statement )
{
    Error::ErrorCode c = FilterModel::removeStatement( statement );
    if ( c == Error::ErrorNone ) {
        d->startTransaction();
        c = d->index->removeStatement( statement );
        d->closeTransaction();
        if ( c != Error::ErrorNone ) {
            setError( d->index->lastError() );
        }
    }
    return c;
}


Soprano::Error::ErrorCode Soprano::Index::IndexFilterModel::removeAllStatements( const Soprano::Statement &statement )
{
    // since we use backends that directly implement this method we don't know which
    // statements are actually removed (there is no signal for that)
    // so we have to check that up front
    // FIXME: can we handle this is the CLuceneIndex?
    Soprano::StatementIterator it = parentModel()->listStatements( statement );
    while ( it.next() ) {
        Statement s = *it;
        if ( s.object().isLiteral() ) {
            d->startTransaction();
            Error::ErrorCode c = d->index->removeStatement( *it );
            d->closeTransaction();
            if ( c != Error::ErrorNone ) {
                setError( d->index->lastError() );
                return c;
            }
        }
    }
    it.close();

    return FilterModel::removeAllStatements( statement );
}


Soprano::QueryResultIterator Soprano::Index::IndexFilterModel::executeQuery( const QString& query, Query::QueryLanguage language, const QString& userQueryLanguage ) const
{
    if ( language == Query::QueryLanguageUser && userQueryLanguage.toLower() == "lucene" ) {

        // commit any changes in the index
        d->transactionCacheCount = d->transactionCacheSize;
        d->closeTransaction();

        clearError();
        Iterator<QueryHit> res = index()->search( query );
        if ( !res.isValid() ) {
            setError( index()->lastError() );
        }
        return new QueryHitWrapperResultIteratorBackend( res );
    }
    else {
        return FilterModel::executeQuery( query, language, userQueryLanguage );
    }
}


void Soprano::Index::IndexFilterModel::setTransactionCacheSize( int size )
{
    d->transactionCacheSize = qMax( 1, size );
    d->closeTransaction();
}


int Soprano::Index::IndexFilterModel::transactionCacheSize() const
{
    return d->transactionCacheSize;
}
