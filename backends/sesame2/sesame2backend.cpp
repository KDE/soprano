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

#include "sesame2backend.h"
#include "sesame2model.h"
#include "jniwrapper.h"
#include "sesame2repository.h"

#include <QtCore/QtPlugin>
#include <QtCore/QDebug>


Q_EXPORT_PLUGIN2(soprano_sesame2backend, Soprano::Sesame2::BackendPlugin)

Soprano::Sesame2::BackendPlugin::BackendPlugin()
  : QObject(),
    Backend( "sesame2" )
{
}


Soprano::Model* Soprano::Sesame2::BackendPlugin::createModel() const
{
    RepositoryWrapper* repo = RepositoryWrapper::create();
    if ( repo ) {
        if ( repo->initialize() ) {
            return new Model( repo );
        }
        else {
            delete repo;
        }
    }

    return 0;
}


Soprano::Model* Soprano::Sesame2::BackendPlugin::createModel( const QString& name, const QStringList& options ) const
{
    QString path;
    Q_FOREACH( QString option, options ) {
        QStringList tokens = option.split( '=' );
        if ( tokens.count() == 2 ) {
            QString key = tokens[0];
            QString value = tokens[1];

            if( key == "storagePath" ) {
                path = value;
            }
        }
    }

    if ( path.isEmpty() ) {
        qDebug() << "(Soprano::Sesame2::BackendPlugin) No path specified. Creating memory model.";
        return createModel();
    }

    RepositoryWrapper* repo = RepositoryWrapper::create( path );

    if ( repo ) {
        if ( repo->initialize() ) {
            return new Model( repo );
        }
        else {
            delete repo;
        }
    }

    return 0;
}


QStringList Soprano::Sesame2::BackendPlugin::features() const
{
    return QString( "inference,contexts,memory" ).split( ',' );
}

#include "sesame2backend.moc"
