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

#define NAO_NS "http://semanticdesktop.org/ontologies/2007/08/15/nao#"

class Nao
{
public:
    Nao()
        : naoNamespace( NAO_NS ),
          created( NAO_NS"created" ) {
    }

    QUrl naoNamespace;
    QUrl created;
};


Q_GLOBAL_STATIC( Nao, nao )


QUrl Soprano::Vocabulary::NAO::naoNamespace()
{
    return nao()->naoNamespace;
}


QUrl Soprano::Vocabulary::NAO::created()
{
    return nao()->created;
}
