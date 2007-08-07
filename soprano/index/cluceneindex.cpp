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

#include <CLucene.h>

#include <QtCore/QFile>
#include <QtCore/QDebug>



// Stuff to remember:
// indexwriter needs to be closed for deletion
// indexreader needs to be closed after usage of writer

class Soprano::Index::CLuceneIndex::Private
{
public:
    Private()
        : indexDir( 0 ),
          indexReader( 0 ),
          indexWriter( 0 ),
          analyzer( 0 ),
          queryParser( 0 ),
          searcher( 0 ),
          deleteAnalyzer( false ) {
    }

    lucene::store::Directory* indexDir;
    lucene::index::IndexReader* indexReader;
    lucene::index::IndexWriter* indexWriter;
    lucene::analysis::Analyzer* analyzer;
    lucene::queryParser::QueryParser* queryParser;
    lucene::search::IndexSearcher* searcher;

    bool deleteAnalyzer;

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
                indexWriter = new lucene::index::IndexWriter( indexDir, analyzer, !indexPresent() );
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
            searcher = new lucene::search::IndexSearcher( indexDir );
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
        d->analyzer = new lucene::analysis::SimpleAnalyzer();
        d->deleteAnalyzer = true;
    }
    d->queryParser = new lucene::queryParser::QueryParser( textFieldName().data(), analyzer );
}


Soprano::Index::CLuceneIndex::~CLuceneIndex()
{
    close();

    _CLDELETE( d->queryParser );
    if ( d->deleteAnalyzer ) {
        delete d->analyzer;
    }
    delete d;
}


bool Soprano::Index::CLuceneIndex::open( const QString& folder )
{
    close();

    try {
        d->indexDir = lucene::store::FSDirectory::getDirectory( QFile::encodeName( folder ).data(), true );
    }
    catch( CLuceneError& err ) {
        qDebug() << "(Soprano::Index::CLuceneIndex) failed to open index folder " << folder << " (" << err.what() << ")";
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

    _CLDELETE( d->indexDir );
}


bool Soprano::Index::CLuceneIndex::isOpen() const
{
    return ( d->indexDir != 0 );
}


int Soprano::Index::CLuceneIndex::addStatement( const Soprano::Statement& statement )
{
    if ( !statement.object().isLiteral() ) {
        qDebug() << "(Soprano::Index::CLuceneIndex::addStatement) only adding statements with literal object type.";
        return -1;
    }

    QString field = statement.predicate().toString();
    QString text = statement.object().toString();

    QString id = d->getId( statement.subject() );
    lucene::document::Document* document = 0;
    try {
        lucene::index::Term idTerm( idFieldName().data(), WString( id ).data() );
        if ( d->indexPresent() ) {
            document = d->getDocument( &idTerm );
        }

        if ( !document ) {
            // create a new document
            lucene::document::Document newDoc;
            CLuceneDocumentWrapper docWrapper( &newDoc );
            docWrapper.addID( id );
            docWrapper.addProperty( field, text );
            d->getIndexWriter()->addDocument( &newDoc );
        }
        else {
            // update the document if the predicate does not exist already
            CLuceneDocumentWrapper docWrapper( document );
            if ( !docWrapper.hasProperty( field, text ) ) {
                lucene::document::Document newDoc;
                CLuceneDocumentWrapper newDocWrapper( &newDoc );
                newDocWrapper.addID( id );

                // copy all fields
                lucene::document::DocumentFieldEnumeration* fields = document->fields();
                while ( fields->hasMoreElements() ) {
                    lucene::document::Field* field = fields->nextElement();
                    if ( Private::isPropertyField( WString( field->name(), true ) ) ) {
                        newDoc.add( *field );
                    }
                }

                newDocWrapper.addProperty( field, text );
                d->getIndexReader()->deleteDocuments( &idTerm );
                d->getIndexWriter()->addDocument( &newDoc );
            }
        }
    }
    catch( CLuceneError& err ) {
        qDebug() << "(Soprano::Index::CLuceneIndex) Exception occured: " << err.what();
        return -1;
    }

    return 0;
}


int Soprano::Index::CLuceneIndex::removeStatement( const Soprano::Statement& statement )
{
    if ( !statement.object().isLiteral() ) {
        qDebug() << "(Soprano::Index::CLuceneIndex::removeStatement) only adding statements with literal object type.";
        return -1;
    }

    if ( !d->indexPresent() ) {
        return 0;
    }

    QString id = d->getId( statement.subject() );
    lucene::index::Term idTerm( idFieldName().data(), WString( id ).data() );
    lucene::document::Document* document = 0;

    try {
        document = d->getDocument( &idTerm );

        if ( document ) {
            // determine the values used in the index for this triple
            QString newFieldName = statement.predicate().toString();
            QString newText = statement.object().toString();

            CLuceneDocumentWrapper documentWrapper( document );

            // see if this triple occurs in this Document
            if ( documentWrapper.hasProperty( newFieldName, newText ) ) {
                // if the Document only has one predicate field, we can remove the
                // document
                int nrProperties = documentWrapper.numberOfPropertyFields();
                if ( !nrProperties ) {
                    qDebug() << "(Soprano::Index::CLuceneIndex::removeStatement) no properties for id " << id;
                }
                else if (nrProperties == 1) {
                    d->getIndexReader()->deleteDocuments( &idTerm );
                }
                else {
                    // there are more triples encoded in this Document: remove the
                    // document and add a new Document without this triple
                    lucene::document::Document newDocument;
                    CLuceneDocumentWrapper newDocWrapper( &newDocument );
                    newDocWrapper.addID( id );

                    // copy all fields except the one we delete
                    lucene::document::DocumentFieldEnumeration* fields = document->fields();
                    while ( fields->hasMoreElements() ) {
                        lucene::document::Field* field = fields->nextElement();
                        WString fieldName( field->name(), true );
                        if ( Private::isPropertyField( fieldName ) &&
                             !( newFieldName == fieldName && newText == WString( field->stringValue(), true ) ) ) {
                            newDocument.add( *field );
                        }
                    }

                    d->getIndexReader()->deleteDocuments( &idTerm );
                    d->getIndexWriter()->addDocument( &newDocument );
                }
            }
        }
    }
    catch( CLuceneError& err ) {
        qDebug() << "(Soprano::Index::CLuceneIndex) Exception occured: " << err.what();
        return -1;
    }

    return 0;
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
    return search( d->queryParser->parse( WString( query ).data() ) );
}


lucene::search::Hits* Soprano::Index::CLuceneIndex::search( lucene::search::Query* query )
{
    return d->getIndexSearcher()->search( query );
}


double Soprano::Index::CLuceneIndex::getScore( const Soprano::Node& resource, const QString& query )
{
    return getScore( resource, d->queryParser->parse( WString( query ).data() ) );
}


double Soprano::Index::CLuceneIndex::getScore( const Soprano::Node& resource, lucene::search::Query* query )
{
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
        r = docs->scoreDocs[0]->score;
    }
    _CLDELETE( docs );
    return r;
}


void Soprano::Index::CLuceneIndex::dump( QTextStream& s ) const
{
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
    }
}

