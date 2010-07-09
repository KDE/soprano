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

#ifndef _SESAME2_SOPRANO_WRAPPER_H_
#define _SESAME2_SOPRANO_WRAPPER_H__

#include "jniobjectwrapper.h"
#include "jobjectref.h"

namespace Soprano {
    namespace Sesame2 {
        /**
         * Wrapper class around our own SopranoSesame2Wrapper
         * which has only one purpose:
         * Passing "(Resource)null" as context parameter to
         * RepositoryConnection.remove() since I have no idea
         * how to cast in JNI (if even possible).
         */
        class SopranoWrapper : public JNIObjectWrapper
        {
        public:
            SopranoWrapper( const JObjectRef& );
            ~SopranoWrapper();

            void removeFromDefaultContext( const JObjectRef& subject, const JObjectRef& predicate, const JObjectRef& object );

        private:
            class Private;
            Private* const d;
        };
    }
}

#endif
