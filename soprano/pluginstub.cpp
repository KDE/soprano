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
#include "backend.h"
#include "parser.h"
#include "serializer.h"

#include <QtCore/QSharedData>
#include <QtCore/QPluginLoader>
#include <QtCore/QLibrary>
#include <QtCore/QDebug>


class Soprano::PluginStub::Private : public QSharedData
{
public:
    QString name;
    QString libPath;

    QObject* plugin;

    ~Private() {
        // FIXME: unload the lib
        // NOTE: This won't work as Qt might already have statically destroyed
        //       the plugin once this destructor is reached.
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


QString Soprano::PluginStub::name() const
{
    return d->name;
}


QObject* Soprano::PluginStub::plugin()
{
    if( !d->plugin ) {
        QPluginLoader loader( d->libPath );
        d->plugin = loader.instance();

        if (d->plugin == 0) {
            qDebug() << "Could not load plugin at " << loader.fileName() << ". " << loader.errorString();
            return d->plugin;
        }
        Plugin* plugin = 0;
        if ( Backend* backend = qobject_cast<Backend*>( d->plugin ) ) {
            plugin = backend;
        }
        else if ( Parser* parser = qobject_cast<Parser*>( d->plugin ) ) {
            plugin = parser;
        }
        else if ( Serializer* serializer = qobject_cast<Serializer*>( d->plugin ) ) {
            plugin = serializer;
        }

        if( plugin ) {
            if ( d->name.isEmpty() )
                d->name = plugin->pluginName();

            if ( !plugin->isAvailable() ) {
                qDebug() << "(Soprano::PluginManager) plugin " << plugin->pluginName() << "is not available.";
                return 0;
            }
        }
        else {
            qDebug() << "(Soprano::PluginManager) found no soprano plugin at " << loader.fileName();
            delete d->plugin;
            d->plugin = 0;
        }
    }
    return d->plugin;
}
