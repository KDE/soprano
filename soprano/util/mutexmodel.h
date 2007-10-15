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

#ifndef _SOPRANO_MUTEX_MODEL_H_
#define _SOPRANO_MUTEX_MODEL_H_

#include "filtermodel.h"
#include "soprano_export.h"

namespace Soprano {
    namespace Util {
	class MutexIteratorBase;

	/**
	 * \class MutexModel mutexmodel.h Soprano/Util/MutexModel
	 *
	 * \brief Protect a Model for multiple calls in multi- or single-thread
	 * applications.
	 *
	 * In most cases one does not need to use this FilterModel since Models
	 * created by Soprano::createModel and Soprano::Backend::createModel
	 * are thread-safe.
	 *
	 * \author Sebastian Trueg <trueg@kde.org>
	 */
	class SOPRANO_EXPORT MutexModel : public FilterModel
	{
	    Q_OBJECT

	public:
	    enum ProtectionMode {
		/**
		 * In PlainMultiThreading mode only one action can be performed
		 * at the same time. Calls block until the previous call is done.
		 *
		 * \warning In this mode nesting iterators will result in a deadlock.
		 */
		PlainMultiThreading,

		/**
		 * In ReadWriteMultiThreading mode multiple read operations
		 * can be performed at the same time but only one write 
		 * operation.
		 */
		ReadWriteMultiThreading,
	    
		/**
		 * ReadWriteSingleThreading mode is targetted at single thread
		 * applications. It makes sure that no read and write operations
		 * are performed interweaved. This mainly means that an open 
		 * iterator will block any write operation. Blocked operations
		 * wait in a local event loop. Thus, the application is not entirely
		 * blocked.
		 *
		 * This mode is mainly useful for server applications that need to
		 * handle multiple clients at the same time in the same thread.
		 * Actually the Soprano::Server implementation does make use of
		 * MutexModel with this mode.
		 *
		 * \warning In ReadWriteSingleThreading mode MutexModel
		 * is not thread-safe.
		 */
		ReadWriteSingleThreading
	    };

	    /**
	     * Create a new MutexModel.
	     *
	     * \param mode The mode to use.
	     * \param parent The parent Model to forward
	     *        the actual calls to.
	     */
	    MutexModel( ProtectionMode mode, Model* parent = 0 );

	    /**
	     * Destructor.
	     */
	    ~MutexModel();

	    Error::ErrorCode addStatement( const Statement &statement );
	    Error::ErrorCode removeStatement( const Statement &statement );
	    Error::ErrorCode removeAllStatements( const Statement &statement );
	    StatementIterator listStatements( const Statement &partial ) const;
	    NodeIterator listContexts() const;
	    QueryResultIterator executeQuery( const QString& query, 
					      Query::QueryLanguage language,
					      const QString& userQueryLanguage = QString() ) const;
	    bool containsStatement( const Statement &statement ) const;
	    bool containsAnyStatement( const Statement &statement ) const;
	    bool isEmpty() const;
	    int statementCount() const;

	private:
	    void removeIterator( MutexIteratorBase* it ) const;

	    class Private;
	    Private* const d;

	    friend class MutexIteratorBase;
	};
    }
}

#endif
