/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2008 Sebastian Trueg <trueg@kde.org>
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
#include "indexfiltermodel_p.h"
#include "cluceneindex.h"
#include "queryhitwrapperresultiteratorbackend.h"
#include "queryresultiterator.h"
#include "statementiterator.h"
#include "qurlhash.h"

#include <CLucene.h>

#include <QtCore/QThread>
#include <QtCore/QSet>
#include <QtCore/QRegExp>
#include <QtCore/QDebug>


Soprano::Index::IndexFilterModelPrivate::IndexFilterModelPrivate()
    : deleteIndex( false ),
      index( 0 ),
      transactionCacheSize( 1 ),
      transactionCacheId( 0 ),
      transactionCacheCount( 0 )
{
}


void Soprano::Index::IndexFilterModelPrivate::startTransaction()
{
    if ( transactionCacheSize > 1 && !transactionCacheId ) {
        transactionCacheId = index->startTransaction();
        transactionCacheCount = 0;
    }

    ++transactionCacheCount;
}


void Soprano::Index::IndexFilterModelPrivate::closeTransaction()
{
    if ( transactionCacheCount >= transactionCacheSize && transactionCacheId ) {
        index->closeTransaction( transactionCacheId );
        transactionCacheCount = 0;
        transactionCacheId = 0;
    }
}


bool Soprano::Index::IndexFilterModelPrivate::storeStatement( const Statement& statement ) const
{
    return !indexOnlyPredicates.contains( statement.predicate().uri() );
}


bool Soprano::Index::IndexFilterModelPrivate::indexStatement( const Statement& statement ) const
{
    return statement.object().isLiteral() || forceIndexPredicates.contains( statement.predicate().uri() );
}




Soprano::Index::IndexFilterModel::IndexFilterModel( const QString& dir, Soprano::Model* model )
    : FilterModel( model ),
      d( new IndexFilterModelPrivate() )
{
    d->index = new CLuceneIndex();
    d->index->open( dir );
    d->deleteIndex = true;
}


Soprano::Index::IndexFilterModel::IndexFilterModel( CLuceneIndex* index, Soprano::Model* model )
    : FilterModel( model ),
      d( new IndexFilterModelPrivate() )
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


Soprano::Error::ErrorCode Soprano::Index::IndexFilterModel::addStatement( const Soprano::Statement& statement )
{
//    qDebug() << "IndexFilterModel::addStatement(" << statement << ") in thread " << QThread::currentThreadId();
    bool store = d->storeStatement( statement );

    // TODO: avoid the containsStatement here. Can we tell clucene to ignore duplicates?
    if ( !store ||
         !FilterModel::containsStatement( statement ) ) {
        Error::ErrorCode c = Error::ErrorNone;
        if( store ) {
            c = FilterModel::addStatement( statement );
        }

        bool index = d->indexStatement( statement );

        if ( c == Error::ErrorNone && index ) {
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


Soprano::Error::ErrorCode Soprano::Index::IndexFilterModel::removeStatement( const Soprano::Statement& statement )
{
    // here we simply ignore the indexOnlyPredicates
    Error::ErrorCode c = FilterModel::removeStatement( statement );
    if ( c == Error::ErrorNone &&
         d->indexStatement( statement ) ) {
        d->startTransaction();
        c = d->index->removeStatement( statement );
        d->closeTransaction();
        if ( c != Error::ErrorNone ) {
            setError( d->index->lastError() );
        }
    }
    return c;
}


Soprano::Error::ErrorCode Soprano::Index::IndexFilterModel::removeAllStatements( const Soprano::Statement& statement )
{
    // here we simply ignore the indexOnlyPredicates

    // since we use backends that directly implement this method we don't know which
    // statements are actually removed (there is no signal for that)
    // so we have to check that up front
    // FIXME: can we handle this is the CLuceneIndex?
    Soprano::StatementIterator it = parentModel()->listStatements( statement );
    while ( it.next() ) {
        Statement s = *it;
        if ( d->indexStatement( s ) ) {
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


void Soprano::Index::IndexFilterModel::rebuildIndex()
{
    d->closeTransaction();

    // clear the index
    // -----------------------------
    d->index->clear();

    // rebuild the index
    // -----------------------------

    // select all resources
    QueryResultIterator it = FilterModel::executeQuery( "select distinct ?r where { ?r ?p ?o . }",
                                                        Query::QueryLanguageSparql );

    while ( it.next() ) {
        int id = d->index->startTransaction();
        Node res = it.binding( "r" );

        // and re-add all the literal statements and those in forceIndexPredicates (we can savely ignore the context here)
        QStringList filters("(isLiteral(?o) && str(?o)!='')");
        foreach( const QUrl& p, d->forceIndexPredicates ) {
            filters << QString("?p = %1").arg(Soprano::Node(p).toN3());
        }
        QueryResultIterator it2 = FilterModel::executeQuery( QString( "select distinct ?p ?o where { %1 ?p ?o . FILTER(%2) . }" )
                                                             .arg( res.toN3() )
                                                             .arg( filters.join( " || " ) ),
                                                             Query::QueryLanguageSparql );
        while ( it2.next() ) {
            d->index->addStatement( Statement( res, it2.binding( "p" ), it2.binding( "o" ) ) );
        }

        d->index->closeTransaction( id );
    }
}



void Soprano::Index::IndexFilterModel::addIndexOnlyPredicate( const QUrl& predicate )
{
    d->indexOnlyPredicates.insert( predicate );
}


void Soprano::Index::IndexFilterModel::setIndexOnlyPredicates( const QList<QUrl>& predicates )
{
    d->indexOnlyPredicates = predicates.toSet();
}


QList<QUrl> Soprano::Index::IndexFilterModel::indexOnlyPredicates() const
{
    return d->indexOnlyPredicates.toList();
}


void Soprano::Index::IndexFilterModel::addForceIndexPredicate( const QUrl& predicate )
{
    d->forceIndexPredicates.insert( predicate );
}


void Soprano::Index::IndexFilterModel::setForceIndexPredicates( const QList<QUrl>& predicates )
{
    d->forceIndexPredicates = predicates.toSet();
}


QList<QUrl> Soprano::Index::IndexFilterModel::forceIndexPredicates() const
{
    return d->forceIndexPredicates.toList();
}


void Soprano::Index::IndexFilterModel::optimizeIndex()
{
    d->index->optimize();
}


QString Soprano::Index::IndexFilterModel::encodeStringForLuceneQuery( const QString& s )
{
    /* Chars to escape: + - && || ! ( ) { } [ ] ^ " ~  : \ */

    static QRegExp rx( "([\\-" + QRegExp::escape( "+&|!(){}[]^\"~:\\" ) + "])" );
    QString es( s );
    es.replace( rx, "\\\\1" );
    return es;
}


QString Soprano::Index::IndexFilterModel::encodeUriForLuceneQuery( const QUrl& uri )
{
    return encodeStringForLuceneQuery( QString::fromAscii( uri.toEncoded() ) );
}
