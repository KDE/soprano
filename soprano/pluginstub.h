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

#ifndef _SOPRANO_PLUGIN_INFO_H_
#define _SOPRANO_PLUGIN_INFO_H_

#include <QtCore/QSharedDataPointer>

class QObject;

namespace Soprano {

    class Plugin;

    /**
     * A stub which will act as a small replacement 
     * for the plugin until it is used.
     */
    class PluginStub
    {
    public:
        /**
         * Create a new invalid instance.
         */
        PluginStub();

        PluginStub( const PluginStub& other );

        /**
         * Create a new plugin stub
         */
        PluginStub( const QString& name, const QString& libPath );

        /**
         * Destructor. Will unload the plugin if loaded.
         */
        ~PluginStub();

        PluginStub& operator=( const PluginStub& other );

        /**
         * Get the plugin. Will try to load it if not
         * already loaded.
         */
        QObject* plugin();

        QString name() const;

    private:
        class Private;
        QExplicitlySharedDataPointer<Private> d;
    };
}

#endif
