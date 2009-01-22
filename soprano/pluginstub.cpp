/*
 * This file is part of Soprano Project.
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

#include "pluginstub.h"
#include "plugin.h"

#include <QtCore/QSharedData>
#include <QtCore/QPluginLoader>
#include <QtCore/QLibrary>
#include <QtCore/QDebug>


class Soprano::PluginStub::Private : public QSharedData
{
public:
    QString name;
    QString libPath;

    Plugin* plugin;

    ~Private() {
        delete plugin;
        // FIXME: unload the lib
    }
};


Soprano::PluginStub::PluginStub()
    : d( new Private() )
{
    d->plugin = 0;
}


Soprano::PluginStub::PluginStub( const PluginStub& other )
    : d( other.d )
{
}


Soprano::PluginStub::PluginStub( const QString& name, const QString& libPath )
    : d( new Private() )
{
    d->name = name;
    d->libPath = libPath;
    d->plugin = 0;
}


Soprano::PluginStub::~PluginStub()
{
}


Soprano::PluginStub& Soprano::PluginStub::operator=( const PluginStub& other )
{
    d = other.d;
    return *this;
}


Soprano::Plugin* Soprano::PluginStub::plugin()
{
    if( !d->plugin ) {
        QPluginLoader loader( d->libPath );
        QObject* plugin = loader.instance();
        d->plugin = dynamic_cast<Plugin*>( plugin );
        if( d->plugin ) {
            qDebug() << "(Soprano::PluginManager) loaded plugin from" << loader.fileName();
            if ( !d->plugin->isAvailable() ) {
                qDebug() << "(Soprano::PluginManager) plugin " << d->plugin->pluginName() << "is not available.";
                return 0;
            }
        }
        else {
            qDebug() << "(Soprano::PluginManager) found no soprano plugin at " << loader.fileName();
            delete plugin;
        }
    }
    return d->plugin;
}
