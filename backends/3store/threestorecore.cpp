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

#include "threestorecore.h"
#include "threestoremodel.h"
#include "threestorebackend.h"

extern "C" {
#include <3store3/tstore.h>
}

#include <QtCore/QDebug>


Q_GLOBAL_STATIC( Soprano::ThreeStore::Core, coreInstance )


class Soprano::ThreeStore::Core::Private
{
public:

};


Soprano::ThreeStore::Core::Core()
    : QObject()
{
    ts_init();
}


Soprano::ThreeStore::Core::~Core()
{
    ts_finish();
}


Soprano::ThreeStore::Core* Soprano::ThreeStore::Core::instance()
{
    return coreInstance();
}


Soprano::ThreeStore::Model* Soprano::ThreeStore::Core::createModel( const Backend* backend,
                                                                    const QString& host,
                                                                    const QString& db,
                                                                    const QString& user,
                                                                    const QString& passwd )
{
    qDebug() << "(Soprano::ThreeStore::Core) Trying to connect 3Store DB '" << db << "' at host '" << host
             << "' as user '" << user << "' with password '" << passwd << "'";

    ts_connection* conn = ts_connect( host.toUtf8().data(),
                                      db.toUtf8().data(),
                                      user.toUtf8().data(),
                                      passwd.toUtf8().data() );

    if ( conn ) {
        return new ThreeStore::Model( backend, conn );
    }
    else {
        return 0;
    }
}

#include "threestorecore.moc"
