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

#include "storagemodel.h"

#include "statement.h"
#include "statementiterator.h"

#include <QtCore/QList>


class Soprano::StorageModel::Private
{
public:
    const Backend* backend;
};

Soprano::StorageModel::StorageModel( const Backend* backend )
    : d( new Private() )
{
    d->backend = backend;
}

Soprano::StorageModel::~StorageModel()
{
    delete d;
}

const Soprano::Backend* Soprano::StorageModel::backend() const
{
    return d->backend;
}

bool Soprano::StorageModel::isEmpty() const
{
    return statementCount() == 0;
}

bool Soprano::StorageModel::containsStatements( const Statement &statement ) const
{
    return listStatements( statement ).next();
}

#include "storagemodel.moc"
