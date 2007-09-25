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

#include "rdfs.h"

class Rdfs
{
public:
    Rdfs()
        : rdfsNs( "http://www.w3.org/2000/01/rdf-schema#" ),
          rdfsClass( "http://www.w3.org/2000/01/rdf-schema#Class" ),
          rdfsComment( "http://www.w3.org/2000/01/rdf-schema#comment" ),
          rdfsDatatype( "http://www.w3.org/2000/01/rdf-schema#Datatype" ),
          rdfsDomain( "http://www.w3.org/2000/01/rdf-schema#domain" ),
          rdfsRange( "http://www.w3.org/2000/01/rdf-schema#range" ),
          rdfsLabel( "http://www.w3.org/2000/01/rdf-schema#label" ),
          rdfsSubClassOf( "http://www.w3.org/2000/01/rdf-schema#subClassOf" ),
          rdfsSubPropertyOf( "http://www.w3.org/2000/01/rdf-schema#subPropertyOf" ),
          rdfsSeeAlso( "http://www.w3.org/2000/01/rdf-schema#seeAlso" ),
          rdfsResource( "http://www.w3.org/2000/01/rdf-schema#Resource" ),
          rdfsMember( "http://www.w3.org/2000/01/rdf-schema#member" ),
          rdfsLiteral( "http://www.w3.org/2000/01/rdf-schema#Literal" ),
          rdfsIsDefinedBy( "http://www.w3.org/2000/01/rdf-schema#isDefinedBy" ),
          rdfsContainer( "http://www.w3.org/2000/01/rdf-schema#Container" ),
          rdfsContainerMembershipProperty( "http://www.w3.org/2000/01/rdf-schema#ContainerMembershipProperty" ) {
    }

    QUrl rdfsNs;
    QUrl rdfsClass;
    QUrl rdfsComment;
    QUrl rdfsDatatype;
    QUrl rdfsDomain;
    QUrl rdfsRange;
    QUrl rdfsLabel;
    QUrl rdfsSubClassOf;
    QUrl rdfsSubPropertyOf;
    QUrl rdfsSeeAlso;
    QUrl rdfsResource;
    QUrl rdfsMember;
    QUrl rdfsLiteral;
    QUrl rdfsIsDefinedBy;
    QUrl rdfsContainer;
    QUrl rdfsContainerMembershipProperty;
};


Q_GLOBAL_STATIC( Rdfs, rdfs )


QUrl Soprano::Vocabulary::RDFS::rdfsNamespace()
{
    return rdfs()->rdfsNs;
}


QUrl Soprano::Vocabulary::RDFS::Class()
{
    return rdfs()->rdfsClass;
}


QUrl Soprano::Vocabulary::RDFS::comment()
{
    return rdfs()->rdfsComment;
}


QUrl Soprano::Vocabulary::RDFS::Datatype()
{
    return rdfs()->rdfsDatatype;
}


QUrl Soprano::Vocabulary::RDFS::domain()
{
    return rdfs()->rdfsDomain;
}


QUrl Soprano::Vocabulary::RDFS::range()
{
    return rdfs()->rdfsRange;
}


QUrl Soprano::Vocabulary::RDFS::label()
{
    return rdfs()->rdfsLabel;
}


QUrl Soprano::Vocabulary::RDFS::subClassOf()
{
    return rdfs()->rdfsSubClassOf;
}


QUrl Soprano::Vocabulary::RDFS::subPropertyOf()
{
    return rdfs()->rdfsSubPropertyOf;
}


QUrl Soprano::Vocabulary::RDFS::seeAlso()
{
    return rdfs()->rdfsSeeAlso;
}


QUrl Soprano::Vocabulary::RDFS::Resource()
{
    return rdfs()->rdfsResource;
}


QUrl Soprano::Vocabulary::RDFS::member()
{
    return rdfs()->rdfsMember;
}


QUrl Soprano::Vocabulary::RDFS::Literal()
{
    return rdfs()->rdfsLiteral;
}


QUrl Soprano::Vocabulary::RDFS::isDefinedBy()
{
    return rdfs()->rdfsIsDefinedBy;
}


QUrl Soprano::Vocabulary::RDFS::Container()
{
    return rdfs()->rdfsContainer;
}


QUrl Soprano::Vocabulary::RDFS::ContainerMembershipProperty()
{
    return rdfs()->rdfsContainerMembershipProperty;
}
