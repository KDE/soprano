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

#include "cluceneutils.h"
#include "wstring.h"
#include "error.h"

namespace Soprano {
    namespace Index {
        class IndexStatics
        {
        public:
            IndexStatics()
                : idFieldName( L"id" ),
                  textFieldName( L"text" ),
                  bnodeIdPrefix( "!" ) {
            }

            WString idFieldName;
            WString textFieldName;
            QString bnodeIdPrefix;
        };
    }
}

Q_GLOBAL_STATIC( Soprano::Index::IndexStatics, cluceneIndexStatics );


WString Soprano::Index::idFieldName()
{
    return cluceneIndexStatics()->idFieldName;
}


WString Soprano::Index::textFieldName()
{
    return cluceneIndexStatics()->textFieldName;
}


QString Soprano::Index::bnodeIdPrefix()
{
    return cluceneIndexStatics()->bnodeIdPrefix;
}


Soprano::Error::Error Soprano::Index::exceptionToError( CLuceneError& err )
{
    return Error::Error( err.what(), Error::ERROR_UNKNOWN + err.number() );
}
