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

#ifndef _SOPRANO_SERVER_COMMANDS_H_
#define _SOPRANO_SERVER_COMMANDS_H_

#include <qglobal.h>

//
// ChangeLog:
// ==========
// Protocol version 3:
//     Soprano 2.1
//     No changed commands but completely changed types streaming
//     no usage of operators anymore
//
// Protocol version 4:
//     Soprano 2.2
//     Added Transaction support (transactions are handled like normal models)
//     Fully backwards compatible
//
#define PROTOCOL_VERSION 4

namespace Soprano {
    namespace Server {
        const quint16 COMMAND_CREATE_MODEL = 0x1;
        const quint16 COMMAND_MODEL_ADD_STATEMENT = 0x2;
        const quint16 COMMAND_MODEL_REMOVE_STATEMENT = 0x3;
        const quint16 COMMAND_MODEL_REMOVE_ALL_STATEMENTS = 0x4;
        const quint16 COMMAND_MODEL_LIST_STATEMENTS = 0x5;
        const quint16 COMMAND_MODEL_CONTAINS_STATEMENT = 0x6;
        const quint16 COMMAND_MODEL_CONTAINS_ANY_STATEMENT = 0x7;
        const quint16 COMMAND_MODEL_LIST_CONTEXTS = 0x8;
        const quint16 COMMAND_MODEL_STATEMENT_COUNT = 0x9;
        const quint16 COMMAND_MODEL_IS_EMPTY = 0x10;
        const quint16 COMMAND_MODEL_QUERY = 0x11;
        const quint16 COMMAND_ITERATOR_NEXT = 0x12;
        const quint16 COMMAND_ITERATOR_CURRENT_STATEMENT = 0x13; /**< Works for both statement and query its. */
        const quint16 COMMAND_ITERATOR_CURRENT_NODE = 0x14;
        const quint16 COMMAND_ITERATOR_CURRENT_BINDINGSET = 0x15;
        const quint16 COMMAND_ITERATOR_CLOSE = 0x16;
        const quint16 COMMAND_ITERATOR_QUERY_TYPE = 0x17;
        const quint16 COMMAND_ITERATOR_QUERY_BOOL_VALUE = 0x18;
        const quint16 COMMAND_SUPPORTED_FEATURES = 0x19;
        const quint16 COMMAND_SUPPORTS_PROTOCOL_VERSION = 0x20;
        const quint16 COMMAND_MODEL_CREATE_BLANK_NODE = 0x21;
        const quint16 COMMAND_REMOVE_MODEL = 0x22;

        // Protocol version 4 commands
        const quint16 COMMAND_MODEL_START_TRANSACTION = 0x23;
        const quint16 COMMAND_TRANSACTION_COMMIT = 0x24;
        const quint16 COMMAND_TRANSACTION_ROLLBACK = 0x25;
        const quint16 COMMAND_TRANSACTION_CLOSE = 0x26;
    }
}

#endif
