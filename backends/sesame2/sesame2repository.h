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

#ifndef _SESAME2_REPOSITORY_H_
#define _SESAME2_REPOSITORY_H_

#include "jniobjectwrapper.h"

namespace Soprano {

    namespace Sesame2 {

	class ValueFactory;
	class RepositoryConnection;

	class RepositoryWrapper : public JNIObjectWrapper
	{
	public:
	    ~RepositoryWrapper();

	    ValueFactory* valueFactory();
	    RepositoryConnection* repositoryConnection();

	    /**
	     * A repo has to be initialized before usage.
	     */
	    bool initialize();
	    
	    /**
	     * Close an open repo and free all resources.
	     * The repo is not usable afterwards.
	     * Normally there is no need to call this as it is
	     * called in the desctructor.
	     */
	    void close();

	    jobject getContextIDs();

	    /**
	     * Create a repository that stores its data in path.
	     */
	    static RepositoryWrapper* create( const QString& path );

	    /**
	     * Create a repository in memory
	     */
	    static RepositoryWrapper* create();

	private:
	    RepositoryWrapper( jobject );

	    class Private;
	    Private* const d;
	};
    }
}

#endif
