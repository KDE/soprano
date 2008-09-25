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

#include "cluceneindex.h"
#include "cluceneutils.h"
#include "clucenedocumentwrapper.h"
#include "tstring.h"
#include "indexqueryhititeratorbackend.h"

#include "clucene-config.h"

#include <CLucene.h>

#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QMutex>
#include <QtCore/QThread>
#include <QtCore/QMutexLocker>



// Stuff to remember:
// indexwriter needs to be closed for deletion
// indexreader needs to be closed after usage of writer

uint Soprano::qHash( const Soprano::Node& node )
{
    return qHash( node.toString() );
}

class Soprano::Index::CLuceneIndex::Private
{
public:
    Private()
        : indexDir( 0 ),
          indexReader( 0 ),
          indexWriter( 0 ),
          analyzer( 0 ),
          searcher( 0 ),
          deleteAnalyzer( false ),
          transactionID( 0 ) {
    }

    lucene::store::Directory* indexDir;
    lucene::index::IndexReader* indexReader;
    lucene::index::IndexWriter* indexWriter;
    lucene::analysis::Analyzer* analyzer;
    lucene::search::IndexSearcher* searcher;

    bool deleteAnalyzer;

    // if > 0 a transaction is running
    int transactionID;
    QHash<Node, lucene::document::Document*> documentCache;

    QMutex mutex;

    bool indexPresent() const {
        return lucene::index::IndexReader::indexExists( indexDir );
    }

    lucene::index::IndexReader* getIndexReader() {
        if ( !indexReader ) {
            try {
                closeWriter();
                indexReader = lucene::index::IndexReader::open( indexDir, false );
            } catch (CLuceneError& err) {
                qDebug() << "(Soprano::Index::CLuceneIndex) could not create reader " << err.what();
                throw err;
            }
        }
        return indexReader;
    }

    lucene::index::IndexWriter* getIndexWriter() {
        if ( !indexWriter ) {
            try {
                closeReader();
                indexWriter = _CLNEW lucene::index::IndexWriter( indexDir, analyzer, !indexPresent(), false );
            }
            catch( CLuceneError& err ) {
                qDebug() << "(Soprano::Index::CLuceneIndex) could not create index writer " << err.what();
                throw err;
            }
        }
        return indexWriter;
    }

    lucene::search::Searcher* getIndexSearcher() {
        if ( !searcher ) {
            closeWriter();
            searcher = _CLNEW lucene::search::IndexSearcher( indexDir );
        }
        return searcher;
    }

    // FIXME: what if someone is still using a Hits instance?
    void closeReader() {
        if ( searcher ) {
            try {
                searcher->close();
            }
            catch ( CLuceneError& err ) {
                qDebug() << "(Soprano::Index::CLuceneIndex) could not close index seacher " << err.what();
            }
            delete searcher;
            searcher = 0;
        }
        if ( indexReader ) {
            try {
                indexReader->close();
            }
            catch ( CLuceneError& err ) {
                qDebug() << "(Soprano::Index::CLuceneIndex) could not close index reader " << err.what();
            }
            delete indexReader;
            indexReader = 0;
        }
    }

    void closeWriter() {
        if ( indexWriter ) {
            try {
                indexWriter->close();
            }
            catch( CLuceneError& err ) {
                qDebug() << "(Soprano::Index::CLuceneError) unable to close IndexWriter: " << err.what();
            }
            delete indexWriter;
            indexWriter = 0;
        }
    }

    QString getId( const Soprano::Node& node ) {
        if ( node.isResource() ) {
            return node.toString();
        }
        else if ( node.isBlank() ) {
            return bnodeIdPrefix() + node.toString();
        }
        else {
            return QString();
        }
    }

    lucene::document::Document* getDocument( lucene::index::Term* term ) {
        if ( lucene::index::TermDocs* docs = getIndexReader()->termDocs( term ) ) {
            if ( docs->next() ) {
                int32_t docId = docs->doc();
                if ( docs->next() ) {
                    qDebug() << "(Soprano::Index::CLuceneIndex) Multiple documents for resource " << term->text();
                }
                docs->close();
                delete docs;
                return getIndexReader()->document( docId );
            }
            docs->close();
            delete docs;
        }
        return 0;
    }


    /**
     * A doc is empty if it only contains the fields id and text
     */
    static bool docEmpty( lucene::document::Document* doc ) {
        int cnt = 0;
        lucene::document::DocumentFieldEnumeration* it = doc->fields();
        while ( it->hasMoreElements() ) {
            it->nextElement();
            ++cnt;
        }
        delete it;
        return cnt <= 2;
    }

    lucene::document::Document* getDocument( const Node& resource ) {
        // check if the resource is already cached
        QHash<Node, lucene::document::Document*>::const_iterator it = documentCache.find( resource );
        if ( it != documentCache.constEnd() ) {
            return *it;
        }
        else {
            QString id = getId( resource );
            lucene::document::Document* document = 0;
            // step 1: create a new document
            document = _CLNEW lucene::document::Document;
            CLuceneDocumentWrapper docWrapper( document );
            docWrapper.addID( id );

            // step 2: check if the resource already exists
            lucene::index::Term idTerm( idFieldName().data(), TString( id ).data() );
            lucene::document::Document* oldDoc = 0;
            if ( indexPresent() ) {
                oldDoc = getDocument( &idTerm );
            }

            // step 3: copy the existing fields from the document into our cache
            if ( oldDoc ) {
                lucene::document::DocumentFieldEnumeration* fields = oldDoc->fields();
                while ( fields->hasMoreElements() ) {
                    lucene::document::Field* field = fields->nextElement();
                    if ( Index::isPropertyField( TString( field->name(), true ) ) ) {
                        docWrapper.addProperty( field->name(), field->stringValue() );
                    }
                }
                delete( fields );
                delete( oldDoc );
            }

            // step 4: add the new doc to our cache
            documentCache[resource] = document;

            return document;
        }
    }

    void commit() {
        // update all documents

        // remove previous instances
        if ( indexPresent() ) {
            for ( QHash<Node, lucene::document::Document*>::iterator it = documentCache.begin();
                  it != documentCache.end(); ++it ) {
                lucene::document::Document* doc = it.value();
                if ( const TCHAR* id = doc->get( idFieldName().data() ) ) { // this check is only for testing, it should NEVER fail
                    lucene::index::Term* idTerm = _CLNEW lucene::index::Term( idFieldName().data(), id );
                    getIndexReader()->deleteDocuments( idTerm );
                    _CLDECDELETE( idTerm );
                }
            }
        }

        // add the updated ones
        for ( QHash<Node, lucene::document::Document*>::iterator it = documentCache.begin();
              it != documentCache.end(); ++it ) {
            lucene::document::Document* doc = it.value();
            // never add empty docs
            if ( !docEmpty( doc ) ) {
                getIndexWriter()->addDocument( doc );
            }
            delete( doc );
        }

        documentCache.clear();
    }
};


Soprano::Index::CLuceneIndex::CLuceneIndex( lucene::analysis::Analyzer* analyzer )
    : d( new Private() )
{
    d->analyzer = analyzer;
    if ( !d->analyzer ) {
        d->analyzer = _CLNEW lucene::analysis::standard::StandardAnalyzer();
        d->deleteAnalyzer = true;
    }
}


Soprano::Index::CLuceneIndex::~CLuceneIndex()
{
    close();

    if ( d->deleteAnalyzer ) {
        delete( d->analyzer );
    }
    delete d;
}


bool Soprano::Index::CLuceneIndex::open( const QString& folder, bool force )
{
    qDebug() << "CLuceneIndex::open in thread " << QThread::currentThreadId();
    close();

    clearError();

    QMutexLocker lock( &d->mutex );

    try {
        d->indexDir = lucene::store::FSDirectory::getDirectory( QFile::encodeName( folder ).data(), !QFile::exists( folder ) );
    }
    catch( CLuceneError& err ) {
        qDebug() << "(Soprano::Index::CLuceneIndex) failed to open index folder " << folder << " (" << err.what() << ")";
        setError( exceptionToError( err ) );
        return false;
    }

    if ( !d->indexDir ) {
        return false;
    }

    // get rid of any locks that may have been left by previous (crashed)
    // sessions
    if ( lucene::index::IndexReader::isLocked( d->indexDir ) ) {
        if ( force ) {
            qDebug() << "(Soprano::Index::CLuceneIndex::open) unlocking directory " << folder;
            lucene::index::IndexReader::unlock( d->indexDir );
        }
        else {
            qDebug() << "(Soprano::Index::CLuceneIndex::open) index folder is locked! Make sure no other clucene processes work on the same data." << folder;

            setError( QString( "Index folder '%1' is locked." ).arg( folder ) );
            return false;
        }
    }

    qDebug() << "CLuceneIndex::open done in thread " << QThread::currentThreadId();

    return true;
}


void Soprano::Index::CLuceneIndex::close()
{
    qDebug() << "CLuceneIndex::close in thread " << QThread::currentThreadId();
    clearError();
    if ( d->transactionID ) {
        closeTransaction( d->transactionID );
    }
    QMutexLocker lock( &d->mutex );
    d->closeReader();
    d->closeWriter();
    qDebug() << "CLuceneIndex::close done in thread " << QThread::currentThreadId();
}


bool Soprano::Index::CLuceneIndex::isOpen() const
{
    QMutexLocker lock( &d->mutex );
    return ( d->indexDir != 0 );
}


lucene::document::Document* Soprano::Index::CLuceneIndex::documentForResource( const Node& resource )
{
    qDebug() << "CLuceneIndex::documentForResource in thread " << QThread::currentThreadId();
    QMutexLocker lock( &d->mutex );

    clearError();

    if ( d->transactionID == 0 ) {
        setError( "No transaction started." );
        qDebug() << "CLuceneIndex::documentForResource done in thread " << QThread::currentThreadId();
        return 0;
    }
    try {
        qDebug() << "CLuceneIndex::documentForResource done in thread " << QThread::currentThreadId();
        return d->getDocument( resource );
    }
    catch( CLuceneError& err ) {
        qDebug() << "(Soprano::Index::CLuceneIndex) Exception occured: " << err.what();
        setError( exceptionToError( err ) );
        qDebug() << "CLuceneIndex::documentForResource done in thread " << QThread::currentThreadId();
        return 0;
    }
}

int Soprano::Index::CLuceneIndex::startTransaction()
{
//    qDebug() << "CLuceneIndex::startTransaction in thread " << QThread::currentThreadId();
    QMutexLocker lock( &d->mutex );

    clearError();

    if ( d->transactionID == 0 ) {
        // FIXME: use a random number
        d->transactionID = 1;
//        qDebug() << "CLuceneIndex::startTransaction done in thread " << QThread::currentThreadId();
        return d->transactionID;
    }
    else {
        setError( "Previous transaction still open." );
//        qDebug() << "CLuceneIndex::startTransaction done in thread " << QThread::currentThreadId();
        return 0;
    }
}


bool Soprano::Index::CLuceneIndex::closeTransaction( int id )
{
//    qDebug() << "CLuceneIndex::closeTransaction in thread " << QThread::currentThreadId();
    QMutexLocker lock( &d->mutex );

    if ( id == d->transactionID && id > 0 ) {
        clearError();
        try {
            d->commit();
        }
        catch( CLuceneError& err ) {
            setError( exceptionToError( err ) );
//            qDebug() << "CLuceneIndex::closeTransaction done in thread " << QThread::currentThreadId();
            return false;
        }

        d->transactionID = 0;
//        qDebug() << "CLuceneIndex::closeTransaction done in thread " << QThread::currentThreadId();
        return true;
    }
    else {
        setError( QString( "Invalid transaction ID: %1" ).arg( id ) );
//        qDebug() << "CLuceneIndex::closeTransaction done in thread " << QThread::currentThreadId();
        return false;
    }
}


Soprano::Error::ErrorCode Soprano::Index::CLuceneIndex::addStatement( const Soprano::Statement& statement )
{
//    qDebug() << "CLuceneIndex::addStatement in thread " << QThread::currentThreadId();
    QMutexLocker lock( &d->mutex );

    if ( !statement.object().isLiteral() ) {
        qDebug() << "(Soprano::Index::CLuceneIndex::addStatement) only adding statements with literal object type.";
        setError( "Only indexing of literal objects supported." );
//        qDebug() << "CLuceneIndex::addStatement done in thread " << QThread::currentThreadId();
        return Error::ErrorUnknown;
    }

    clearError();

    QString field = statement.predicate().toString();
    QString text = statement.object().toString();

    bool success = true;

    try {
        lucene::document::Document* document = d->getDocument( statement.subject() );
        if ( document ) {
            CLuceneDocumentWrapper docWrapper( document );
            docWrapper.addProperty( field, text );
            if ( d->transactionID == 0 ) {
                d->commit();
            }
            success = true;
        }
        else {
            // error already set in documentForResource
            success = false;
        }
    }
    catch( CLuceneError& err ) {
        setError( exceptionToError( err ) );
        success = false;
    }

//    qDebug() << "CLuceneIndex::addStatement done in thread " << QThread::currentThreadId();
    return success ? Error::ErrorNone : Error::ErrorUnknown;
}


Soprano::Error::ErrorCode Soprano::Index::CLuceneIndex::removeStatement( const Soprano::Statement& statement )
{
//    qDebug() << "CLuceneIndex::removeStatement in thread " << QThread::currentThreadId();
    QMutexLocker lock( &d->mutex );

    if ( !statement.object().isLiteral() ) {
        qDebug() << "(Soprano::Index::CLuceneIndex::removeStatement) only adding statements with literal object type.";
        setError( Error::Error( "Only indexing of literal objects supported." ) );
//        qDebug() << "CLuceneIndex::removeStatement done in thread " << QThread::currentThreadId();
        return Error::ErrorUnknown;
    }

    clearError();

    // just for speed
    if ( !d->indexPresent() ) {
//        qDebug() << "CLuceneIndex::removeStatement done in thread " << QThread::currentThreadId();
        return Error::ErrorNone;
    }

    bool success = false;

    try {
        lucene::document::Document* document = d->getDocument( statement.subject() );
        if ( document ) {
            CLuceneDocumentWrapper docWrapper( document );
            docWrapper.removeProperty( statement.predicate().toString(), statement.object().toString() );
            if ( d->transactionID == 0 ) {
                d->commit();
            }
            success = true;
        }
    }
    catch( CLuceneError& err ) {
        qDebug() << "(Soprano::Index::CLuceneIndex::removeStatement) Exception occured: " << err.what();
        setError( exceptionToError( err ) );
        success = false;
    }

//    qDebug() << "CLuceneIndex::removeStatement done in thread " << QThread::currentThreadId();
    return success ? Error::ErrorNone : Error::ErrorUnknown;
}


Soprano::Node Soprano::Index::CLuceneIndex::getResource( lucene::document::Document* document )
{
    qDebug() << "CLuceneIndex::getResource in thread " << QThread::currentThreadId();
    QString id = TString( document->get( idFieldName().data() ) );
    if ( id.startsWith( bnodeIdPrefix() ) ) {
        qDebug() << "CLuceneIndex::getResource done in thread " << QThread::currentThreadId();
        return Soprano::Node( id.mid( bnodeIdPrefix().length() ) );
    }
    else {
        qDebug() << "CLuceneIndex::getResource done in thread " << QThread::currentThreadId();
        return Soprano::Node( QUrl( id ) );
    }
}


Soprano::Iterator<Soprano::Index::QueryHit> Soprano::Index::CLuceneIndex::search( const QString& query )
{
    clearError();
    try {
        lucene::search::Query* q = lucene::queryParser::QueryParser::parse( TString( query ).data(), textFieldName().data(), d->analyzer );
        Iterator<QueryHit> hits = search( q );
        // FIXME: is it possible to use the stupid CLucene ref counting here?
        if ( !hits.isValid() ) {
            delete q;
        }
        return hits;
    }
    catch( CLuceneError& err ) {
        qDebug() << "search" << query << "failed: " << err.what();
        setError( exceptionToError( err ) );
        return Iterator<QueryHit>();
    }
}


Soprano::Iterator<Soprano::Index::QueryHit> Soprano::Index::CLuceneIndex::search( lucene::search::Query* query )
{
    QMutexLocker lock( &d->mutex );

    if ( query ) {
        clearError();
        try {
            lucene::search::Hits* hits = d->getIndexSearcher()->search( query );
            if ( hits ) {
                return new QueryHitIteratorBackend( hits, query );
            }
            else {
                return Iterator<QueryHit>();
            }
        }
        catch( CLuceneError& err ) {
            qDebug() << "search failed: " << err.what();
            setError( exceptionToError( err ) );
            return Iterator<QueryHit>();
        }
    }
    else {
        setError( "Invalid CLucene query" );
        return Iterator<QueryHit>();
    }
}


int Soprano::Index::CLuceneIndex::resourceCount() const
{
    QMutexLocker lock( &d->mutex );

    clearError();
    try  {
        lucene::index::IndexReader* reader = d->getIndexReader();
        return reader->numDocs();
    }
    catch( CLuceneError& err ) {
        qDebug() << "(Soprano::Index::CLuceneIndex) failed to dump index.";
        setError( exceptionToError( err ) );
        return -1;
    }
}

#if 0
double Soprano::Index::CLuceneIndex::getScore( const Soprano::Node& resource, const QString& query )
{
    clearError();
    try {
        lucene::search::Query* q = lucene::queryParser::QueryParser::parse( TString( query ).data(), textFieldName().data(), d->analyzer );
        double score = getScore( resource, q );
        delete( q );
        return score;
    }
    catch( CLuceneError& err ) {
        qDebug() << "search failed: " << err.what();
        setError( exceptionToError( err ) );
        return 0.0;
    }
}


double Soprano::Index::CLuceneIndex::getScore( const Soprano::Node& resource, lucene::search::Query* query )
{
    QMutexLocker lock( &d->mutex );

    clearError();
    try {
        // rewrite the query
        lucene::index::Term queryTerm( idFieldName().data(), TString( d->getId( resource ) ).data() );
        lucene::search::TermQuery idQuery( &queryTerm );
        lucene::search::BooleanQuery combinedQuery;
        combinedQuery.add( &idQuery, true, false );
        combinedQuery.add( query, true, false );

        // fetch the score when the URI matches the original query
        lucene::search::TopDocs* docs = static_cast<lucene::search::Searchable*>( d->getIndexSearcher() )->_search( &combinedQuery, 0, 1 );
        double r = -1.0;
        if ( docs->totalHits > 0 ) {
#ifdef CL_VERSION_19_OR_GREATER
            r = docs->scoreDocs[0].score;
#else
            r = docs->scoreDocs[0]->score;
#endif
        }
        _CLDELETE( docs );
        return r;
    }
    catch( CLuceneError& err ) {
        qDebug() << "search failed: " << err.what();
        setError( exceptionToError( err ) );
        return 0.0;
    }
}
#endif


void Soprano::Index::CLuceneIndex::dump( QTextStream& s ) const
{
    QMutexLocker lock( &d->mutex );

    clearError();
    try  {
        lucene::index::IndexReader* reader = d->getIndexReader();

        for ( int i = 0; i < reader->numDocs(); ++i ) {
            lucene::document::Document* doc = reader->document( i );
            s << "Document " << i << endl
              << "====================" << endl;
            lucene::document::DocumentFieldEnumeration* e = doc->fields();
            while ( e->hasMoreElements() ) {
                lucene::document::Field* field = e->nextElement();

                s << TString( field->name(), true ).toQString() << ": "
                  << TString( field->stringValue(), true ).toQString()
                  << endl;
            }
            s << endl;
            _CLDELETE( e );
            _CLDELETE( doc );
        }

        s << flush;
    }
    catch( CLuceneError& err ) {
        qDebug() << "(Soprano::Index::CLuceneIndex) failed to dump index.";
        setError( exceptionToError( err ) );
    }
}


QString Soprano::Index::CLuceneIndex::defaultSearchField()
{
    return textFieldName().toQString();
}


void Soprano::Index::CLuceneIndex::clear()
{
    int numDocs = d->getIndexReader()->numDocs();
    for ( int i = 0; i < numDocs; ++i ) {
        d->getIndexReader()->deleteDocument( i );
    }
    d->closeReader();
}


void Soprano::Index::CLuceneIndex::optimize()
{
    d->getIndexWriter()->optimize();
}
