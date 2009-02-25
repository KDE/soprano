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
#include "sopranodirs.h"

#include <QtCore/QDebug>
#include <QtCore/QtPlugin>

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

        // FIXME: create some status sql table which stores a version and the already enabled features or already performed
        //        optimization tasks
        //        then perform missing initialization based on the values in that table
    }

    QString driverPath = findVirtuosoDriver();
    if ( driverPath.isEmpty() ) {
        setError( "Could not find Virtuoso ODBC driver" );
        return 0;
    }

    QString connectString = QString( "host=%1;port=%2;uid=%3;pwd=%4;driver=%5" )
                            .arg( host, QString::number( port ), uid, pwd, driverPath );

//     VirtuosoConfigurator configurator( connectString );
//     configurator.configureServer( settings );

    IODBCModel* model = new IODBCModel( this );
    if ( model->connect( connectString ) ) {
        // mem mangement the ugly way
        // FIXME: improve
        if ( controller )
            controller->setParent( model );
        clearError();
        return model;
    }
    else {
        setError( model->lastError() );
        delete controller;
        delete model;
        return 0;
    }
}


bool Soprano::Virtuoso::BackendPlugin::deleteModelData( const BackendSettings& settings ) const
{
    Q_UNUSED( settings );
    setError( "deleting model data not supported yet!" );
    return false;
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
    return !findVirtuosoDriver().isEmpty();
}


QString Soprano::Virtuoso::BackendPlugin::findVirtuosoDriver() const
{
    return Soprano::findLibraryPath( "virtodbc_r" );
}

#include "virtuosobackend.moc"
