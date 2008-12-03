/* This file is part of Soprano
 *
 * Copyright (C) 2007-2008 Sebastian Trueg <trueg@kde.org>
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

#ifndef SOPRANO_SERIALIZER_H
#define SOPRANO_SERIALIZER_H

#include "plugin.h"
#include "soprano_export.h"
#include "sopranotypes.h"
#include "sopranomacros.h"
#include "error.h"

#include <QtCore/QObject>

class QTextStream;


namespace Soprano
{
    class StatementIterator;

    /**
     * \deprecated
     *
     * \class Serializer serializer.h Soprano/Serializer
     *
     * \brief Replaced by Serializer2
     *
     * In %Soprano 2.2 Serializer has been deprecated in favor of Serializer2. Serializer is kept around
     * for binary compatibility.
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class SOPRANO_DEPRECATED SOPRANO_EXPORT Serializer : public Plugin, public Error::ErrorCache
    {
    public:
        virtual ~Serializer();
        virtual RdfSerializations supportedSerializations() const = 0;
        virtual QStringList supportedUserSerializations() const;
        bool supportsSerialization( RdfSerialization s, const QString& userSerialization = QString() ) const;
        virtual bool serialize( StatementIterator it, QTextStream& stream, RdfSerialization serialization, const QString& userSerialization = QString() ) const = 0;

    protected:
        Serializer( const QString& name );

    private:
        class Private;
        Private* const d;
    };
}

Q_DECLARE_INTERFACE(Soprano::Serializer, "org.soprano.plugins.Serializer/1.0")

#endif

