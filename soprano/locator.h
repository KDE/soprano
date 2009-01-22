/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007 Daniele Galdi <daniele.galdi@gmail.com>
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

#ifndef SOPRANO_LOCATOR_H
#define SOPRANO_LOCATOR_H

#include "soprano_export.h"

#include <QString>
#include <QtCore/QSharedData>

namespace Soprano 
{
    namespace Error {
        /**
         * \class Locator locator.h Soprano/Error/Locator
         *
         * \brief Details of a parser Error
         *
         * A Locator object provides additional information about
         * parsing errors.
         */
        class SOPRANO_EXPORT Locator {
        public:
            //@{
            Locator();
            Locator( int line, int column, int byte = -1, const QString& filename = QString() );
            Locator( const Locator &other );
            ~Locator();

            Locator& operator=( const Locator &other );
            //@}

            //@{
            int line() const;
            int column() const;
            int byte() const;
            QString fileName() const;
            //@}

            //@{
            void setLine( int line );
            void setColumn( int column );
            void setByte( int byte );
            void setFileName( const QString& fileName );
            //@}

        private:
            class Private;
            QSharedDataPointer<Private> d;     
        };
    }
}

#endif // SOPRANO_LOCATOR_H

