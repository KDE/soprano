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

#include "threestorebackend.h"
#include "threestorecore.h"
#include "threestoremodel.h"

#include <QtCore/QtPlugin>
#include <QtCore/QDebug>


Q_EXPORT_PLUGIN2(soprano_tstorebackend, Soprano::ThreeStore::BackendPlugin)

Soprano::ThreeStore::BackendPlugin::BackendPlugin()
  : QObject(),
    Backend( "3store" )
{
}


Soprano::Model* Soprano::ThreeStore::BackendPlugin::createModel() const
{
    // no memory model support. Hmm... kind of ugly, maybe the APi should be changes?
    return 0;
}


Soprano::Model* Soprano::ThreeStore::BackendPlugin::createModel( const QString& name, const QStringList& options ) const
{
    // we use the name as 3Store/SQL database name and parse the options for mysql connection data
    QString host,  user,  passwd;
    Q_FOREACH( QString option, options ) {
        QStringList tokens = option.split( '=' );
        if ( tokens.count() == 2 ) {
            if ( tokens[0] == "host" ) {
                host = tokens[1];
            }
            else if ( tokens[0] == "user" ) {
                user = tokens[1];
            }
            else if ( tokens[0] == "password" ) {
                passwd = tokens[1];
            }
        }
    }

    if ( host.isEmpty() ) {
        host = "localhost";
    }

    return ThreeStore::Core::instance()->createModel( host, name, user, passwd );
}


QStringList Soprano::ThreeStore::BackendPlugin::features() const
{
  return QString( "inference,contexts" ).split( ',' );
}

#include "threestorebackend.moc"
