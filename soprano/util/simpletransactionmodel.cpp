/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2008 Sebastian Trueg <trueg@kde.org>
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

#include "simpletransactionmodel.h"
#include "simpletransactionmodel_p.h"
#include "simpletransaction.h"


Soprano::Util::SimpleTransactionModel::SimpleTransactionModel( Model* parent )
    : FilterModel( parent ),
      d( new SimpleTransactionModelPrivate() )
{
    d->model = this;
}


Soprano::Util::SimpleTransactionModel::~SimpleTransactionModel()
{
    foreach( SimpleTransaction* st, d->openTransactions ) {
        st->invalidate();
    }
    delete d;
}


Soprano::Transaction* Soprano::Util::SimpleTransactionModel::startTransaction()
{
    SimpleTransaction* t = new SimpleTransaction( d );
    d->openTransactions.append( t );
    return t;
}

#include "simpletransactionmodel.moc"
