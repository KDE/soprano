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

#ifndef _SOPRANO_CLUCENE_UTILS_H_
#define _SOPRANO_CLUCENE_UTILS_H_

#include <QtCore/QString>
#include <CLucene.h>

#include "wstring.h"

namespace Soprano {
    namespace Error {
	class Error;
    }

    namespace Index {
        /**
	 * The name of the Document field holding the Resource identifier. The value
	 * stored in this field is either a URI or a BNode ID.
	 */
	WString idFieldName();

        /**
         * The name of the Document field that holds the concatenated text of a
         * Resource.
         */
	WString textFieldName();

        /**
         * String used to prefix BNode IDs with so that we can distinguish BNode
         * fields from URI fields in Documents. The prefix is chosen so that it is
         * invalid as a (part of a) URI scheme.
         */
        QString bnodeIdPrefix();

	Error::Error exceptionToError( CLuceneError& );
    }
}

#endif
