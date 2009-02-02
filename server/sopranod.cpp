/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2009 Sebastian Trueg <trueg@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */

#include "servercore.h"
#include "soprano-server-config.h"
#include "backend.h"
#include "pluginmanager.h"
#include "version.h"
#ifdef BUILD_DBUS_SUPPORT
#include "dbus/dbusserveradaptor.h"
#endif
#include "lockfile.h"
#include "sopranodcore.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QTextStream>
#include <QtCore/QStringList>
#include <QtCore/QFile>
#include <QtCore/QDir>

#include <signal.h>


namespace {
#ifndef Q_OS_WIN
    void signalHandler( int signal )
    {
        switch( signal ) {
        case SIGHUP:
        case SIGQUIT:
        case SIGINT:
            QCoreApplication::exit( 0 );
        }
    }
#endif
}

int usage()
{
    QTextStream s( stderr );
    s << "sopranod " << Soprano::versionString() << endl;
    s << "   Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>" << endl;
    s << "   This program is free software; you can redistribute it and/or modify" << endl
      << "   it under the terms of the GNU General Public License as published by" << endl
      << "   the Free Software Foundation; either version 2 of the License, or" << endl
      << "   (at your option) any later version." << endl;
    s << endl;
    s << "Usage:" << endl
      << "   sopranod [--backend <name>] [--storagedir <dir>] [--port <port>]"
#ifdef BUILD_CLUCENE_INDEX
           " [--with-index]"
#endif
      << endl;

    return 1;
}

int main( int argc, char** argv )
{
    QCoreApplication app( argc, argv );

#ifndef Q_OS_WIN
    struct sigaction sa;
    ::memset( &sa, 0, sizeof( sa ) );
    sa.sa_handler = signalHandler;
    sigaction( SIGHUP, &sa, 0 );
    sigaction( SIGINT, &sa, 0 );
    sigaction( SIGQUIT, &sa, 0 );
#endif

    QStringList args = app.arguments();
    QString backendName;
    int port = Soprano::Server::ServerCore::DEFAULT_PORT;
    bool withIndex = false;
    QList<Soprano::BackendSetting> settings;
    int i = 1;
    while ( i < args.count() ) {
        if ( args[i] == "--storagedir" ) {
            ++i;
            if ( i < args.count() ) {
                settings.append( Soprano::BackendSetting( Soprano::BackendOptionStorageDir, args[i] ) );
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
                bool ok = true;
                port = args[i].toInt( &ok );
                if ( !ok ) {
                    return usage();
                }
            }
            else {
                return usage();
            }
        }
#ifdef BUILD_CLUCENE_INDEX
        else if ( args[i] == "--with-index" ) {
            withIndex = true;
        }
#endif
        else {
            return usage();
        }

        ++i;
    }

    // make sure we have a soprano dir
    if ( !QFile::exists( QDir::homePath() + QLatin1String( "/.soprano" ) ) ) {
        QDir::home().mkdir( ".soprano" );
    }

    // let's start by aquiring a lock
    LockFile lock( QDir::homePath() + "/.soprano/lock" );
    if ( !lock.aquireLock() ) {
        QTextStream s( stderr );
        s << "Unable to create lock. Sopranod is probably already running." << endl;
        return 5;
    }

    SopranodCore* core = new SopranodCore( withIndex, &app );
    core->setBackendSettings( settings );

#ifdef BUILD_DBUS_SUPPORT
    QDBusConnection::sessionBus().registerService( "org.soprano.Server" );
    core->registerAsDBusObject();
#endif

    if ( !backendName.isEmpty() ) {
        const Soprano::Backend* backend = Soprano::PluginManager::instance()->discoverBackendByName( backendName );
        if ( !backend ) {
            qWarning("Could not find backend: %s", qPrintable(backendName));
            return 2;
        }
        core->setBackend( backend );
    }

    QString socketPath = QDir::homePath() + QLatin1String( "/.soprano/socket" );
    if ( QFile::exists( socketPath ) ) {
        QFile::remove( socketPath );
    }

    if ( core->start() && core->listen( port ) ) {
        return app.exec();
    }
    else {
        return 1;
    }
}
