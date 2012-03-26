/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2008-2012 Sebastian Trueg <trueg@kde.org>
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

#include "virtuosobackend.h"
#include "virtuosocontroller.h"
#include "virtuosomodel.h"
#include "virtuosoconfigurator.h"
#include "odbcconnection.h"
#include "odbcconnectionpool.h"

#include "sopranodirs.h"

#include <QtCore/QDebug>
#include <QtCore/QtPlugin>
#include <QtCore/QDir>


Q_EXPORT_PLUGIN2(soprano_virtuosobackend, Soprano::Virtuoso::BackendPlugin)

Soprano::Virtuoso::BackendPlugin::BackendPlugin()
    : Backend( "virtuosobackend" )
{
}


Soprano::Virtuoso::BackendPlugin::~BackendPlugin()
{
}


Soprano::StorageModel* Soprano::Virtuoso::BackendPlugin::createModel( const BackendSettings& settings ) const
{
    // for now we only support connecting to a running virtuoso instance
    QString host = valueInSettings( settings, BackendOptionHost ).toString();
    int port = valueInSettings( settings, BackendOptionPort ).toInt();
    QString uid = valueInSettings( settings, BackendOptionUsername ).toString();
    QString pwd = valueInSettings( settings, BackendOptionPassword ).toString();
    QString path = valueInSettings( settings, BackendOptionStorageDir ).toString();
    bool debugMode = valueInSettings( settings, BackendOptionUser, QLatin1String( "debugmode" ) ).toBool();
    int queryTimeout = valueInSettings( settings, QLatin1String( "QueryTimeout" ), 0 ).toInt();

    VirtuosoController* controller = 0;
    if ( host.isEmpty() &&
         port == 0 &&
         uid.isEmpty() &&
         pwd.isEmpty() ) {
        if ( path.isEmpty() ) {
            setError( "Need a database storage path set to start a local Virtuoso instance." );
            return 0;
        }

        const QString virtuosoExe = locateVirtuosoBinary();
        if ( virtuosoExe.isEmpty() ) {
            setError( "Unable to find the Virtuoso binary." );
            return 0;
        }

        // start local server
        controller = new VirtuosoController();
        if ( !controller->start( virtuosoExe, settings, debugMode ? VirtuosoController::DebugMode : VirtuosoController::NoFlags ) ) {
            setError( controller->lastError() );
            delete controller;
            return 0;
        }

        host = QLatin1String("localhost");
        port = controller->usedPort();
        uid = QLatin1String("dba");
        pwd = QLatin1String("dba");
    }

#ifdef Q_OS_WIN
    const QString odbcDriver = QLatin1String( "{Virtuoso (Open Source)}" );
#else
    const QString odbcDriver = findVirtuosoDriver();
    if ( odbcDriver.isEmpty() ) {
        setError( "Could not find Virtuoso ODBC driver" );
        return 0;
    }
#endif

    const QString connectString = QString::fromLatin1( "host=%1:%2;uid=%3;pwd=%4;driver=%5" )
                                  .arg( host, QString::number( port ), uid, pwd, odbcDriver );
    QStringList connectionSetupCommands;
    if ( queryTimeout > 1000 ) {
        connectionSetupCommands << QString::fromLatin1( "set result_timeout=%1" ).arg( queryTimeout );
    }

    ODBC::ConnectionPool* connectionPool = new ODBC::ConnectionPool( connectString, connectionSetupCommands );

    // FIXME: should configuration only be allowed on spawned servers?
    if ( ODBC::Connection* conn = connectionPool->connection() ) {
        DatabaseConfigurator configurator( conn );
        configurator.configureServer( settings );
        delete conn;
    }
    else {
        qDebug() << Q_FUNC_INFO << "Failed to connect to" << connectString;
        delete connectionPool;
        return 0;
    }

    VirtuosoModel* model = new VirtuosoModel( connectionPool, this );
    // mem mangement the ugly way
    // FIXME: improve
    if ( controller ) {
        controller->setParent( model );
        connect(controller, SIGNAL(stopped(Soprano::VirtuosoController::ExitStatus)), model, SLOT(slotVirtuosoStopped(Soprano::VirtuosoController::ExitStatus)));
    }
    return model;
}


bool Soprano::Virtuoso::BackendPlugin::deleteModelData( const BackendSettings& settings ) const
{
    // get the storage dir for this instance
    QString path = valueInSettings( settings, BackendOptionStorageDir ).toString();
    if ( path.isEmpty() ) {
        setError( "No storage path set. Cannot delete model data.", Error::ErrorInvalidArgument );
        return false;
    }

    // create a list of database files as configured in VirtuosoController::writeConfig
    QString prefix = QLatin1String( "soprano-virtuoso" );
    QStringList suffixes;
    suffixes << QLatin1String( ".db" )
             << QLatin1String( ".log" )
             << QLatin1String( ".trx" )
             << QLatin1String( ".pxa" )
             << QLatin1String( "-temp.db" )
             << QLatin1String( "-temp.trx" );

    // delete all database files
    QDir dir( path );
    foreach( const QString& suffix, suffixes ) {
        QString file = prefix + suffix;
        if ( dir.exists( file ) &&
             !dir.remove( file ) ) {
            setError( "Failed to remove file '" + dir.filePath( file ), Error::ErrorUnknown );
            return false;
        }
    }

    clearError();
    return true;
}


Soprano::BackendFeatures Soprano::Virtuoso::BackendPlugin::supportedFeatures() const
{
    return( BackendFeatureInference|
            BackendFeatureAddStatement|
            BackendFeatureRemoveStatements|
            BackendFeatureListStatements|
            BackendFeatureQuery|
            BackendFeatureContext );
}


namespace {
    QString parseVirtuosoVersion( const QByteArray& data ) {
        QString erroutput = QString::fromLocal8Bit( data );
        int vp = erroutput.indexOf( QLatin1String("Version" ) );
        if ( vp > 0 ) {
            vp += 8;
            return erroutput.mid( vp, erroutput.indexOf( ' ', vp ) - vp );
        }
        return QString();
    }

    QString determineVirtuosoVersion( const QString& virtuosoBin ) {
        QProcess p;
        p.start( virtuosoBin, QStringList() << QLatin1String( "--version" ), QIODevice::ReadOnly );
        p.waitForFinished();
        return parseVirtuosoVersion( p.readAllStandardError() );
    }
}

bool Soprano::Virtuoso::BackendPlugin::isAvailable() const
{
#ifndef Q_OS_WIN
    if ( findVirtuosoDriver().isEmpty() ) {
        qDebug() << Q_FUNC_INFO << "could not find Virtuoso ODBC driver";
        return false;
    }
#endif

    QString virtuosoBin = locateVirtuosoBinary();
    if ( virtuosoBin.isEmpty() ) {
        qDebug() << Q_FUNC_INFO << "could not find virtuoso-t binary";
        return false;
    }

    QString vs = determineVirtuosoVersion( virtuosoBin );
    if ( vs.isEmpty() ) {
        qDebug() << Q_FUNC_INFO << "Failed to determine version of the Virtuoso server at" << virtuosoBin;
        return false;
    }
    if ( vs < QLatin1String( "5.0.12" ) ) {
        qDebug() << Q_FUNC_INFO << "Minimum Virtuoso version is 5.0.12." << virtuosoBin << "has version" << vs;
        return false;
    }

    qDebug() << "Using Virtuoso Version:" << vs;
    return true;
}


// static
QString Soprano::Virtuoso::BackendPlugin::locateVirtuosoBinary()
{
    QStringList dirs = Soprano::exeDirs();
#ifdef Q_OS_WIN
    const QString virtuosoHome = QDir::fromNativeSeparators( qgetenv("VIRTUOSO_HOME") );
    if ( !virtuosoHome.isEmpty() )
        dirs << virtuosoHome + QLatin1String("/bin");
    else {
        dirs << QCoreApplication::applicationDirPath();
    }
#endif

    foreach( const QString& dir, dirs ) {
#ifdef Q_OS_WIN
        QFileInfo info( dir + QLatin1String("/virtuoso-t.exe") );
#else
        QFileInfo info( dir + QLatin1String("/virtuoso-t") );
#endif
        if ( info.isExecutable() ) {
            return info.absoluteFilePath();
        }
    }
    return QString();
}


#ifndef Q_OS_WIN
QString Soprano::Virtuoso::BackendPlugin::findVirtuosoDriver() const
{
    return Soprano::findLibraryPath( "virtodbc_r", QStringList(), QStringList() << QLatin1String( "virtuoso/plugins/" ) << QLatin1String( "odbc/" ) );
}
#endif

#include "virtuosobackend.moc"
