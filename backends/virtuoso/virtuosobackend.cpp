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
#include "odbcenvironment.h"

#include "sopranodirs.h"

#include <QtCore/QDebug>
#include <QtCore/QtPlugin>
#include <QtCore/QDir>


Q_EXPORT_PLUGIN2(soprano_virtuosobackend, Soprano::Virtuoso::BackendPlugin)

Soprano::Virtuoso::BackendPlugin::BackendPlugin()
    : Backend( "virtuosobackend" ),
      m_odbcEnvironment( 0 )
{
}


Soprano::Virtuoso::BackendPlugin::~BackendPlugin()
{
}


Soprano::StorageModel* Soprano::Virtuoso::BackendPlugin::createModel( const BackendSettings& settings ) const
{
    if ( !m_odbcEnvironment ) {
        if ( !( m_odbcEnvironment = ODBC::Environment::createEnvironment() ) ) {
            setError( "Unable to create ODBC environment." );
            return 0;
        }
    }

    // for now we only support connecting to a running virtuoso instance
    QString host = valueInSettings( settings, BackendOptionHost ).toString();
    int port = valueInSettings( settings, BackendOptionPort ).toInt();
    QString uid = valueInSettings( settings, BackendOptionUsername ).toString();
    QString pwd = valueInSettings( settings, BackendOptionPassword ).toString();
    QString path = valueInSettings( settings, BackendOptionStorageDir ).toString();
    bool debugMode = valueInSettings( settings, BackendOptionUser, QLatin1String( "debugmode" ) ).toBool();

    VirtuosoController* controller = 0;
    if ( host.isEmpty() || port == 0 ) {
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

    QString connectString = QString( "host=%1;port=%2;uid=%3;pwd=%4;driver=%5" )
                            .arg( host, QString::number( port ), uid, pwd, driverPath );

    if ( ODBC::Connection* connection = odbcEnvironment()->createConnection( connectString ) ) {

        // FIXME: should configuration only be allowed on spawned servers?
        DatabaseConfigurator configurator( connection );
        configurator.configureServer( settings );

        VirtuosoModel* model = new VirtuosoModel( connection, this );
        // mem mangement the ugly way
        // FIXME: improve
        if ( controller )
            controller->setParent( model );
        return model;
    }
    else {
        setError( odbcEnvironment()->lastError() );
        return 0;
    }
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
        if ( !dir.remove( file ) ) {
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


bool Soprano::Virtuoso::BackendPlugin::isAvailable() const
{
    // FIXME: make sure the virtuoso bin's version is at least 5.0.10
    return !findVirtuosoDriver().isEmpty() && !VirtuosoController::locateVirtuosoBinary().isEmpty();
}


QString Soprano::Virtuoso::BackendPlugin::findVirtuosoDriver() const
{
    return Soprano::findLibraryPath( "virtodbc_r" );
}

#include "virtuosobackend.moc"
