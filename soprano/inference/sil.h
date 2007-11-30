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

#ifndef _SOPRANO_INFERENCE_LANGUAGE_H_
#define _SOPRANO_INFERENCE_LANGUAGE_H_

#include <QtCore/QUrl>
#include "soprano_export.h"

namespace Soprano {
    namespace Vocabulary {
    /**
     * SIL, The Soprano Inference Language defines a set or URIs that
     * are used by the Soprano inference engine.
     */
    namespace SIL {
        /**
         * The SIL namespace (http://soprano.org/sil#)
         */
        SOPRANO_EXPORT QUrl silNamespace();

        /**
         * URI of the named graph that contains all metadata about the inference graphs
         * such as their sourceStatements and their type (which is always INFERENCE_GRAPH).
         *
         * (http://soprano.org/sil#InferenceMetaData)
         */
        SOPRANO_EXPORT QUrl InferenceMetaData();

        /**
         * inferenceGraph is a rdfs:Class that represents named graphs containing
         * infered triples.
         *
         * (http://soprano.org/sil#InferenceGraph)
         */
        SOPRANO_EXPORT QUrl InferenceGraph();

        /**
         * Property that states which statement was used to infer the triples in one
         * inferenceGraph.
         *
         * (http://soprano.org/sil#sourceStatement)
         */
        SOPRANO_EXPORT QUrl sourceStatement();

        /**
         * Property as addition to rdf:subject, rdf:predicate, and rdf:object
         * to state the context of a statement.
         */
        SOPRANO_EXPORT QUrl context();
    }
    }
}

#endif
