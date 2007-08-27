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

#include "clucenedocumentwrapper.h"
#include "cluceneutils.h"
#include "wstring.h"

#include <CLucene/document/Document.h>
#include <CLucene/document/Field.h>

#include <QtCore/QDebug>


class Soprano::Index::CLuceneDocumentWrapper::Private
{
public:
    lucene::document::Document* document;
};


Soprano::Index::CLuceneDocumentWrapper::CLuceneDocumentWrapper( lucene::document::Document* doc )
    : d( new Private() )
{
    d->document = doc;
}


Soprano::Index::CLuceneDocumentWrapper::~CLuceneDocumentWrapper()
{
    delete d;
}


using lucene::document::Field;

void Soprano::Index::CLuceneDocumentWrapper::addProperty( const WString& field, const WString& text )
{
    // FIXME: Do we really need to store the values? after all we have them in the RDF store anyway!
    // store this predicate (YES, the CLucene API is that bad. We actually put in Fields allocated on the heap here!)
    d->document->add( *new Field( field.data(), text.data(), /*true, false, false*/ Field::STORE_YES|Field::INDEX_NO|Field::TERMVECTOR_NO ) );

    // We don't have to recalculate the concatenated text: just add another
    // TEXT field and Lucene will take care of this. Additional advantage:
    // Lucene may be able to handle the invididual strings in a way that may
    // affect e.g. phrase and proximity searches (concatenation basically
    // means loss of information).
    //
    // (YES, the CLucene API is that bad. We actually put in Fields allocated on the heap here!)
    d->document->add( *new Field( textFieldName().data(), text.data(), /*false, true, true*/ Field::STORE_NO|Field::INDEX_TOKENIZED|Field::TERMVECTOR_YES ) );
}


bool Soprano::Index::CLuceneDocumentWrapper::hasProperty( const QString& field, const QString& text ) const
{
    WString wText( text );
    TCHAR** values = d->document->getValues( WString( field ).data() );
    while ( values ) {
        if ( WString( *values, true ) == wText ) {
            return true;
        }
        ++values;
    }

    return false;
}


int Soprano::Index::CLuceneDocumentWrapper::numberOfPropertyFields() const
{
    // ugly but fast hack, assumes that only the ID and the predicates are
    // stored.
    int size = 0;
    lucene::document::DocumentFieldEnumeration* fields = d->document->fields();
    while ( fields->hasMoreElements() ) {
        ++size;
        fields->nextElement();
    }
    _CLDELETE( fields );

    return qMax( 0, size - 1 );
}


void Soprano::Index::CLuceneDocumentWrapper::addID( const QString& id )
{
    // (YES, the CLucene API is that bad. We actually put in Fields allocated on the heap here!)
    d->document->add( *new lucene::document::Field( idFieldName().data(), WString( id ).data(), /*true, true, false*/ Field::STORE_YES|Field::INDEX_UNTOKENIZED|Field::TERMVECTOR_NO ) );
}


void Soprano::Index::CLuceneDocumentWrapper::save( lucene::index::IndexWriter* writer )
{
    writer->addDocument( d->document );
}
