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

#ifndef _SOPRANO_SIGNALCACHE_MODEL_H_
#define _SOPRANO_SIGNALCACHE_MODEL_H_

#include "filtermodel.h"
#include "soprano_export.h"

class QTimerEvent;

namespace Soprano {
    namespace Util {
        /**
         * \class SignalCacheModel signalcachemodel.h Soprano/Util/SignalCacheModel
         *
         * \brief Caches multiple signals into one to avoid slowdown when adding
         * or removing many statements.
         *
         * When adding or removing many statements with separate calls to Model::addStatement
         * or Model::removeStatement Model emits signals Model::statementsAdded and Model::statementsRemoved
         * over and over for each call. This can slow down an application that monitors
         * the state of the model.
         *
         * The SignalCacheModel caches multiple emitted signals into one, trying to ensure
         * that only one signal is delivered in a certain time. The downside of this is
         * that signals are not delivered immiadetely but through the event loop. This,
         * however, should seldomly be a problem.
         *
         * \author Sebastian Trueg <trueg@kde.org>
         *
         * \since 2.1
         */
        class SOPRANO_EXPORT SignalCacheModel : public FilterModel
        {
            Q_OBJECT

        public:
            /**
             * Create a new SignalCacheModel.
             *
             * \param parent The parent Model to forward
             *        the actual calls to.
             */
            SignalCacheModel( Model* parent = 0 );

            /**
             * Destructor.
             */
            virtual ~SignalCacheModel();

            /**
             * The cache time.
             * \sa setCacheTime
             */
            int cacheTime() const;

        public Q_SLOTS:
            /**
             * Signals are only delivered once every \p msec milliseconds.
             *
             * Default value is 50
             */
            void setCacheTime( int msec );

        protected:
            /**
             * Reimplemented to do the actual signal caching.
             */
            virtual void parentStatementsAdded();

            /**
             * Reimplemented to do the actual signal caching.
             */
            virtual void parentStatementsRemoved();

            void timerEvent( QTimerEvent* event );

        private:
            class Private;
            Private* const d;
        };
    }
}

#endif
