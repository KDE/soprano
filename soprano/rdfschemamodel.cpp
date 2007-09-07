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

#include "rdfschemawrapper.h"
#include "model.h"
#include "statementiterator.h"
#include "rdf.h"
#include "rdfs.h"
#include "statement.h"


class Soprano::RdfSchemaWrapper::Private
{
public:
    Model* model;
};


Soprano::RdfSchemaWrapper::RdfSchemaWrapper( Model* model )
    : d( new Private() )
{
    d->model = model;
}


Soprano::RdfSchemaWrapper::~RdfSchemaWrapper()
{
    delete d;
}

Soprano::StatementIterator Soprano::RdfSchemaWrapper::classes() const
{
    return d->model->listStatements( Statement( Node(), Vocabulary::RDF::TYPE(), Vocabulary::RDFS::CLASS() ) );
}


Soprano::StatementIterator Soprano::RdfSchemaWrapper::directSubClassOf( const Node& subClass, const Node& superClass ) const
{
    return d->model->listStatements( Statement( subClass, Vocabulary::RDFS::SUBCLASSOF(), superClass ) );
}


Soprano::StatementIterator Soprano::RdfSchemaWrapper::directSubPropertyOf( const Node& subProperty, const Node& superProperty ) const
{
    return d->model->listStatements( Statement( subProperty, Vocabulary::RDFS::SUBPROPERTYOF(), superProperty ) );
}


Soprano::StatementIterator Soprano::RdfSchemaWrapper::directType( const Node& someClass, const Node& someType ) const
{
    return d->model->listStatements( Statement( someClass, Vocabulary::RDF::TYPE(), someType ) );
}


Soprano::StatementIterator Soprano::RdfSchemaWrapper::domain( const Node& prop, const Node& domain ) const
{
    return d->model->listStatements( Statement( prop, Vocabulary::RDFS::DOMAIN(), domain ) );
}


Soprano::StatementIterator Soprano::RdfSchemaWrapper::properties() const
{
    return d->model->listStatements( Statement( Node(), Vocabulary::RDF::TYPE(), Vocabulary::RDF::PROPERTY() ) );
}


Soprano::StatementIterator Soprano::RdfSchemaWrapper::range( const Node& prop, const Node& range ) const
{
    return d->model->listStatements( Statement( prop, Vocabulary::RDFS::RANGE(), range ) );
}


Soprano::StatementIterator Soprano::RdfSchemaWrapper::subClassOf( const Node& subClass, const Node& superClass ) const
{
    // FIXME: do a recursive search for all subclasses. Problem: We cannot wrap multiple results in one iterator yet
    return directSubClassOf( subClass, superClass );
}


Soprano::StatementIterator Soprano::RdfSchemaWrapper::subPropertyOf( const Node& subProperty, const Node& superProperty ) const
{
    // FIXME: do a recursive search for all subproperties. Problem: We cannot wrap multiple results in one iterator yet
    return directSubPropertyOf( subProperty, superProperty );
}


Soprano::StatementIterator Soprano::RdfSchemaWrapper::type( const Node& someClass, const Node& someType ) const
{
    // FIXME: do a recursive search for all subtypes. Problem: We cannot wrap multiple results in one iterator yet
    return directType( someClass, someType );
}


bool Soprano::RdfSchemaWrapper::isClass( const Node& resource ) const
{
    return d->model->containsAnyStatement( Statement( resource,
                                                      Vocabulary::RDF::TYPE(),
                                                      Vocabulary::RDFS::CLASS() ) );
}


bool Soprano::RdfSchemaWrapper::isProperty( const Node& resource ) const
{
    return d->model->containsAnyStatement( Statement( resource,
                                                      Vocabulary::RDF::TYPE(),
                                                      Vocabulary::RDF::PROPERTY() ) );
}


bool Soprano::RdfSchemaWrapper::isDirectSubClassOf( const Node& subClass, const Node& superClass ) const
{
    return d->model->containsAnyStatement( Statement( subClass,
                                                      Vocabulary::RDFS::SUBCLASSOF(),
                                                      superClass ) );
}


bool Soprano::RdfSchemaWrapper::isDirectSubPropertyOf( const Node& subProperty, const Node& superProperty ) const
{
    return d->model->containsAnyStatement( Statement( subProperty,
                                                      Vocabulary::RDFS::SUBPROPERTYOF(),
                                                      superProperty ) );
}


bool Soprano::RdfSchemaWrapper::isDirectType( const Node& someClass, const Node& someType ) const
{
    return d->model->containsAnyStatement( Statement( someClass,
                                                      Vocabulary::RDF::TYPE(),
                                                    someType ) );
}


bool Soprano::RdfSchemaWrapper::isSubClassOf( const Node& subClass, const Node& superClass ) const
{
    return subClassOf( subClass, superClass ).next();
}


bool Soprano::RdfSchemaWrapper::isSubPropertyOf( const Node& subProperty, const Node& superProperty ) const
{
    return subPropertyOf( subProperty, superProperty ).next();
}


bool Soprano::RdfSchemaWrapper::isType( const Node& someClass, const Node& someType ) const
{
    return type( someClass, someType ).next();
}


Soprano::Error::Error Soprano::RdfSchemaWrapper::lastError() const
{
    return d->model->lastError();
}
