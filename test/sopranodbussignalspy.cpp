/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2006 Sebastian Trueg <strueg@mandriva.com>
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

#include "modelsignalspy.h"
#include "../server/dbus/dbusclient.h"
#include "../server/dbus/dbusmodel.h"

#include <QtCore/QCoreApplication>

int main( int argc, char *argv[] )
{
    QCoreApplication app( argc, argv );

    if ( argc < 2 ) {
        return 1;
    }

    QString modelName = app.arguments()[1];
    QString dbusService;
    if ( argc > 2 ) {
        dbusService = app.arguments()[2];
    }

    Soprano::Client::DBusClient client( dbusService );
    Soprano::Client::DBusModel* model = client.createModel( modelName );
    Soprano::ModelSignalSpy spy;
    spy.setModel( model );

    return app.exec();
}
