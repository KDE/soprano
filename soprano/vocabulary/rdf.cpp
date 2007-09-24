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

#include "rdf.h"

#define RDF_NS "http://www.w3.org/1999/02/22-rdf-syntax-ns#"

class Rdf
{
public:
    Rdf()
        : rdfNamespace( RDF_NS ),
          rdfType( RDF_NS"type" ),
          rdfProperty( RDF_NS"Property" ),
          rdfStatement( RDF_NS"Statement" ),
          rdfSubject( RDF_NS"subject" ),
          rdfPredicate( RDF_NS"predicate" ),
          rdfObject( RDF_NS"object" ),
          rdfBag( RDF_NS"Bag" ),
          rdfSeq( RDF_NS"Seq" ),
          rdfAlt( RDF_NS"Alt" ),
          rdfValue( RDF_NS"value" ),
          rdfList( RDF_NS"List" ),
          rdfNil( RDF_NS"nil" ),
          rdfFirst( RDF_NS"first" ),
          rdfRest( RDF_NS"rest" ),
          rdfXMLLiteral( RDF_NS"XMLLiteral" ) {
    }

    QUrl rdfNamespace;
    QUrl rdfType;
    QUrl rdfProperty;
    QUrl rdfStatement;
    QUrl rdfSubject;
    QUrl rdfPredicate;
    QUrl rdfObject;
    QUrl rdfBag;
    QUrl rdfSeq;
    QUrl rdfAlt;
    QUrl rdfValue;
    QUrl rdfList;
    QUrl rdfNil;
    QUrl rdfFirst;
    QUrl rdfRest;
    QUrl rdfXMLLiteral;
};

Q_GLOBAL_STATIC( Rdf, rdf )


QUrl Soprano::Vocabulary::RDF::rdfNamespace()
{
    return rdf()->rdfNamespace;
}


QUrl Soprano::Vocabulary::RDF::type()
{
    return rdf()->rdfType;
}


QUrl Soprano::Vocabulary::RDF::Property()
{
    return rdf()->rdfProperty;
}


QUrl Soprano::Vocabulary::RDF::Statement()
{
    return rdf()->rdfStatement;
}


QUrl Soprano::Vocabulary::RDF::subject()
{
    return rdf()->rdfSubject;
}


QUrl Soprano::Vocabulary::RDF::predicate()
{
    return rdf()->rdfPredicate;
}


QUrl Soprano::Vocabulary::RDF::object()
{
    return rdf()->rdfObject;
}


QUrl Soprano::Vocabulary::RDF::Bag()
{
    return rdf()->rdfBag;
}


QUrl Soprano::Vocabulary::RDF::Seq()
{
    return rdf()->rdfSeq;
}


QUrl Soprano::Vocabulary::RDF::Alt()
{
    return rdf()->rdfAlt;
}


QUrl Soprano::Vocabulary::RDF::value()
{
    return rdf()->rdfValue;
}


QUrl Soprano::Vocabulary::RDF::List()
{
    return rdf()->rdfList;
}


QUrl Soprano::Vocabulary::RDF::nil()
{
    return rdf()->rdfNil;
}


QUrl Soprano::Vocabulary::RDF::first()
{
    return rdf()->rdfFirst;
}


QUrl Soprano::Vocabulary::RDF::rest()
{
    return rdf()->rdfRest;
}


QUrl Soprano::Vocabulary::RDF::XMLLiteral()
{
    return rdf()->rdfXMLLiteral;
}
