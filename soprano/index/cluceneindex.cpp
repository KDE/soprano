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
#include "wstring.h"

#include "clucene-config.h"

#include <CLucene.h>

#include <QtCore/QFile>
#include <QtCore/QDebug>



// Stuff to remember:
// indexwriter needs to be closed for deletion
// indexreader needs to be closed after usage of writer

uint qHash( const Soprano::Node& node )
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

    bool indexPresent() const {
        return lucene::index::IndexReader::indexExists( indexDir );
    }

    lucene::index::IndexReader* getIndexReader() {
        if ( !indexReader ) {
            try {
                closeWriter();
                indexReader = lucene::index::IndexReader::open( indexDir );
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
                indexWriter = _CLNEW lucene::index::IndexWriter( indexDir, analyzer, !indexPresent() );
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
            _CLDELETE( searcher );
            searcher = 0;
        }
        if ( indexReader ) {
            try {
                indexReader->close();
            }
            catch ( CLuceneError& err ) {
                qDebug() << "(Soprano::Index::CLuceneIndex) could not close index reader " << err.what();
            }
            _CLDELETE( indexReader );
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
            _CLDELETE( indexWriter );
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
        lucene::index::TermDocs* docs = getIndexReader()->termDocs( term );
        if ( docs->next() ) {
            int32_t docId = docs->doc();
            if ( docs->next() ) {
                qDebug() << "(Soprano::Index::CLuceneIndex) Multiple documents for resource " << term->text();
            }
            docs->close();
            return getIndexReader()->document( docId );
        }
        docs->close();
        return 0;
    }

    static bool isPropertyField( const WString& fieldName ) {
        return ( idFieldName() != fieldName &&
                 textFieldName() != fieldName );
    }
};


Soprano::Index::CLuceneIndex::CLuceneIndex( lucene::analysis::Analyzer* analyzer )
    : d( new Private() )
{
    d->analyzer = analyzer;
    if ( !d->analyzer ) {
        d->analyzer = _CLNEW lucene::analysis::SimpleAnalyzer();
        d->deleteAnalyzer = true;
    }
}


Soprano::Index::CLuceneIndex::~CLuceneIndex()
{
    close();

    if ( d->deleteAnalyzer ) {
        _CLDELETE( d->analyzer );
    }
    delete d;
}


bool Soprano::Index::CLuceneIndex::open( const QString& folder )
{
    close();

    clearError();

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
        qDebug() << "(Soprano::Index::CLuceneIndex::open) Removing lock for folder " << folder;
        lucene::index::IndexReader::unlock( d->indexDir );
    }

    return true;
}


void Soprano::Index::CLuceneIndex::close()
{
    d->closeReader();
    d->closeWriter();
}


bool Soprano::Index::CLuceneIndex::isOpen() const
{
    return ( d->indexDir != 0 );
}


lucene::document::Document* Soprano::Index::CLuceneIndex::documentForResource( const Node& resource )
{
    clearError();

    if ( d->transactionID == 0 ) {
        setError( "No transaction started." );
        return 0;
    }

    // check if the resource is already cached
    QHash<Node, lucene::document::Document*>::const_iterator it = d->documentCache.find( resource );
    if ( it != d->documentCache.constEnd() ) {
        return *it;
    }
    else {
        QString id = d->getId( resource );
        lucene::document::Document* document = 0;
        try {
            // step 1: create a new document
            document = _CLNEW lucene::document::Document;
            CLuceneDocumentWrapper docWrapper( document );
            docWrapper.addID( id );

            // step 2: check if the resource already exists
            lucene::index::Term idTerm( idFieldName().data(), WString( id ).data() );
            lucene::document::Document* oldDoc = 0;
            if ( d->indexPresent() ) {
                oldDoc = d->getDocument( &idTerm );
            }

            // step 3: copy the existing fields from the document into our cache
            if ( oldDoc ) {
                CLuceneDocumentWrapper oldDocWrapper( oldDoc );
                lucene::document::DocumentFieldEnumeration* fields = oldDoc->fields();
                while ( fields->hasMoreElements() ) {
                    lucene::document::Field* field = fields->nextElement();
                    if ( Private::isPropertyField( WString( field->name(), true ) ) ) {
                        docWrapper.addProperty( field->name(), field->stringValue() );
                    }
                }
            }

            // step 4: add the new doc to our cache
            d->documentCache[resource] = document;

            return document;
        }
        catch( CLuceneError& err ) {
            qDebug() << "(Soprano::Index::CLuceneIndex) Exception occured: " << err.what();
            setError( exceptionToError( err ) );
            return 0;
        }
    }
}

int Soprano::Index::CLuceneIndex::startTransaction()
{
    clearError();

    if ( d->transactionID == 0 ) {
        // FIXME: use a random number
        d->transactionID = 1;
        return d->transactionID;
    }
    else {
        setError( "Previous transaction still open." );
        return 0;
    }
}


bool Soprano::Index::CLuceneIndex::closeTransaction( int id )
{
    if ( id == d->transactionID && id > 0 ) {
        // update all documents
        for ( QHash<Node, lucene::document::Document*>::iterator it = d->documentCache.begin();
              it != d->documentCache.end(); ++it ) {
            lucene::document::Document* doc = it.value();

            try  {
                // remove previous instances
                if ( d->indexPresent() ) {
                    lucene::index::Term idTerm( idFieldName().data(), doc->get( idFieldName().data() ) );
                    d->getIndexReader()->deleteDocuments( &idTerm );
                }

                // add the updated one
                d->getIndexWriter()->addDocument( doc );
            }
            catch( CLuceneError& err ) {
                qDebug() << "Failed to store document: " << err.what();
                setError( exceptionToError( err ) );
                return false;
            }

            _CLDELETE( doc );
        }

        clearError();
        d->documentCache.clear();
        d->transactionID = 0;
        return true;
    }
    else {
        setError( QString( "Invalid transaction ID: %1" ).arg( id ) );
        return false;
    }
}


Soprano::Error::ErrorCode Soprano::Index::CLuceneIndex::addFieldToResourceDocument( const Node& resource, lucene::document::Field* field )
{
    bool localTransaction = false;
    if ( d->transactionID == 0 ) {
        startTransaction();
        localTransaction = true;
    }

    bool success = true;
    if ( lucene::document::Document* doc = documentForResource( resource ) ) {
        doc->add( *field );
    }

    if ( localTransaction ) {
        closeTransaction( d->transactionID );
    }

    return success ? Error::ERROR_NONE : Error::ERROR_UNKNOWN;
}


Soprano::Error::ErrorCode Soprano::Index::CLuceneIndex::addStatement( const Soprano::Statement& statement )
{
    if ( !statement.object().isLiteral() ) {
        qDebug() << "(Soprano::Index::CLuceneIndex::addStatement) only adding statements with literal object type.";
        setError( Error::Error( "Only indexing of literal objects supported." ) );
        return Error::ERROR_UNKNOWN;
    }

    clearError();

    bool localTransaction = false;
    if ( d->transactionID == 0 ) {
        startTransaction();
        localTransaction = true;
    }

    QString field = statement.predicate().toString();
    QString text = statement.object().toString();

    bool success = true;

    lucene::document::Document* document = documentForResource( statement.subject() );
    if ( document ) {
        CLuceneDocumentWrapper docWrapper( document );
        docWrapper.addProperty( field, text );
        success = true;
    }
    else {
        // error already set in documentForResource
        success = false;
    }

    if ( localTransaction ) {
        closeTransaction( d->transactionID );
    }

    return success ? Error::ERROR_NONE : Error::ERROR_UNKNOWN;
}


Soprano::Error::ErrorCode Soprano::Index::CLuceneIndex::removeStatement( const Soprano::Statement& statement )
{
    if ( !statement.object().isLiteral() ) {
        qDebug() << "(Soprano::Index::CLuceneIndex::removeStatement) only adding statements with literal object type.";
        setError( Error::Error( "Only indexing of literal objects supported." ) );
        return Error::ERROR_UNKNOWN;
    }

    clearError();

    // just for speed
    if ( !d->indexPresent() ) {
        return Error::ERROR_NONE;
    }

    bool localTransaction = false;
    if ( d->transactionID == 0 ) {
        startTransaction();
        localTransaction = true;
    }

    bool success = false;

    lucene::document::Document* document = documentForResource( statement.subject() );
    if ( document ) {
        try {
            CLuceneDocumentWrapper docWrapper( document );
            docWrapper.removeProperty( statement.predicate().toString(), statement.object().toString() );
            success = true;
        }
        catch( CLuceneError& err ) {
            qDebug() << "(Soprano::Index::CLuceneIndex) Exception occured: " << err.what();
            setError( exceptionToError( err ) );
            success = false;
        }
    }

    if ( localTransaction ) {
        closeTransaction( d->transactionID );
    }

    return success ? Error::ERROR_NONE : Error::ERROR_UNKNOWN;
}


Soprano::Node Soprano::Index::CLuceneIndex::getResource( int documentNumber )
{
    Soprano::Node node;
    if ( lucene::document::Document* document = d->getIndexSearcher()->doc( documentNumber ) ) {
        node = getResource( document );
        _CLDELETE( document );
    }
    return node;
}


Soprano::Node Soprano::Index::CLuceneIndex::getResource( lucene::document::Document* document )
{
    QString id = WString( document->get( idFieldName().data() ) );
    if ( id.startsWith( bnodeIdPrefix() ) ) {
        return Soprano::Node( QUrl( id.mid( bnodeIdPrefix().length() ) ), Soprano::Node::BlankNode );
    }
    else {
        return Soprano::Node( QUrl( id ) );
    }
}


lucene::search::Hits* Soprano::Index::CLuceneIndex::search( const QString& query )
{
    clearError();
    try {
        lucene::search::Query* q = lucene::queryParser::QueryParser::parse( WString( query ).data(), textFieldName().data(), d->analyzer );
        lucene::search::Hits* hits = search( q );
        _CLDELETE( q );
        return hits;
    }
    catch( CLuceneError& err ) {
        qDebug() << "search failed: " << err.what();
        setError( exceptionToError( err ) );
        return 0;
    }
}


lucene::search::Hits* Soprano::Index::CLuceneIndex::search( lucene::search::Query* query )
{
    clearError();
    try {
        return d->getIndexSearcher()->search( query );
    }
    catch( CLuceneError& err ) {
        qDebug() << "search failed: " << err.what();
        setError( exceptionToError( err ) );
        return 0;
    }
}


double Soprano::Index::CLuceneIndex::getScore( const Soprano::Node& resource, const QString& query )
{
    clearError();
    try {
        lucene::search::Query* q = lucene::queryParser::QueryParser::parse( WString( query ).data(), textFieldName().data(), d->analyzer );
        double score = getScore( resource, q );
        _CLDELETE( q );
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
    clearError();
    try {
        // rewrite the query
        lucene::index::Term queryTerm( idFieldName().data(), WString( d->getId( resource ) ).data() );
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


void Soprano::Index::CLuceneIndex::dump( QTextStream& s ) const
{
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

                s << WString( field->name(), true ).toQString() << ": " << WString( field->stringValue(), true ).toQString() << endl;
            }
            s << endl;
        }

        s << flush;
    }
    catch( CLuceneError& err ) {
        qDebug() << "(Soprano::Index::CLuceneIndex) failed to dump index.";
        setError( exceptionToError( err ) );
    }
}

