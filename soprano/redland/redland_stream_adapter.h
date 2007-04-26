/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
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

#ifndef REDLAND_STREAM_ADAPTER_H
#define REDLAND_STREAM_ADAPTER_H

#include <redland.h>

#ifdef __cplusplus
extern "C" {
#endif

struct stream_adapter_s {
  librdf_stream *impl;
};

typedef struct stream_adapter_s stream_adapter;

void free_stream_adapter_backend( stream_adapter *s );

void free_stream_adapter( stream_adapter *s );

librdf_statement *stream_adapter_get_object( stream_adapter *s );

librdf_node *stream_adapter_get_context( stream_adapter *s );

void stream_adapter_next( stream_adapter *s );

int stream_adapter_end( stream_adapter *s );

#ifdef __cplusplus
}
#endif

#endif /* REDLAND_STREAM_ADAPTER_H */

