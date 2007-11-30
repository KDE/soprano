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

#ifndef _SOPRANO_NAO_H_
#define _SOPRANO_NAO_H_

#include <QtCore/QUrl>
#include "soprano_export.h"

namespace Soprano {
    namespace Vocabulary {
    /**
     * The Nepomuk Annotation Ontology
     */
        namespace NAO {
            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#
             */
            SOPRANO_EXPORT QUrl naoNamespace();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasDefaultNamespaceAbbreviation
             */
            SOPRANO_EXPORT QUrl hasDefaultNamespaceAbbreviation();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#Symbol
             */
            SOPRANO_EXPORT QUrl Symbol();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#isTopicOf
             */
            SOPRANO_EXPORT QUrl isTopicOf();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#isTagFor
             */
            SOPRANO_EXPORT QUrl isTagFor();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#version
             */
            SOPRANO_EXPORT QUrl version();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#altLabel
             */
            SOPRANO_EXPORT QUrl altLabel();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasSymbol
             */
            SOPRANO_EXPORT QUrl hasSymbol();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#prefSymbol
             */
            SOPRANO_EXPORT QUrl prefSymbol();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#altSymbol
             */
            SOPRANO_EXPORT QUrl altSymbol();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasTopic
             */
            SOPRANO_EXPORT QUrl hasTopic();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#serializationLanguage
             */
            SOPRANO_EXPORT QUrl serializationLanguage();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#creator
             */
            SOPRANO_EXPORT QUrl creator();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#annotation
             */
            SOPRANO_EXPORT QUrl annotation();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#rating
             */
            SOPRANO_EXPORT QUrl rating();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#numericRating
             */
            SOPRANO_EXPORT QUrl numericRating();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#Tag
             */
            SOPRANO_EXPORT QUrl Tag();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#contributor
             */
            SOPRANO_EXPORT QUrl contributor();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasDefaultNamespace
             */
            SOPRANO_EXPORT QUrl hasDefaultNamespace();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#modified
             */
            SOPRANO_EXPORT QUrl modified();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#created
             */
            SOPRANO_EXPORT QUrl created();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#lastModified
             */
            SOPRANO_EXPORT QUrl lastModified();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#identifier
             */
            SOPRANO_EXPORT QUrl identifier();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#status
             */
            SOPRANO_EXPORT QUrl status();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#description
             */
            SOPRANO_EXPORT QUrl description();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#prefLabel
             */
            SOPRANO_EXPORT QUrl prefLabel();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#engineeringTool
             */
            SOPRANO_EXPORT QUrl engineeringTool();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#hasTag
             */
            SOPRANO_EXPORT QUrl hasTag();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#isRelated
             */
            SOPRANO_EXPORT QUrl isRelated();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#personalIdentifier
             */
            SOPRANO_EXPORT QUrl personalIdentifier();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nao#Party
             */
            SOPRANO_EXPORT QUrl Party();
        }
    }
}

#endif
