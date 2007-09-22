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


QUrl Soprano::Vocabulary::RDFS::rdfsNamespace()
{
    return rdfs()->NAMESPACE;
}


QUrl Soprano::Vocabulary::RDFS::Class()
{
    return rdfs()->CLASS;
}


QUrl Soprano::Vocabulary::RDFS::comment()
{
    return rdfs()->COMMENT;
}


QUrl Soprano::Vocabulary::RDFS::Datatype()
{
    return rdfs()->DATATYPE;
}


QUrl Soprano::Vocabulary::RDFS::domain()
{
    return rdfs()->DOMAIN;
}


QUrl Soprano::Vocabulary::RDFS::range()
{
    return rdfs()->RANGE;
}


QUrl Soprano::Vocabulary::RDFS::label()
{
    return rdfs()->LABEL;
}


QUrl Soprano::Vocabulary::RDFS::subClassOf()
{
    return rdfs()->SUBCLASSOF;
}


QUrl Soprano::Vocabulary::RDFS::subPropertyOf()
{
    return rdfs()->SUBPROPERTYOF;
}


QUrl Soprano::Vocabulary::RDFS::seeAlso()
{
    return rdfs()->SEEALSO;
}


QUrl Soprano::Vocabulary::RDFS::Resource()
{
    return rdfs()->RESOURCE;
}


QUrl Soprano::Vocabulary::RDFS::member()
{
    return rdfs()->MEMBER;
}


QUrl Soprano::Vocabulary::RDFS::Literal()
{
    return rdfs()->LITERAL;
}


QUrl Soprano::Vocabulary::RDFS::isDefinedBy()
{
    return rdfs()->ISDEFINEDBY;
}


QUrl Soprano::Vocabulary::RDFS::Container()
{
    return rdfs()->CONTAINER;
}


QUrl Soprano::Vocabulary::RDFS::ContainerMembershipProperty()
{
    return rdfs()->CONTAINERMEMBERSHIPPROPERTY;
}
