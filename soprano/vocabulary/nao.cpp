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

#include "nao.h"

class NaoPrivate
{
public:
    NaoPrivate()
        : nao_namespace( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#" ),
          nao_Party( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#Party" ),
          nao_Symbol( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#Symbol" ),
          nao_Tag( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#Tag" ),
          nao_altLabel( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#altLabel" ),
          nao_altSymbol( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#altSymbol" ),
          nao_annotation( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#annotation" ),
          nao_contributor( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#contributor" ),
          nao_created( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#created" ),
          nao_creator( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#creator" ),
          nao_description( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#description" ),
          nao_engineeringTool( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#engineeringTool" ),
          nao_hasDefaultNamespace( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasDefaultNamespace" ),
          nao_hasDefaultNamespaceAbbreviation( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasDefaultNamespaceAbbreviation" ),
          nao_hasSymbol( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasSymbol" ),
          nao_hasTag( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasTag" ),
          nao_hasTopic( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasTopic" ),
          nao_identifier( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#identifier" ),
          nao_isRelated( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#isRelated" ),
          nao_isTagFor( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#isTagFor" ),
          nao_isTopicOf( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#isTopicOf" ),
          nao_lastModified( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#lastModified" ),
          nao_modified( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#modified" ),
          nao_numericRating( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#numericRating" ),
          nao_personalIdentifier( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#personalIdentifier" ),
          nao_prefLabel( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#prefLabel" ),
          nao_prefSymbol( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#prefSymbol" ),
          nao_rating( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#rating" ),
          nao_serializationLanguage( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#serializationLanguage" ),
          nao_status( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#status" ),
          nao_version( "http://www.semanticdesktop.org/ontologies/2007/08/15/nao#version" ) {
    }

    QUrl nao_namespace;
    QUrl nao_Party;
    QUrl nao_Symbol;
    QUrl nao_Tag;
    QUrl nao_altLabel;
    QUrl nao_altSymbol;
    QUrl nao_annotation;
    QUrl nao_contributor;
    QUrl nao_created;
    QUrl nao_creator;
    QUrl nao_description;
    QUrl nao_engineeringTool;
    QUrl nao_hasDefaultNamespace;
    QUrl nao_hasDefaultNamespaceAbbreviation;
    QUrl nao_hasSymbol;
    QUrl nao_hasTag;
    QUrl nao_hasTopic;
    QUrl nao_identifier;
    QUrl nao_isRelated;
    QUrl nao_isTagFor;
    QUrl nao_isTopicOf;
    QUrl nao_lastModified;
    QUrl nao_modified;
    QUrl nao_numericRating;
    QUrl nao_personalIdentifier;
    QUrl nao_prefLabel;
    QUrl nao_prefSymbol;
    QUrl nao_rating;
    QUrl nao_serializationLanguage;
    QUrl nao_status;
    QUrl nao_version;
};

Q_GLOBAL_STATIC( NaoPrivate, s_nao )

QUrl Soprano::Vocabulary::NAO::naoNamespace()
{
    return s_nao()->nao_namespace;
}

QUrl Soprano::Vocabulary::NAO::Party()
{
    return s_nao()->nao_Party;
}

QUrl Soprano::Vocabulary::NAO::Symbol()
{
    return s_nao()->nao_Symbol;
}

QUrl Soprano::Vocabulary::NAO::Tag()
{
    return s_nao()->nao_Tag;
}

QUrl Soprano::Vocabulary::NAO::altLabel()
{
    return s_nao()->nao_altLabel;
}

QUrl Soprano::Vocabulary::NAO::altSymbol()
{
    return s_nao()->nao_altSymbol;
}

QUrl Soprano::Vocabulary::NAO::annotation()
{
    return s_nao()->nao_annotation;
}

QUrl Soprano::Vocabulary::NAO::contributor()
{
    return s_nao()->nao_contributor;
}

QUrl Soprano::Vocabulary::NAO::created()
{
    return s_nao()->nao_created;
}

QUrl Soprano::Vocabulary::NAO::creator()
{
    return s_nao()->nao_creator;
}

QUrl Soprano::Vocabulary::NAO::description()
{
    return s_nao()->nao_description;
}

QUrl Soprano::Vocabulary::NAO::engineeringTool()
{
    return s_nao()->nao_engineeringTool;
}

QUrl Soprano::Vocabulary::NAO::hasDefaultNamespace()
{
    return s_nao()->nao_hasDefaultNamespace;
}

QUrl Soprano::Vocabulary::NAO::hasDefaultNamespaceAbbreviation()
{
    return s_nao()->nao_hasDefaultNamespaceAbbreviation;
}

QUrl Soprano::Vocabulary::NAO::hasSymbol()
{
    return s_nao()->nao_hasSymbol;
}

QUrl Soprano::Vocabulary::NAO::hasTag()
{
    return s_nao()->nao_hasTag;
}

QUrl Soprano::Vocabulary::NAO::hasTopic()
{
    return s_nao()->nao_hasTopic;
}

QUrl Soprano::Vocabulary::NAO::identifier()
{
    return s_nao()->nao_identifier;
}

QUrl Soprano::Vocabulary::NAO::isRelated()
{
    return s_nao()->nao_isRelated;
}

QUrl Soprano::Vocabulary::NAO::isTagFor()
{
    return s_nao()->nao_isTagFor;
}

QUrl Soprano::Vocabulary::NAO::isTopicOf()
{
    return s_nao()->nao_isTopicOf;
}

QUrl Soprano::Vocabulary::NAO::lastModified()
{
    return s_nao()->nao_lastModified;
}

QUrl Soprano::Vocabulary::NAO::modified()
{
    return s_nao()->nao_modified;
}

QUrl Soprano::Vocabulary::NAO::numericRating()
{
    return s_nao()->nao_numericRating;
}

QUrl Soprano::Vocabulary::NAO::personalIdentifier()
{
    return s_nao()->nao_personalIdentifier;
}

QUrl Soprano::Vocabulary::NAO::prefLabel()
{
    return s_nao()->nao_prefLabel;
}

QUrl Soprano::Vocabulary::NAO::prefSymbol()
{
    return s_nao()->nao_prefSymbol;
}

QUrl Soprano::Vocabulary::NAO::rating()
{
    return s_nao()->nao_rating;
}

QUrl Soprano::Vocabulary::NAO::serializationLanguage()
{
    return s_nao()->nao_serializationLanguage;
}

QUrl Soprano::Vocabulary::NAO::status()
{
    return s_nao()->nao_status;
}

QUrl Soprano::Vocabulary::NAO::version()
{
    return s_nao()->nao_version;
}
