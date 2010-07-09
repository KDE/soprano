/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>
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

#ifndef _SOPRANO_PLUGIN_H_
#define _SOPRANO_PLUGIN_H_

#include <QtCore/QString>
#include "soprano_export.h"

namespace Soprano {

    class SopranoPluginFile;

    /**
     * \class Plugin plugin.h Soprano/Plugin
     *
     * \brief Base class for all plugins in %Soprano.
     *
     * Each %Soprano %Plugin has a name which can be read via
     * pluginName().
     *
     * \sa Soprano::PluginManager
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class SOPRANO_EXPORT Plugin
    {
    public:
        virtual ~Plugin();

        QString pluginName() const;

        /**
         * This method can be reimplemented by plugins that need
         * to do additional run-time checks before they can be used.
         *
         * The default implementation returns \p true.
         *
         * \return \p true if the plugin can be used.
         */
        virtual bool isAvailable() const;

    protected:
        Plugin( const QString& name );

    private:
        class Private;
        Private* const d;
    };
}

#endif
