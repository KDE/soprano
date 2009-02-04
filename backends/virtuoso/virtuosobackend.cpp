/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2008 Sebastian Trueg <trueg@kde.org>
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
#include "iodbcmodel.h"
#include "sopranotools.h"

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

    if ( host.isEmpty() || port == 0 ) {
        setError( "Need host and port to connect to Virtuoso server.", Error::ErrorInvalidArgument );
        return 0;
    }

    if ( uid.isEmpty() )
        uid = "dba";
    if ( pwd.isEmpty() )
        pwd = "dba";

    QString driverPath = findVirtuosoDriver();
    if ( driverPath.isEmpty() ) {
        setError( "Could not find Virtuoso ODBC driver" );
        return 0;
    }

    QString connectString = QString( "host=%1;port=%2;uid=%3;pwd=%4;driver=%5" )
                            .arg( host, QString::number( port ), uid, pwd, driverPath );

    IODBCModel* model = new IODBCModel( this );
    if ( model->connect( connectString ) ) {
        clearError();
        return model;
    }
    else {
        setError( model->lastError() );
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
