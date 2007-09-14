/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */

#include <QtCore/QCoreApplication>
#include <QtCore/QTextStream>
#include <QtCore/QStringList>

#include "servercore.h"
#include "soprano/backend.h"
#include "soprano/pluginmanager.h"
#include "soprano/version.h"

#include "dbus/dbusserveradaptor.h"

#define VERSION "1.0"

int usage()
{
    QTextStream s( stderr );
    s << "sopranod " << VERSION << " (using Soprano " << Soprano::versionString() << ")" << endl;
    s << "   Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>" << endl;
    s << "   This software is released under the GNU General Public License version 2." << endl;
    s << endl;
    s << "Usage:" << endl
      << "   sopranod [--port <port>] [--backend <name>] [--storagedir <dir>]" << endl;

    return 1;
}

int main( int argc, char** argv )
{
    QCoreApplication app( argc, argv );
    QStringList args = app.arguments();
    QString backendName;
    int port = Soprano::Server::ServerCore::DEFAULT_PORT;
    QList<Soprano::BackendSetting> settings;
    int i = 1;
    while ( i < args.count() ) {
        if ( args[i] == "--storagedir" ) {
            ++i;
            if ( i < args.count() ) {
                settings.append( Soprano::BackendSetting( Soprano::BACKEND_OPTION_STORAGE_DIR, args[i] ) );
            }
            else {
                return usage();
            }
        }
        else if ( args[i] == "--backend" ) {
            ++i;
            if ( i < args.count() ) {
                backendName = args[i];
            }
            else {
                return usage();
            }
        }
        else if ( args[i] == "--port" ) {
            ++i;
            if ( i < args.count() ) {
                bool ok;
                port = args[i].toInt( &ok );
                if ( !ok ) {
                    return usage();
                }
            }
            else {
                return usage();
            }
        }
        else {
            return usage();
        }

        ++i;
    }

    Soprano::Server::ServerCore core;

    QDBusConnection::sessionBus().registerService( "org.soprano.Server" );
    core.registerAsDBusObject();

    if ( !backendName.isEmpty() ) {
        const Soprano::Backend* backend = Soprano::PluginManager::instance()->discoverBackendByName( backendName );
        if ( !backend ) {
            QTextStream( stderr ) << "Could not find backend: " << backendName << endl;
            return 2;
        }
        core.setBackend( backend );
    }

    core.start( port );
    return app.exec();
}
