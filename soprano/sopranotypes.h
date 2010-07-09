/* This file is part of Soprano
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
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

#ifndef _SOPRANO_TYPES_H_
#define _SOPRANO_TYPES_H_

#include "soprano_export.h"

#include <QtCore/QFlags>
#include <QtCore/QString>

namespace Soprano
{
    /**
     * Different types of RDF serialization.
     *
     * \sa serializationMimeType, mimeTypeToSerialization
     */
    // FIXME: what about the used charsets? Should we and if so, how should we include them?
    // FIXME: SerializationUnknown and SerializationUser should become the same, i.e. one should become deprecated
    enum RdfSerialization {
        SerializationUnknown = 0x0,   /**< The serialization is unknown. */
        SerializationRdfXml = 0x1,    /**< Standard RDF/XML serialization */
        SerializationN3 = 0x2,        /**< Notation 3: http://www.w3.org/DesignIssues/Notation3 */
        SerializationNTriples = 0x4,  /**< N-Triples as defined by W3: http://www.w3.org/TR/rdf-testcases/#ntriples */
        SerializationTurtle = 0x8,    /**< Turtle - Terse RDF Triple Language: http://www.dajobe.org/2004/01/turtle/ */
        SerializationTrig = 0x10,     /**< TriG - Turtle + Named Graphs: http://sites.wiwiss.fu-berlin.de/suhl/bizer/TriG/ */
        SerializationNQuads = 0x20,   /**< N-Quads extends over N-Triples in that it adds an optional context node. */
        SerializationUser = 0x0       /**< The user type can be used to introduce unknown RDF serializations by name */
    };
    Q_DECLARE_FLAGS(RdfSerializations, RdfSerialization)

    /**
     * Get the mimetype string of a serialization.
     *
     * \param serialization The serialization the mimetype is wanted for.
     * \param userSerialization If \p serialization is SerializationUser then this is the user defined serialization.
     *        (this parameter is added for convinience to avoid having an additional check before using this method.)
     *
     * \return The mimetype of serialization, an empty string is serialization is SerializationUnknown, or
     * \p userSerialization if \p serialization is SerializationUser.
     */
    SOPRANO_EXPORT QString serializationMimeType( RdfSerialization serialization, const QString& userSerialization = QString() );

    /**
     * Parse a mimetype and match it to the Soprano::RdfSerialization enum.
     * \return the Soprano::RdfSerialization type that matches mimetype or SerializationUnknown if the mimetype
     * could not be parsed. Be aware that Soprano is very lax in parsing the mimetype, i.e. you can use simple
     * strings like 'trig' or 'n-quads' instead of the proper mimetype for convenience.
     */
    SOPRANO_EXPORT RdfSerialization mimeTypeToSerialization( const QString& mimetype );

    /**
     * There are two types of backend settings: boolean flags and key/value pairs.
     * The boolean flags are identified by BackendFlag.
     *
     * \sa BackendSetting, Backend::createModel()
     */
    enum BackendOption {
        BackendOptionNone = 0x0,
        BackendOptionStorageMemory = 0x1,   /**< A boolean option, if set means that the Model should be a pure memory model which does not store any data on disk. Only backend supporting Soprano::BackendFeatureStorageMemory can handle this option. Other backends should return 0 if this option is specified in createModel() */
        BackendOptionEnableInference = 0x2, /**< A boolean option, if set means that the Model should have inference enabled. FIXME: but what kind of inference? RDFS only? rule-based? */
        BackendOptionStorageDir = 0x4,      /**< A key/value option that states the directory where the model should actually store the data. */
        BackendOptionHost = 0x8,            /**< A string option defining a host to connect to (IP address or domain name). \since 2.3 */
        BackendOptionPort = 0x10,           /**< An integer option defining the port on which to connect the host. \since 2.3 */
        BackendOptionUsername = 0x20,       /**< A string option optionally defining the username. \since 2.3 */
        BackendOptionPassword = 0x40,       /**< A string option optionally defining the password. \since 2.3 */
        BackendOptionUser = 0x1000          /**< Additional options can be supported through user options which are identified by a string key in BackendSetting::userSettingName. */
    };
    Q_DECLARE_FLAGS( BackendOptions, BackendOption )

    /**
     * Each Backend plugin can support different features. %Soprano defines a list of well-known features
     * that each backend implementation should try to realize. In addition user features can be defined.
     * For this BackendFeatureUser has to be included in the supported features. Then additional features
     * may be reported through Backend::supportedUserFeatures().
     *
     * \sa Backend::supportedFeatures(), Backend::supportedUserFeatures()
     */
    enum BackendFeature {
        BackendFeatureNone = 0x0,
        BackendFeatureAddStatement = 0x1,       /**< The backend supports the adding of statements (Model::addStatement()). */
        BackendFeatureRemoveStatements = 0x2,   /**< The backend supports the removal of statements (Model::removeStatement()). */
        BackendFeatureListStatements = 0x4,     /**< The backend supports the listing of statements (Model::listStatements(), Model::containsStatement()) */
        BackendFeatureQuery = 0x8,              /**< The backend supports RDF queries (Model::executeQuery()) */
        BackendFeatureInference = 0x10,         /**< The backend provides includes inference engine. */
        BackendFeatureInferenceOptional = 0x20, /**< The backend's inference engine is optional, i.e. it can be disabled. */
        BackendFeatureContext = 0x40,           /**< The backend supports contexts, i.e. named graphs. If this feature is not present Statement::context() will always return an empty node. */
        BackendFeatureStorageMemory = 0x80,     /**< The backend supports pure memory Models. (Soprano::BackendOptionStorageMemory) */
        BackendFeatureAll = BackendFeatureAddStatement|
        BackendFeatureRemoveStatements|
        BackendFeatureListStatements|
        BackendFeatureQuery|
        BackendFeatureInference|
        BackendFeatureInferenceOptional|
        BackendFeatureContext|
        BackendFeatureStorageMemory,            /**< All features. \since 2.3 */
        BackendFeatureUser = 0x1000             /**< Backends may support additional features that are not officially supported by %Soprano. */
    };
    Q_DECLARE_FLAGS( BackendFeatures, BackendFeature )


    namespace Query {
        /**
         * %Soprano defines a set of well-known query languages that can be used to serialize queries.
         * Additional languages may be supported using Query::QueryLanguageUser in combination with
         * string codes.
         *
         * \sa Model::executeQuery, Query::Parser, Query::Serializer
         */
        enum QueryLanguage {
            QueryLanguageNone = 0x0,    /**< No query language */
            QueryLanguageSparql = 0x1,  /**< The SPARQL query language: http://www.w3.org/TR/rdf-sparql-query/ */
            QueryLanguageRdql = 0x2,    /**< The RDQL RDF query language: http://www.w3.org/Submission/2004/SUBM-RDQL-20040109/ */
            QueryLanguageSerql = 0x4,   /**< Sesame RDF %Query Language: http://openrdf.org/doc/sesame2/users/ch05.html */
            QueryLanguageUser = 0x1000, /**< The user type can be used to introduce unknown query lanaguages by name */
            QUERY_LANGUAGE_NONE = QueryLanguageNone,   /**< \deprecated use Soprano::Query::QueryLanguageNone */
            QUERY_LANGUAGE_SPARQL = QueryLanguageSparql, /**< \deprecated use Soprano::Query::QueryLanguageSparql */
            QUERY_LANGUAGE_RDQL = QueryLanguageRdql,   /**< \deprecated use Soprano::Query::QueryLanguageRdql */
            QUERY_LANGUAGE_SERQL = QueryLanguageSerql,  /**< \deprecated use Soprano::Query::QueryLanguageSerql */
            QUERY_LANGUAGE_USER = QueryLanguageUser /**< \deprecated use Soprano::Query::QueryLanguageUser */
        };
        Q_DECLARE_FLAGS( QueryLanguages, QueryLanguage )

        /**
         * Convert a Soprano::Query::QueryLanguage value into a string representation.
         *
         * \param lang The query language to convert
         * \param userQueryLanguage If \p lang equals Query::QueryLanguageUser,
         * \p userQueryLanguage defines the language to use.
         *
         * \return A string representation of \p lang. If \p lang equals Soprano::Query::QueryLanguageUser, the method
         * simply returns \p userQueryLanguage.
         */
        SOPRANO_EXPORT QString queryLanguageToString( Soprano::Query::QueryLanguage lang, const QString& userQueryLanguage = QString() );

        /**
         * Convert a query language string into a Soprano::Query::QueryLanguage value.
         *
         * \param queryLanguage The query language string to convert.
         *
         * \return The QueryLanguage value corresponding to \p queryLanguage or Soprano::Query::QueryLanguageUser if it is unknown.
         */
        SOPRANO_EXPORT QueryLanguage queryLanguageFromString( const QString& queryLanguage );
    }
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Soprano::RdfSerializations)
Q_DECLARE_OPERATORS_FOR_FLAGS(Soprano::BackendOptions)
Q_DECLARE_OPERATORS_FOR_FLAGS(Soprano::BackendFeatures)
Q_DECLARE_OPERATORS_FOR_FLAGS(Soprano::Query::QueryLanguages)

#endif
