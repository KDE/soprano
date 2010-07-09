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

#include "rdfschemamodel.h"
#include "model.h"
#include "statementiterator.h"
#include "rdf.h"
#include "rdfs.h"
#include "statement.h"


class Soprano::RdfSchemaModel::Private
{
public:
};


Soprano::RdfSchemaModel::RdfSchemaModel( Model* model )
    : FilterModel( model ),
      d( new Private() )
{
}


Soprano::RdfSchemaModel::~RdfSchemaModel()
{
    delete d;
}


Soprano::StatementIterator Soprano::RdfSchemaModel::classes() const
{
    return parentModel()->listStatements( Statement( Node(), Vocabulary::RDF::type(), Vocabulary::RDFS::Class() ) );
}


Soprano::StatementIterator Soprano::RdfSchemaModel::directSubClassOf( const Node& subClass, const Node& superClass ) const
{
    return parentModel()->listStatements( Statement( subClass, Vocabulary::RDFS::subClassOf(), superClass ) );
}


Soprano::StatementIterator Soprano::RdfSchemaModel::directSubPropertyOf( const Node& subProperty, const Node& superProperty ) const
{
    return parentModel()->listStatements( Statement( subProperty, Vocabulary::RDFS::subPropertyOf(), superProperty ) );
}


Soprano::StatementIterator Soprano::RdfSchemaModel::directType( const Node& someClass, const Node& someType ) const
{
    return parentModel()->listStatements( Statement( someClass, Vocabulary::RDF::type(), someType ) );
}


Soprano::StatementIterator Soprano::RdfSchemaModel::domain( const Node& prop, const Node& domain ) const
{
    return parentModel()->listStatements( Statement( prop, Vocabulary::RDFS::domain(), domain ) );
}


Soprano::StatementIterator Soprano::RdfSchemaModel::properties() const
{
    return parentModel()->listStatements( Statement( Node(), Vocabulary::RDF::type(), Vocabulary::RDF::Property() ) );
}


Soprano::StatementIterator Soprano::RdfSchemaModel::range( const Node& prop, const Node& range ) const
{
    return parentModel()->listStatements( Statement( prop, Vocabulary::RDFS::range(), range ) );
}


Soprano::StatementIterator Soprano::RdfSchemaModel::subClassOf( const Node& subClass, const Node& superClass ) const
{
    // FIXME: do a recursive search for all subclasses. Problem: We cannot wrap multiple results in one iterator yet
    return directSubClassOf( subClass, superClass );
}


Soprano::StatementIterator Soprano::RdfSchemaModel::subPropertyOf( const Node& subProperty, const Node& superProperty ) const
{
    // FIXME: do a recursive search for all subproperties. Problem: We cannot wrap multiple results in one iterator yet
    return directSubPropertyOf( subProperty, superProperty );
}


Soprano::StatementIterator Soprano::RdfSchemaModel::type( const Node& someClass, const Node& someType ) const
{
    // FIXME: do a recursive search for all subtypes. Problem: We cannot wrap multiple results in one iterator yet
    return directType( someClass, someType );
}


bool Soprano::RdfSchemaModel::isClass( const Node& resource ) const
{
    return parentModel()->containsAnyStatement( Statement( resource,
                                                      Vocabulary::RDF::type(),
                                                      Vocabulary::RDFS::Class() ) );
}


bool Soprano::RdfSchemaModel::isProperty( const Node& resource ) const
{
    return parentModel()->containsAnyStatement( Statement( resource,
                                                      Vocabulary::RDF::type(),
                                                      Vocabulary::RDF::Property() ) );
}


bool Soprano::RdfSchemaModel::isDirectSubClassOf( const Node& subClass, const Node& superClass ) const
{
    return parentModel()->containsAnyStatement( Statement( subClass,
                                                      Vocabulary::RDFS::subClassOf(),
                                                      superClass ) );
}


bool Soprano::RdfSchemaModel::isDirectSubPropertyOf( const Node& subProperty, const Node& superProperty ) const
{
    return parentModel()->containsAnyStatement( Statement( subProperty,
                                                      Vocabulary::RDFS::subPropertyOf(),
                                                      superProperty ) );
}


bool Soprano::RdfSchemaModel::isDirectType( const Node& someClass, const Node& someType ) const
{
    return parentModel()->containsAnyStatement( Statement( someClass,
                                                      Vocabulary::RDF::type(),
                                                    someType ) );
}


bool Soprano::RdfSchemaModel::isSubClassOf( const Node& subClass, const Node& superClass ) const
{
    return subClassOf( subClass, superClass ).next();
}


bool Soprano::RdfSchemaModel::isSubPropertyOf( const Node& subProperty, const Node& superProperty ) const
{
    return subPropertyOf( subProperty, superProperty ).next();
}


bool Soprano::RdfSchemaModel::isType( const Node& someClass, const Node& someType ) const
{
    return type( someClass, someType ).next();
}
