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

#ifndef _SOPRANO_XMLS_H_
#define _SOPRANO_XMLS_H_

#include <QtCore/QUrl>
#include "soprano_export.h"

namespace Soprano {
    namespace Vocabulary {
        namespace XMLSchema {
            /**
             * http://www.w3.org/2001/XMLSchema#
             */
            SOPRANO_EXPORT QUrl xsdNamespace();
            SOPRANO_EXPORT QUrl xsdInt();
            SOPRANO_EXPORT QUrl integer();
            SOPRANO_EXPORT QUrl negativeInteger();
            SOPRANO_EXPORT QUrl nonNegativeInteger();
            SOPRANO_EXPORT QUrl decimal();
            SOPRANO_EXPORT QUrl xsdShort();
            SOPRANO_EXPORT QUrl xsdLong();
            SOPRANO_EXPORT QUrl unsignedInt();
            SOPRANO_EXPORT QUrl unsignedShort();
            SOPRANO_EXPORT QUrl unsignedLong();
            SOPRANO_EXPORT QUrl string();
            SOPRANO_EXPORT QUrl xsdFloat();
            SOPRANO_EXPORT QUrl xsdDouble();
            SOPRANO_EXPORT QUrl boolean();
            SOPRANO_EXPORT QUrl date();
            SOPRANO_EXPORT QUrl dateTime();
            SOPRANO_EXPORT QUrl time();
            SOPRANO_EXPORT QUrl base64Binary();
        }
    }
}

#endif
