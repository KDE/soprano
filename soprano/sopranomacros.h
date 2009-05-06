/* This file is part of Soprano
 *
 * Copyright (C) 2008 Sebastian Trueg <trueg@kde.org>
 *
 * Macros in this file are based on macros from KDE's kdemacros.h
 * Copyright (c) 2002-2003 KDE Team
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

/**
 * \file sopranomacros.h
 *
 * This header defines several compiler-independent macros which are used
 * throughout Soprano. Most of these macros make use of GCC extensions; on other
 * compilers, they don't have any effect.
 */

#ifndef _SOPRANO_MACROS_H_
#define _SOPRANO_MACROS_H_

#ifdef __cplusplus
# include <QtCore/qglobal.h>
# ifndef SOPRANO_DEPRECATED
#   define SOPRANO_DEPRECATED Q_DECL_DEPRECATED
# endif
#endif

#ifndef SOPRANO_CONSTRUCTOR_DEPRECATED
# ifdef __GNUC__
#  if __GNUC__ == 3 && __GNUC_MINOR__ <= 3 
    /* GCC 3.3.x cannot handle Qt 4.1.2's definition of Q_DECL_CONSTRUCTOR_DEPRECATED */
#   define SOPRANO_CONSTRUCTOR_DEPRECATED
#  else
#   define SOPRANO_CONSTRUCTOR_DEPRECATED Q_DECL_CONSTRUCTOR_DEPRECATED
#  endif
# else
#  define SOPRANO_CONSTRUCTOR_DEPRECATED Q_DECL_CONSTRUCTOR_DEPRECATED
# endif
#endif


#endif
