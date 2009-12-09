/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2008-2009 Sebastian Trueg <trueg@kde.org>
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

    VirtuosoController* controller = 0;
    if ( host.isEmpty() &&
         port == 0 &&
         uid.isEmpty() &&
         pwd.isEmpty() ) {
        if ( path.isEmpty() ) {
            setError( "Need a database storage path set to start a local Virtuoso instance." );
            return 0;
        }

        // start local server
        controller = new VirtuosoController();
        if ( !controller->start( settings, debugMode ? VirtuosoController::DebugMode : VirtuosoController::NoFlags ) ) {
            setError( controller->lastError() );
            delete controller;
            return 0;
        }

        host = "localhost";
        port = controller->usedPort();
        uid = "dba";
        pwd = "dba";
    }

    QString driverPath = findVirtuosoDriver();
    if ( driverPath.isEmpty() ) {
        setError( "Could not find Virtuoso ODBC driver" );
        return 0;
    }

#ifdef Q_OS_WIN
    QString connectString = QString( "driver={Virtuoso (Open Source)};host=%1:%2;uid=%3;pwd=%4" )
                            .arg( host, QString::number( port ), uid, pwd );
#else
    QString connectString = QString( "host=%1:%2;uid=%3;pwd=%4;driver=%5" )
                            .arg( host, QString::number( port ), uid, pwd, driverPath );
#endif
    ODBC::ConnectionPool* connectionPool = new ODBC::ConnectionPool( connectString );

    // FIXME: should configuration only be allowed on spawned servers?
    ODBC::Connection* conn = connectionPool->connection();
    DatabaseConfigurator configurator( conn );
    configurator.configureServer( settings );
    delete conn;

    VirtuosoModel* model = new VirtuosoModel( connectionPool, this );
    // mem mangement the ugly way
    // FIXME: improve
    if ( controller )
        controller->setParent( model );
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
    if ( findVirtuosoDriver().isEmpty() ) {
        qDebug() << Q_FUNC_INFO << "could not find Virtuoso ODBC driver";
        return false;
    }

    QString virtuosoBin = VirtuosoController::locateVirtuosoBinary();
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


QString Soprano::Virtuoso::BackendPlugin::findVirtuosoDriver() const
{
#ifdef Q_OS_WIN
    QStringList virtuosoDirs;
    const QString virtuosoHome = QDir::fromNativeSeparators( qgetenv("VIRTUOSO_HOME") );
    if ( !virtuosoHome.isEmpty() ) {
        virtuosoDirs << virtuosoHome + QLatin1String("/bin")
                     << virtuosoHome + QLatin1String("/lib");
    }
    return Soprano::findLibraryPath( "virtodbc", virtuosoDirs );
#else
    return Soprano::findLibraryPath( "virtodbc_r", QStringList(), QStringList() << QLatin1String( "virtuoso/plugins/" ) );
#endif
}

#include "virtuosobackend.moc"
