/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2008 Sebastian Trueg <trueg@kde.org>
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

#include "owl.h"

class OwlPrivate
{
public:
    OwlPrivate()
        : owl_namespace( "http://www.w3.org/2002/07/owl#" ),
          owl_AllDifferent( "http://www.w3.org/2002/07/owl#AllDifferent" ),
          owl_AnnotationProperty( "http://www.w3.org/2002/07/owl#AnnotationProperty" ),
          owl_Class( "http://www.w3.org/2002/07/owl#Class" ),
          owl_DataRange( "http://www.w3.org/2002/07/owl#DataRange" ),
          owl_DatatypeProperty( "http://www.w3.org/2002/07/owl#DatatypeProperty" ),
          owl_DeprecatedClass( "http://www.w3.org/2002/07/owl#DeprecatedClass" ),
          owl_DeprecatedProperty( "http://www.w3.org/2002/07/owl#DeprecatedProperty" ),
          owl_FunctionalProperty( "http://www.w3.org/2002/07/owl#FunctionalProperty" ),
          owl_InverseFunctionalProperty( "http://www.w3.org/2002/07/owl#InverseFunctionalProperty" ),
          owl_ObjectProperty( "http://www.w3.org/2002/07/owl#ObjectProperty" ),
          owl_Ontology( "http://www.w3.org/2002/07/owl#Ontology" ),
          owl_OntologyProperty( "http://www.w3.org/2002/07/owl#OntologyProperty" ),
          owl_Restriction( "http://www.w3.org/2002/07/owl#Restriction" ),
          owl_SymmetricProperty( "http://www.w3.org/2002/07/owl#SymmetricProperty" ),
          owl_TransitiveProperty( "http://www.w3.org/2002/07/owl#TransitiveProperty" ),
          owl_allValuesFrom( "http://www.w3.org/2002/07/owl#allValuesFrom" ),
          owl_backwardCompatibleWith( "http://www.w3.org/2002/07/owl#backwardCompatibleWith" ),
          owl_cardinality( "http://www.w3.org/2002/07/owl#cardinality" ),
          owl_complementOf( "http://www.w3.org/2002/07/owl#complementOf" ),
          owl_differentFrom( "http://www.w3.org/2002/07/owl#differentFrom" ),
          owl_disjointWith( "http://www.w3.org/2002/07/owl#disjointWith" ),
          owl_distinctMembers( "http://www.w3.org/2002/07/owl#distinctMembers" ),
          owl_equivalentClass( "http://www.w3.org/2002/07/owl#equivalentClass" ),
          owl_equivalentProperty( "http://www.w3.org/2002/07/owl#equivalentProperty" ),
          owl_hasValue( "http://www.w3.org/2002/07/owl#hasValue" ),
          owl_imports( "http://www.w3.org/2002/07/owl#imports" ),
          owl_incompatibleWith( "http://www.w3.org/2002/07/owl#incompatibleWith" ),
          owl_intersectionOf( "http://www.w3.org/2002/07/owl#intersectionOf" ),
          owl_inverseOf( "http://www.w3.org/2002/07/owl#inverseOf" ),
          owl_maxCardinality( "http://www.w3.org/2002/07/owl#maxCardinality" ),
          owl_minCardinality( "http://www.w3.org/2002/07/owl#minCardinality" ),
          owl_onProperty( "http://www.w3.org/2002/07/owl#onProperty" ),
          owl_oneOf( "http://www.w3.org/2002/07/owl#oneOf" ),
          owl_priorVersion( "http://www.w3.org/2002/07/owl#priorVersion" ),
          owl_sameAs( "http://www.w3.org/2002/07/owl#sameAs" ),
          owl_someValuesFrom( "http://www.w3.org/2002/07/owl#someValuesFrom" ),
          owl_unionOf( "http://www.w3.org/2002/07/owl#unionOf" ),
          owl_versionInfo( "http://www.w3.org/2002/07/owl#versionInfo" ) {
    }

    QUrl owl_namespace;
    QUrl owl_AllDifferent;
    QUrl owl_AnnotationProperty;
    QUrl owl_Class;
    QUrl owl_DataRange;
    QUrl owl_DatatypeProperty;
    QUrl owl_DeprecatedClass;
    QUrl owl_DeprecatedProperty;
    QUrl owl_FunctionalProperty;
    QUrl owl_InverseFunctionalProperty;
    QUrl owl_ObjectProperty;
    QUrl owl_Ontology;
    QUrl owl_OntologyProperty;
    QUrl owl_Restriction;
    QUrl owl_SymmetricProperty;
    QUrl owl_TransitiveProperty;
    QUrl owl_allValuesFrom;
    QUrl owl_backwardCompatibleWith;
    QUrl owl_cardinality;
    QUrl owl_complementOf;
    QUrl owl_differentFrom;
    QUrl owl_disjointWith;
    QUrl owl_distinctMembers;
    QUrl owl_equivalentClass;
    QUrl owl_equivalentProperty;
    QUrl owl_hasValue;
    QUrl owl_imports;
    QUrl owl_incompatibleWith;
    QUrl owl_intersectionOf;
    QUrl owl_inverseOf;
    QUrl owl_maxCardinality;
    QUrl owl_minCardinality;
    QUrl owl_onProperty;
    QUrl owl_oneOf;
    QUrl owl_priorVersion;
    QUrl owl_sameAs;
    QUrl owl_someValuesFrom;
    QUrl owl_unionOf;
    QUrl owl_versionInfo;
};

Q_GLOBAL_STATIC( OwlPrivate, s_owl )

QUrl Soprano::Vocabulary::OWL::owlNamespace()
{
    return s_owl()->owl_namespace;
}

QUrl Soprano::Vocabulary::OWL::AllDifferent()
{
    return s_owl()->owl_AllDifferent;
}

QUrl Soprano::Vocabulary::OWL::AnnotationProperty()
{
    return s_owl()->owl_AnnotationProperty;
}

QUrl Soprano::Vocabulary::OWL::Class()
{
    return s_owl()->owl_Class;
}

QUrl Soprano::Vocabulary::OWL::DataRange()
{
    return s_owl()->owl_DataRange;
}

QUrl Soprano::Vocabulary::OWL::DatatypeProperty()
{
    return s_owl()->owl_DatatypeProperty;
}

QUrl Soprano::Vocabulary::OWL::DeprecatedClass()
{
    return s_owl()->owl_DeprecatedClass;
}

QUrl Soprano::Vocabulary::OWL::DeprecatedProperty()
{
    return s_owl()->owl_DeprecatedProperty;
}

QUrl Soprano::Vocabulary::OWL::FunctionalProperty()
{
    return s_owl()->owl_FunctionalProperty;
}

QUrl Soprano::Vocabulary::OWL::InverseFunctionalProperty()
{
    return s_owl()->owl_InverseFunctionalProperty;
}

QUrl Soprano::Vocabulary::OWL::ObjectProperty()
{
    return s_owl()->owl_ObjectProperty;
}

QUrl Soprano::Vocabulary::OWL::Ontology()
{
    return s_owl()->owl_Ontology;
}

QUrl Soprano::Vocabulary::OWL::OntologyProperty()
{
    return s_owl()->owl_OntologyProperty;
}

QUrl Soprano::Vocabulary::OWL::Restriction()
{
    return s_owl()->owl_Restriction;
}

QUrl Soprano::Vocabulary::OWL::SymmetricProperty()
{
    return s_owl()->owl_SymmetricProperty;
}

QUrl Soprano::Vocabulary::OWL::TransitiveProperty()
{
    return s_owl()->owl_TransitiveProperty;
}

QUrl Soprano::Vocabulary::OWL::allValuesFrom()
{
    return s_owl()->owl_allValuesFrom;
}

QUrl Soprano::Vocabulary::OWL::backwardCompatibleWith()
{
    return s_owl()->owl_backwardCompatibleWith;
}

QUrl Soprano::Vocabulary::OWL::cardinality()
{
    return s_owl()->owl_cardinality;
}

QUrl Soprano::Vocabulary::OWL::complementOf()
{
    return s_owl()->owl_complementOf;
}

QUrl Soprano::Vocabulary::OWL::differentFrom()
{
    return s_owl()->owl_differentFrom;
}

QUrl Soprano::Vocabulary::OWL::disjointWith()
{
    return s_owl()->owl_disjointWith;
}

QUrl Soprano::Vocabulary::OWL::distinctMembers()
{
    return s_owl()->owl_distinctMembers;
}

QUrl Soprano::Vocabulary::OWL::equivalentClass()
{
    return s_owl()->owl_equivalentClass;
}

QUrl Soprano::Vocabulary::OWL::equivalentProperty()
{
    return s_owl()->owl_equivalentProperty;
}

QUrl Soprano::Vocabulary::OWL::hasValue()
{
    return s_owl()->owl_hasValue;
}

QUrl Soprano::Vocabulary::OWL::imports()
{
    return s_owl()->owl_imports;
}

QUrl Soprano::Vocabulary::OWL::incompatibleWith()
{
    return s_owl()->owl_incompatibleWith;
}

QUrl Soprano::Vocabulary::OWL::intersectionOf()
{
    return s_owl()->owl_intersectionOf;
}

QUrl Soprano::Vocabulary::OWL::inverseOf()
{
    return s_owl()->owl_inverseOf;
}

QUrl Soprano::Vocabulary::OWL::maxCardinality()
{
    return s_owl()->owl_maxCardinality;
}

QUrl Soprano::Vocabulary::OWL::minCardinality()
{
    return s_owl()->owl_minCardinality;
}

QUrl Soprano::Vocabulary::OWL::onProperty()
{
    return s_owl()->owl_onProperty;
}

QUrl Soprano::Vocabulary::OWL::oneOf()
{
    return s_owl()->owl_oneOf;
}

QUrl Soprano::Vocabulary::OWL::priorVersion()
{
    return s_owl()->owl_priorVersion;
}

QUrl Soprano::Vocabulary::OWL::sameAs()
{
    return s_owl()->owl_sameAs;
}

QUrl Soprano::Vocabulary::OWL::someValuesFrom()
{
    return s_owl()->owl_someValuesFrom;
}

QUrl Soprano::Vocabulary::OWL::unionOf()
{
    return s_owl()->owl_unionOf;
}

QUrl Soprano::Vocabulary::OWL::versionInfo()
{
    return s_owl()->owl_versionInfo;
}
