/*
 * This file is part of Soprano Project.
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

#include "sil.h"

#define SIL_NS "http://soprano.org/sil#"

class SILGlobals
{
public:
    SILGlobals()
        : silNamespace( SIL_NS ),
          silInferenceMetaData( SIL_NS"InferenceMetaData" ),
          silInferenceGraph( SIL_NS"InferenceGraph" ),
          silSourceStatement( SIL_NS"sourceStatement" ),
          silContext( SIL_NS"context" ) {
    }

    QUrl silNamespace;
    QUrl silInferenceMetaData;
    QUrl silInferenceGraph;
    QUrl silSourceStatement;
    QUrl silContext;
};


Q_GLOBAL_STATIC( SILGlobals, silGlobals );

QUrl Soprano::Vocabulary::SIL::NAMESPACE()
{
    return silGlobals()->silNamespace;
}


QUrl Soprano::Vocabulary::SIL::INFERENCE_METADATA()
{
    return silGlobals()->silInferenceMetaData;
}


QUrl Soprano::Vocabulary::SIL::INFERENCE_GRAPH()
{
    return silGlobals()->silInferenceGraph;
}


QUrl Soprano::Vocabulary::SIL::SOURCE_STATEMENT()
{
    return silGlobals()->silSourceStatement;
}


QUrl Soprano::Vocabulary::SIL::CONTEXT()
{
    return silGlobals()->silContext;
}
