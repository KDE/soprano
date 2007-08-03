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

class Rdf
{
public:
    Rdf()
        : rdfNamespace( "http://www.w3.org/1999/02/22-rdf-syntax-ns#" ),
          rdfType( "http://www.w3.org/1999/02/22-rdf-syntax-ns#type" ),
          rdfProperty( "http://www.w3.org/1999/02/22-rdf-syntax-ns#Property" ) {
    }

    QUrl rdfNamespace;
    QUrl rdfType;
    QUrl rdfProperty;
};

Q_GLOBAL_STATIC( Rdf, rdf );


QUrl Soprano::Vocabulary::RDF::NAMESPACE()
{
    return rdf()->rdfNamespace;
}


QUrl Soprano::Vocabulary::RDF::TYPE()
{
    return rdf()->rdfType;
}


QUrl Soprano::Vocabulary::RDF::PROPERTY()
{
    return rdf()->rdfProperty;
}
