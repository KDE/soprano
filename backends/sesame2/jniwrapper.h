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

#ifndef _JNI_WRAPPER_H_H
#define _JNI_WRAPPER_H_H

#include <QtCore/QString>

#include <jni.h>

#include <soprano/error.h>

#include "jobjectref.h"

/**
 * The JNI wrapper takes care of creating, initializing, and deleting the Java VM.
 * It also properly attaches new threads to the VM
 */
class JNIWrapper
{
public:
    ~JNIWrapper();

    JNIEnv* env() const;

    JObjectRef constructObject( const char* className, const char* constructorSignature = 0, ... );

    bool exceptionOccured();

    /**
     * If an exception occured, converts it to a Soprano::Error instance.
     */
    Soprano::Error::Error convertAndClearException();

    /**
     * If an exception occured, prints and then clears it
     * for debugging purposes.
     */
    void debugException();

    static JNIWrapper* instance();

private:
    JNIWrapper();

    static JNIWrapper* s_instance;

    class Private;
    Private* const d;
};

#endif
