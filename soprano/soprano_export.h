/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2006 Kevin Ottens <ervin@kde.org>
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

#ifndef SOPRANO_EXPORT_H
#define SOPRANO_EXPORT_H

/* needed for KDE_EXPORT and KDE_IMPORT macros */
#include <QtCore/QtGlobal>

#if defined Q_OS_WIN

#ifndef SOPRANO_EXPORT
# if defined(MAKE_SOPRANO_LIB)
   /* We are building this library */ 
#  define SOPRANO_EXPORT Q_DECL_EXPORT
# else
   /* We are using this library */ 
#  define SOPRANO_EXPORT Q_DECL_IMPORT
# endif
#endif

#ifndef REDLANDBACKEND_EXPORT
# if defined(MAKE_REDLANDBACKEND_LIB)
   /* We are building this library */ 
#  define REDLANDBACKEND_EXPORT Q_DECL_EXPORT
# else
   /* We are using this library */ 
#  define REDLANDBACKEND_EXPORT Q_DECL_IMPORT
# endif
#endif

#ifndef THREESTOREBACKEND_EXPORT
# if defined(MAKE_REDLANDBACKEND_LIB)
   /* We are building this library */ 
#  define THREESTOREBACKEND_EXPORT Q_DECL_EXPORT
# else
   /* We are using this library */ 
#  define THREESTOREBACKEND_EXPORT Q_DECL_IMPORT
# endif
#endif

#else /* UNIX */

#define SOPRANO_EXPORT Q_DECL_EXPORT
#define REDLANDBACKEND_EXPORT Q_DECL_EXPORT
#define THREESTOREBACKEND_EXPORT Q_DECL_EXPORT

#endif

#endif
