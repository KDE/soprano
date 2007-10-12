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

#ifndef _SOPRANO_MULTI_CALL_PROTECTION_ITERATOR_BASE_H_
#define _SOPRANO_MULTI_CALL_PROTECTION_ITERATOR_BASE_H_

#include "multicallprotectionmodel.h"

namespace Soprano {
    namespace Util {
	/**
	 * Does nothing. Only used as interface for 
	 * MultiCallProtectionModel::removeIterator
	 */
	class MultiCallProtectionIteratorBase
	{
	public:
	    MultiCallProtectionIteratorBase( MultiCallProtectionModel* model )
		: m_model( model ) {}
		virtual ~MultiCallProtectionIteratorBase() {
		    remove();
		}

		void close() {
		    // no need to really close, that is done by
		    // the real Model. We only need to make sure
		    // that we do not try to unlock a deleted model
		    m_model = 0;
		}

	protected:
		void remove() { 
		    if( m_model ) {
			m_model->removeIterator( this );
			m_model = 0;
		    }
		}

	private:
		MultiCallProtectionModel* m_model;
	};
    }
}

#endif
