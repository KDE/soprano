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
        : NAMESPACE( "http://www.w3.org/2000/01/rdf-schema#" ),
          CLASS( "http://www.w3.org/2000/01/rdf-schema#Class" ),
          COMMENT( "http://www.w3.org/2000/01/rdf-schema#comment" ),
          DATATYPE( "http://www.w3.org/2000/01/rdf-schema#Datatype" ),
          DOMAIN( "http://www.w3.org/2000/01/rdf-schema#domain" ),
          RANGE( "http://www.w3.org/2000/01/rdf-schema#range" ),
          LABEL( "http://www.w3.org/2000/01/rdf-schema#label" ),
          SUBCLASSOF( "http://www.w3.org/2000/01/rdf-schema#subClassOf" ),
          SUBPROPERTYOF( "http://www.w3.org/2000/01/rdf-schema#subPropertyOf" ),
          SEEALSO( "http://www.w3.org/2000/01/rdf-schema#seeAlso" ),
          RESOURCE( "http://www.w3.org/2000/01/rdf-schema#Resource" ),
          MEMBER( "http://www.w3.org/2000/01/rdf-schema#member" ),
          LITERAL( "http://www.w3.org/2000/01/rdf-schema#Literal" ),
          ISDEFINEDBY( "http://www.w3.org/2000/01/rdf-schema#isDefinedBy" ),
          CONTAINER( "http://www.w3.org/2000/01/rdf-schema#Container" ),
          CONTAINERMEMBERSHIPPROPERTY( "http://www.w3.org/2000/01/rdf-schema#ContainerMembershipProperty" ) {
    }

    QUrl NAMESPACE;
    QUrl CLASS;
    QUrl COMMENT;
    QUrl DATATYPE;
    QUrl DOMAIN;
    QUrl RANGE;
    QUrl LABEL;
    QUrl SUBCLASSOF;
    QUrl SUBPROPERTYOF;
    QUrl SEEALSO;
    QUrl RESOURCE;
    QUrl MEMBER;
    QUrl LITERAL;
    QUrl ISDEFINEDBY;
    QUrl CONTAINER;
    QUrl CONTAINERMEMBERSHIPPROPERTY;
};


Q_GLOBAL_STATIC( Rdfs, rdfs );


QUrl Soprano::Vocabulary::RDFS::NAMESPACE()
{
    return rdfs()->NAMESPACE;
}


QUrl Soprano::Vocabulary::RDFS::CLASS()
{
    return rdfs()->CLASS;
}


QUrl Soprano::Vocabulary::RDFS::COMMENT()
{
    return rdfs()->COMMENT;
}


QUrl Soprano::Vocabulary::RDFS::DATATYPE()
{
    return rdfs()->DATATYPE;
}


QUrl Soprano::Vocabulary::RDFS::DOMAIN()
{
    return rdfs()->DOMAIN;
}


QUrl Soprano::Vocabulary::RDFS::RANGE()
{
    return rdfs()->RANGE;
}


QUrl Soprano::Vocabulary::RDFS::LABEL()
{
    return rdfs()->LABEL;
}


QUrl Soprano::Vocabulary::RDFS::SUBCLASSOF()
{
    return rdfs()->SUBCLASSOF;
}


QUrl Soprano::Vocabulary::RDFS::SUBPROPERTYOF()
{
    return rdfs()->SUBPROPERTYOF;
}


QUrl Soprano::Vocabulary::RDFS::SEEALSO()
{
    return rdfs()->SEEALSO;
}


QUrl Soprano::Vocabulary::RDFS::RESOURCE()
{
    return rdfs()->RESOURCE;
}


QUrl Soprano::Vocabulary::RDFS::MEMBER()
{
    return rdfs()->MEMBER;
}


QUrl Soprano::Vocabulary::RDFS::LITERAL()
{
    return rdfs()->LITERAL;
}


QUrl Soprano::Vocabulary::RDFS::ISDEFINEDBY()
{
    return rdfs()->ISDEFINEDBY;
}


QUrl Soprano::Vocabulary::RDFS::CONTAINER()
{
    return rdfs()->CONTAINER;
}


QUrl Soprano::Vocabulary::RDFS::CONTAINERMEMBERSHIPPROPERTY()
{
    return rdfs()->CONTAINERMEMBERSHIPPROPERTY;
}
