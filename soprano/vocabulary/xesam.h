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

#ifndef _SOPRANO_XESAM_H_
#define _SOPRANO_XESAM_H_

#include <QtCore/QUrl>
#include "soprano_export.h"

namespace Soprano {
    namespace Vocabulary {
        namespace Xesam {
            /**
             * http://freedesktop.org/standards/xesam/1.0/core#
             */
            SOPRANO_EXPORT QUrl xesamNamespace();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#35mmEquivalent 
             * 
             * Photo metering mode 
             */
            SOPRANO_EXPORT QUrl xesam35mmEquivalent();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#Annotation 
             * 
             * Generic annotation. Annotation provides a set of document 
             * description properties(like subject, title, description) 
             * for a list of objects it links to. It can link to other annotations, 
             * however interpretation of this may differ between specific 
             * annotation classes.. 
             */
            SOPRANO_EXPORT QUrl Annotation();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#Archive 
             * 
             * Generic archive 
             */
            SOPRANO_EXPORT QUrl Archive();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#ArchivedFile 
             * 
             * File stored in an archive 
             */
            SOPRANO_EXPORT QUrl ArchivedFile();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#Audio 
             * 
             * Audio content 
             */
            SOPRANO_EXPORT QUrl Audio();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#AudioList 
             * 
             * Generic audio list(playlist). Linking to other content types 
             * is forbidden 
             */
            SOPRANO_EXPORT QUrl AudioList();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#BlockDevice 
             * 
             * Generic block device. Typically contains partitions/filesystems 
             */
            SOPRANO_EXPORT QUrl BlockDevice();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#Bookmark 
             * 
             * Bookmark. Currently there's nothing that would distinguish 
             * bookmarks and tags 
             */
            SOPRANO_EXPORT QUrl Bookmark();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#Contact 
             * 
             * Contact 
             */
            SOPRANO_EXPORT QUrl Contact();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#Content 
             * 
             * Generic content 
             */
            SOPRANO_EXPORT QUrl Content();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#DataObject 
             * 
             * Generic data object. Unites form and essense aspects represented 
             * by Content and Source. Used to aggreate properties that may 
             * be extracted from both content and source. 
             */
            SOPRANO_EXPORT QUrl DataObject();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#DeletedFile 
             * 
             * File deleted to trash 
             */
            SOPRANO_EXPORT QUrl DeletedFile();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#Document 
             * 
             * Document is an arrangement of various atomic data types with 
             * text being the primary data type. 
             */
            SOPRANO_EXPORT QUrl Document();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#Documentation 
             * 
             * Documentation is a document containing help, manuals, guides. 
             */
            SOPRANO_EXPORT QUrl Documentation();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#Email 
             * 
             * Email message 
             */
            SOPRANO_EXPORT QUrl Email();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#EmailAttachment 
             * 
             * Generic storage 
             */
            SOPRANO_EXPORT QUrl EmailAttachment();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#EmbeddedObject 
             * 
             * Generic embedded/inlined object: attachment, inlined SVG, 
             * script etc. 
             */
            SOPRANO_EXPORT QUrl EmbeddedObject();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#File 
             * 
             * Regular file stored in a filesystem 
             */
            SOPRANO_EXPORT QUrl File();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#FileSystem 
             * 
             * Filesystem differs from other containers in that it has total/free/occupied 
             * space(though DBs too have similar properties), has volume(content.title), 
             * UUID for *ix(content.ID), mount point(if mounted) 
             */
            SOPRANO_EXPORT QUrl FileSystem();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#Filelike 
             * 
             * File-like object 
             */
            SOPRANO_EXPORT QUrl Filelike();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#Folder 
             * 
             * Generic folder. In general, folders represent a tree-like 
             * structure(taxonomy), however on occasion this rule may violated 
             * in cases like symlinks. 
             */
            SOPRANO_EXPORT QUrl Folder();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#IMAP4Message 
             * 
             * IMAP4 mailbox message 
             */
            SOPRANO_EXPORT QUrl IMAP4Message();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#IMMessage 
             * 
             * Generic Instant Messaging message 
             */
            SOPRANO_EXPORT QUrl IMMessage();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#Image 
             * 
             * Visual content 
             */
            SOPRANO_EXPORT QUrl Image();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#MailingListEmail 
             * 
             * Email message addressed at/received from a mailing list 
             */
            SOPRANO_EXPORT QUrl MailingListEmail();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#Media 
             * 
             * Generic raster/sampled media is considered consisting of 
             * Sets of Samples being reproduced(played/shown) at once. We 
             * describe: sample data type(int/float), data bit depth,configuration(color 
             * space for images, channel count for audio); set configuration(pixel 
             * dimensions for image); set count and rate. 
             */
            SOPRANO_EXPORT QUrl Media();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#MediaList 
             * 
             * Generic media content list(playlist). Linking to other content 
             * types is forbidden 
             */
            SOPRANO_EXPORT QUrl MediaList();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#Message 
             * 
             * Generic message 
             */
            SOPRANO_EXPORT QUrl Message();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#MessageboxMessage 
             * 
             * Message stored in a message box 
             */
            SOPRANO_EXPORT QUrl MessageboxMessage();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#NewsGroupEmail 
             * 
             * Email message addressed at/received from a news group 
             */
            SOPRANO_EXPORT QUrl NewsGroupEmail();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#OfflineMedia 
             * 
             * Generic offline media. e.g. USB drive not attached at this moment. 
             */
            SOPRANO_EXPORT QUrl OfflineMedia();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#Organization 
             * 
             * Organization 
             */
            SOPRANO_EXPORT QUrl Organization();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#POP3Message 
             * 
             * POP3 mailbox message 
             */
            SOPRANO_EXPORT QUrl POP3Message();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#Person 
             * 
             * Person 
             */
            SOPRANO_EXPORT QUrl Person();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#PersonalEmail 
             * 
             * Personal email message(not related to a mailing list or discussion 
             * group) 
             */
            SOPRANO_EXPORT QUrl PersonalEmail();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#Photo 
             * 
             * An image with EXIF tags(photo) 
             */
            SOPRANO_EXPORT QUrl Photo();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#Presentation 
             * 
             * Presentation document 
             */
            SOPRANO_EXPORT QUrl Presentation();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#Project 
             * 
             * Generic project 
             */
            SOPRANO_EXPORT QUrl Project();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#RSSMessage 
             * 
             * RSS message(RSS feed item) 
             */
            SOPRANO_EXPORT QUrl RSSMessage();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#RemoteFile 
             * 
             * Remote file 
             */
            SOPRANO_EXPORT QUrl RemoteFile();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#RemoteMessageboxMessage 
             * 
             * Remote messagebox message 
             */
            SOPRANO_EXPORT QUrl RemoteMessageboxMessage();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#RemoteResource 
             * 
             * Generic remote resource 
             */
            SOPRANO_EXPORT QUrl RemoteResource();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#RevisionControlledFile 
             * 
             * File managed by a revision control system 
             */
            SOPRANO_EXPORT QUrl RevisionControlledFile();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#SoftwarePackage 
             * 
             * Software distribution package 
             */
            SOPRANO_EXPORT QUrl SoftwarePackage();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#Source 
             * 
             * Generic source 
             */
            SOPRANO_EXPORT QUrl Source();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#SourceCode 
             * 
             * Source Code 
             */
            SOPRANO_EXPORT QUrl SourceCode();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#Spreadsheet 
             * 
             * Spreadsheet document 
             */
            SOPRANO_EXPORT QUrl Spreadsheet();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#Tag 
             * 
             * Tag/keyword. As opposed to folders, there are no limitations 
             * on the structure of tags and arbitrary overlaps are possible. 
             */
            SOPRANO_EXPORT QUrl Tag();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#Text 
             * 
             * Defines a textual aspect of content. Properties represent 
             * only actual content intended for the user, not markup. Other 
             * parts of content like markup should be described using other 
             * clsses. Abstract class. 
             */
            SOPRANO_EXPORT QUrl Text();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#TextDocument 
             * 
             * Text document. For word processing apps. 
             */
            SOPRANO_EXPORT QUrl TextDocument();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#UncategorizedText 
             * 
             * Text content which doesn't fit any other text-based classes. 
             * Corresponds to Tracker's Text category. 
             */
            SOPRANO_EXPORT QUrl UncategorizedText();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#Video 
             * 
             * Video content 
             */
            SOPRANO_EXPORT QUrl Video();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#Visual 
             * 
             * Visual content 
             */
            SOPRANO_EXPORT QUrl Visual();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#XML 
             * 
             * XML content 
             */
            SOPRANO_EXPORT QUrl XML();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#acl 
             * 
             * File access control list 
             */
            SOPRANO_EXPORT QUrl acl();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#album 
             * 
             * Media album 
             */
            SOPRANO_EXPORT QUrl album();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#albumArtist 
             * 
             * Music album artist 
             */
            SOPRANO_EXPORT QUrl albumArtist();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#albumGain 
             * 
             * Gain adjustment of album 
             */
            SOPRANO_EXPORT QUrl albumGain();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#albumPeakGain 
             * 
             * Peak gain adjustment of album 
             */
            SOPRANO_EXPORT QUrl albumPeakGain();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#albumTrackCount 
             * 
             * Album track count 
             */
            SOPRANO_EXPORT QUrl albumTrackCount();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#aperture 
             * 
             * Photo aperture 
             */
            SOPRANO_EXPORT QUrl aperture();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#artist 
             * 
             * Music artist 
             */
            SOPRANO_EXPORT QUrl artist();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#asText 
             * 
             * Content plain-text representation for indexing purposes 
             */
            SOPRANO_EXPORT QUrl asText();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#aspectRatio 
             * 
             * Visual content aspect ratio 
             */
            SOPRANO_EXPORT QUrl aspectRatio();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#attachmentEncoding 
             * 
             * Email attachment encoding(base64,utf-7, etc) 
             */
            SOPRANO_EXPORT QUrl attachmentEncoding();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#audioBPM 
             * 
             * Beats per minute 
             */
            SOPRANO_EXPORT QUrl audioBPM();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#audioBitrate 
             * 
             * Audio Bitrate 
             */
            SOPRANO_EXPORT QUrl audioBitrate();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#audioChannels 
             * 
             * Audio channels 
             */
            SOPRANO_EXPORT QUrl audioChannels();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#audioCodec 
             * 
             * Audio codec 
             */
            SOPRANO_EXPORT QUrl audioCodec();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#audioCodecType 
             * 
             * Audio codec type 
             */
            SOPRANO_EXPORT QUrl audioCodecType();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#audioSampleBitDepth 
             * 
             * Audio sample data bit depth 
             */
            SOPRANO_EXPORT QUrl audioSampleBitDepth();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#audioSampleCount 
             * 
             * Audio sample count 
             */
            SOPRANO_EXPORT QUrl audioSampleCount();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#audioSampleDataType 
             * 
             * Audio sample data type 
             */
            SOPRANO_EXPORT QUrl audioSampleDataType();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#audioSampleRate 
             * 
             * Audio sample rate 
             */
            SOPRANO_EXPORT QUrl audioSampleRate();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#author 
             * 
             * Content author. Primary contributor. 
             */
            SOPRANO_EXPORT QUrl author();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#autoRating 
             * 
             * Rating of the object provided automatically by software, inferred 
             * from user behavior or other indirect indicators. 
             */
            SOPRANO_EXPORT QUrl autoRating();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#baseRevisionID 
             * 
             * RevisionID on which a revision-controlled file is based 
             */
            SOPRANO_EXPORT QUrl baseRevisionID();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#bcc 
             * 
             * BCC: 
             */
            SOPRANO_EXPORT QUrl bcc();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#birthDate 
             * 
             * Contact birthDate 
             */
            SOPRANO_EXPORT QUrl birthDate();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#blogContactURL 
             * 
             * Contact blog URL 
             */
            SOPRANO_EXPORT QUrl blogContactURL();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#cameraManufacturer 
             * 
             * Photo camera manufacturer 
             */
            SOPRANO_EXPORT QUrl cameraManufacturer();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#cameraModel 
             * 
             * Photo camera model 
             */
            SOPRANO_EXPORT QUrl cameraModel();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#cc 
             * 
             * CC: 
             */
            SOPRANO_EXPORT QUrl cc();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#ccdWidth 
             * 
             * Photo CCD Width 
             */
            SOPRANO_EXPORT QUrl ccdWidth();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#cellPhoneNumber 
             * 
             * Contact cell phone number 
             */
            SOPRANO_EXPORT QUrl cellPhoneNumber();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#changeCommitTime 
             * 
             * Time of the last change to the base file in the repository(preceding 
             * the baseRevisionID?) 
             */
            SOPRANO_EXPORT QUrl changeCommitTime();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#changeCommitter 
             * 
             * Who made the last change to the base file in the repository(preceding 
             * the baseRevisionID?) 
             */
            SOPRANO_EXPORT QUrl changeCommitter();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#characterCount 
             * 
             * Text character count 
             */
            SOPRANO_EXPORT QUrl characterCount();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#charset 
             * 
             * Content charset encoding 
             */
            SOPRANO_EXPORT QUrl charset();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#chatRoom 
             * 
             * Chatroom this message belongs to 
             */
            SOPRANO_EXPORT QUrl chatRoom();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#colorCount 
             * 
             * Visual content color count for palettes 
             */
            SOPRANO_EXPORT QUrl colorCount();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#colorSpace 
             * 
             * Visual content color space(RGB, CMYK etc) 
             */
            SOPRANO_EXPORT QUrl colorSpace();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#columnCount 
             * 
             * Spreadsheet column count 
             */
            SOPRANO_EXPORT QUrl columnCount();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#comment 
             * 
             * Object comment 
             */
            SOPRANO_EXPORT QUrl comment();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#commentCharacterCount 
             * 
             * Source code comment character count 
             */
            SOPRANO_EXPORT QUrl commentCharacterCount();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#commitDiff 
             * 
             * The diff of the content and the base file 
             */
            SOPRANO_EXPORT QUrl commitDiff();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#communicationChannel 
             * 
             * Message communication channel like chatroom name or mailing 
             * list 
             */
            SOPRANO_EXPORT QUrl communicationChannel();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#composer 
             * 
             * Music composer 
             */
            SOPRANO_EXPORT QUrl composer();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#compressionAlgorithm 
             * 
             * Compression algorithm for archivers which support several 
             */
            SOPRANO_EXPORT QUrl compressionAlgorithm();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#compressionLevel 
             * 
             * Level of compression. How much effort was spent towards achieving 
             * maximal compression ratio. 
             */
            SOPRANO_EXPORT QUrl compressionLevel();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#conflicts 
             * 
             * Software conflicts with 
             */
            SOPRANO_EXPORT QUrl conflicts();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#contactMedium 
             * 
             * Generic contact medium 
             */
            SOPRANO_EXPORT QUrl contactMedium();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#contactNick 
             * 
             * Contact nick 
             */
            SOPRANO_EXPORT QUrl contactNick();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#contactURL 
             * 
             * Contact URL 
             */
            SOPRANO_EXPORT QUrl contactURL();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#contains 
             * 
             * Containment relation 
             */
            SOPRANO_EXPORT QUrl contains();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#contentComment 
             * 
             * Content comment 
             */
            SOPRANO_EXPORT QUrl contentComment();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#contentCreated 
             * 
             * Content creation time 
             */
            SOPRANO_EXPORT QUrl contentCreated();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#contentKeyword 
             * 
             * Content keyword/tag 
             */
            SOPRANO_EXPORT QUrl contentKeyword();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#contentModified 
             * 
             * Content last modification time 
             */
            SOPRANO_EXPORT QUrl contentModified();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#contentType 
             * 
             * Email content mime type/charset 
             */
            SOPRANO_EXPORT QUrl contentType();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#contributor 
             * 
             * Content contributor. Secondary contributor. 
             */
            SOPRANO_EXPORT QUrl contributor();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#copyright 
             * 
             * Content copyright 
             */
            SOPRANO_EXPORT QUrl copyright();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#creator 
             * 
             * Abstract content creator. Use children 
             */
            SOPRANO_EXPORT QUrl creator();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#definesClass 
             * 
             * Source code defines class 
             */
            SOPRANO_EXPORT QUrl definesClass();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#definesFunction 
             * 
             * Source code defines function 
             */
            SOPRANO_EXPORT QUrl definesFunction();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#definesGlobalVariable 
             * 
             * Source code defines global variable 
             */
            SOPRANO_EXPORT QUrl definesGlobalVariable();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#deletionTime 
             * 
             * File deletion time 
             */
            SOPRANO_EXPORT QUrl deletionTime();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#depends 
             * 
             * Dependency relation 
             */
            SOPRANO_EXPORT QUrl depends();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#derivedFrom 
             * 
             * Links to the original content from which this content is derived 
             */
            SOPRANO_EXPORT QUrl derivedFrom();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#description 
             * 
             * Content description. Description of content an order of magnitude 
             * more elaborate than Title 
             */
            SOPRANO_EXPORT QUrl description();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#discNumber 
             * 
             * Audio cd number 
             */
            SOPRANO_EXPORT QUrl discNumber();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#disclaimer 
             * 
             * Content disclaimer 
             */
            SOPRANO_EXPORT QUrl disclaimer();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#documentCategory 
             * 
             * Document category: book, article, flyer, pamphlet whatever 
             */
            SOPRANO_EXPORT QUrl documentCategory();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#emailAddress 
             * 
             * Contact email address 
             */
            SOPRANO_EXPORT QUrl emailAddress();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#exposureBias 
             * 
             * Photo exposure bias 
             */
            SOPRANO_EXPORT QUrl exposureBias();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#exposureProgram 
             * 
             * Photo exposure program 
             */
            SOPRANO_EXPORT QUrl exposureProgram();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#exposureTime 
             * 
             * Photo exposure time 
             */
            SOPRANO_EXPORT QUrl exposureTime();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#faxPhoneNumber 
             * 
             * Contact fax phone number 
             */
            SOPRANO_EXPORT QUrl faxPhoneNumber();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#fileExtension 
             * 
             * File extension 
             */
            SOPRANO_EXPORT QUrl fileExtension();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#fileSystemType 
             * 
             * File system type e.g. ext3 
             */
            SOPRANO_EXPORT QUrl fileSystemType();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#fingerprint 
             * 
             * Content fingerprint: a small ID calculated from content byte 
             * stream, aimed at uniquely identifying the content. Abstract. 
             */
            SOPRANO_EXPORT QUrl fingerprint();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#firstUsed 
             * 
             * When the content was used for the first time 
             */
            SOPRANO_EXPORT QUrl firstUsed();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#flashUsed 
             * 
             * Photo flash used 
             */
            SOPRANO_EXPORT QUrl flashUsed();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#focalLength 
             * 
             * Photo focal length 
             */
            SOPRANO_EXPORT QUrl focalLength();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#focusDistance 
             * 
             * Photo focus distance 
             */
            SOPRANO_EXPORT QUrl focusDistance();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#formatSubtype 
             * 
             * Format subtype. Use to indicate format extensions/specifics 
             */
            SOPRANO_EXPORT QUrl formatSubtype();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#frameCount 
             * 
             * Visual content frame count 
             */
            SOPRANO_EXPORT QUrl frameCount();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#frameRate 
             * 
             * Visual content frame rate 
             */
            SOPRANO_EXPORT QUrl frameRate();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#freeSpace 
             * 
             * File system free space 
             */
            SOPRANO_EXPORT QUrl freeSpace();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#gender 
             * 
             * Contact gender 
             */
            SOPRANO_EXPORT QUrl gender();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#generator 
             * 
             * Software used to generate the content byte stream 
             */
            SOPRANO_EXPORT QUrl generator();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#generatorOptions 
             * 
             * Generator software options 
             */
            SOPRANO_EXPORT QUrl generatorOptions();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#genre 
             * 
             * Media genre 
             */
            SOPRANO_EXPORT QUrl genre();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#group 
             * 
             * File group 
             */
            SOPRANO_EXPORT QUrl group();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#height 
             * 
             * Visual content height 
             */
            SOPRANO_EXPORT QUrl height();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#homeEmailAddress 
             * 
             * Contact home email address 
             */
            SOPRANO_EXPORT QUrl homeEmailAddress();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#homePhoneNumber 
             * 
             * Contact home phone number 
             */
            SOPRANO_EXPORT QUrl homePhoneNumber();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#homePostalAddress 
             * 
             * Contact home address 
             */
            SOPRANO_EXPORT QUrl homePostalAddress();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#homepageContactURL 
             * 
             * Contact homepage URL 
             */
            SOPRANO_EXPORT QUrl homepageContactURL();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#horizontalResolution 
             * 
             * Visual content horizontal resolution 
             */
            SOPRANO_EXPORT QUrl horizontalResolution();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#id 
             * 
             * Content ID 
             */
            SOPRANO_EXPORT QUrl id();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#imContactMedium 
             * 
             * Generic IM contact medium 
             */
            SOPRANO_EXPORT QUrl imContactMedium();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#inReplyTo 
             * 
             * In-Reply-To: 
             */
            SOPRANO_EXPORT QUrl inReplyTo();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#interests 
             * 
             * Contact interests/hobbies 
             */
            SOPRANO_EXPORT QUrl interests();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#interlaceMode 
             * 
             * Visual content interlace mode 
             */
            SOPRANO_EXPORT QUrl interlaceMode();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#ircContactMedium 
             * 
             * Contact IRC ID@server 
             */
            SOPRANO_EXPORT QUrl ircContactMedium();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#isContentEncrypted 
             * 
             * Is some portion of content encrypted? Setting this field doesn't 
             * imply that all data is encrypted. 
             */
            SOPRANO_EXPORT QUrl isContentEncrypted();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#isEncrypted 
             * 
             * Is Object or part of it encrypted? 
             */
            SOPRANO_EXPORT QUrl isEncrypted();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#isImportant 
             * 
             * Is the message important 
             */
            SOPRANO_EXPORT QUrl isImportant();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#isInProgress 
             * 
             * Is the message in progress 
             */
            SOPRANO_EXPORT QUrl isInProgress();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#isRead 
             * 
             * Is the message read 
             */
            SOPRANO_EXPORT QUrl isRead();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#isSourceEncrypted 
             * 
             * Is archived file password-protected? 
             */
            SOPRANO_EXPORT QUrl isSourceEncrypted();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#isoEquivalent 
             * 
             * Photo ISO equivalent 
             */
            SOPRANO_EXPORT QUrl isoEquivalent();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#jabberContactMedium 
             * 
             * Contact Jabber ID 
             */
            SOPRANO_EXPORT QUrl jabberContactMedium();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#keyword 
             * 
             * Object keyword/tag 
             */
            SOPRANO_EXPORT QUrl keyword();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#knows 
             * 
             * FOAF:knows relation 
             */
            SOPRANO_EXPORT QUrl knows();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#language 
             * 
             * Content language 
             */
            SOPRANO_EXPORT QUrl language();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#lastRefreshed 
             * 
             * Last time the resource info was refreshed 
             */
            SOPRANO_EXPORT QUrl lastRefreshed();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#lastUsed 
             * 
             * When the content was last used. Different from last access as 
             * this only accounts usage by the user e.g. playing a song as opposed 
             * to apps scanning the HD 
             */
            SOPRANO_EXPORT QUrl lastUsed();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#legal 
             * 
             * Abstract content legal notice. 
             */
            SOPRANO_EXPORT QUrl legal();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#license 
             * 
             * Content license 
             */
            SOPRANO_EXPORT QUrl license();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#licenseType 
             * 
             * Content license type 
             */
            SOPRANO_EXPORT QUrl licenseType();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#lineCount 
             * 
             * Text line count 
             */
            SOPRANO_EXPORT QUrl lineCount();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#links 
             * 
             * Linking/mention relation 
             */
            SOPRANO_EXPORT QUrl links();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#localRevision 
             * 
             * Local revision number. An automatically generated ID that 
             * is changed everytime the generator software/revisioning 
             * system deems the content has changed. 
             */
            SOPRANO_EXPORT QUrl localRevision();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#lyricist 
             * 
             * Music lyricist 
             */
            SOPRANO_EXPORT QUrl lyricist();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#mailingList 
             * 
             * Mailing list this message belongs to 
             */
            SOPRANO_EXPORT QUrl mailingList();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#mailingPostalAddress 
             * 
             * Contact mailing address 
             */
            SOPRANO_EXPORT QUrl mailingPostalAddress();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#maintainer 
             * 
             * Content maintainer. 
             */
            SOPRANO_EXPORT QUrl maintainer();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#markupCharacterCount 
             * 
             * XML markup character count 
             */
            SOPRANO_EXPORT QUrl markupCharacterCount();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#md5Hash 
             * 
             * MD5 hash 
             */
            SOPRANO_EXPORT QUrl md5Hash();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#mediaBitrate 
             * 
             * Media bitrate 
             */
            SOPRANO_EXPORT QUrl mediaBitrate();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#mediaCodec 
             * 
             * Media codec 
             */
            SOPRANO_EXPORT QUrl mediaCodec();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#mediaCodecType 
             * 
             * Media codec type: lossless, CBR, ABR, VBR 
             */
            SOPRANO_EXPORT QUrl mediaCodecType();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#mediaDuration 
             * 
             * Media duration 
             */
            SOPRANO_EXPORT QUrl mediaDuration();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#mergeConflict 
             * 
             * If true, the file has a merge conflict(can't be cleanly merged 
             * into the repository) 
             */
            SOPRANO_EXPORT QUrl mergeConflict();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#meteringMode 
             * 
             * Photo metering mode 
             */
            SOPRANO_EXPORT QUrl meteringMode();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#mimeType 
             * 
             * Content mime-type 
             */
            SOPRANO_EXPORT QUrl mimeType();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#mountPoint 
             * 
             * File system mount point 
             */
            SOPRANO_EXPORT QUrl mountPoint();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#name 
             * 
             * Name provided by container 
             */
            SOPRANO_EXPORT QUrl name();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#newsGroup 
             * 
             * News group this message belongs to 
             */
            SOPRANO_EXPORT QUrl newsGroup();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#occupiedSpace 
             * 
             * File system occupied space 
             */
            SOPRANO_EXPORT QUrl occupiedSpace();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#orientation 
             * 
             * Photo orientation 
             */
            SOPRANO_EXPORT QUrl orientation();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#originURL 
             * 
             * Origin URL, e.g. where the file had been downloaded from 
             */
            SOPRANO_EXPORT QUrl originURL();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#originalLocation 
             * 
             * Deleted file original location 
             */
            SOPRANO_EXPORT QUrl originalLocation();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#owner 
             * 
             * File owner 
             */
            SOPRANO_EXPORT QUrl owner();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#pageCount 
             * 
             * Document page count. Slide count for presentations 
             */
            SOPRANO_EXPORT QUrl pageCount();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#paragraphCount 
             * 
             * Document paragraph count 
             */
            SOPRANO_EXPORT QUrl paragraphCount();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#performer 
             * 
             * Music performer 
             */
            SOPRANO_EXPORT QUrl performer();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#permissions 
             * 
             * File permissions 
             */
            SOPRANO_EXPORT QUrl permissions();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#personPhoto 
             * 
             * Contact photo/avatar 
             */
            SOPRANO_EXPORT QUrl personPhoto();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#phoneNumber 
             * 
             * Contact phone number 
             */
            SOPRANO_EXPORT QUrl phoneNumber();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#pixelDataBitDepth 
             * 
             * Visual content pixel data bit depth 
             */
            SOPRANO_EXPORT QUrl pixelDataBitDepth();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#pixelDataType 
             * 
             * Visual content pixel data type 
             */
            SOPRANO_EXPORT QUrl pixelDataType();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#postalAddress 
             * 
             * Contact postal address 
             */
            SOPRANO_EXPORT QUrl postalAddress();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#primaryRecipient 
             * 
             * Message primary recipient 
             */
            SOPRANO_EXPORT QUrl primaryRecipient();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#programmingLanguage 
             * 
             * Source code programming language 
             */
            SOPRANO_EXPORT QUrl programmingLanguage();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#receptionTime 
             * 
             * Message reception time 
             */
            SOPRANO_EXPORT QUrl receptionTime();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#recipient 
             * 
             * Message recipient 
             */
            SOPRANO_EXPORT QUrl recipient();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#related 
             * 
             * Abstract content relation. Use children 
             */
            SOPRANO_EXPORT QUrl related();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#remotePassword 
             * 
             * Remote resource user password 
             */
            SOPRANO_EXPORT QUrl remotePassword();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#remotePort 
             * 
             * Server port 
             */
            SOPRANO_EXPORT QUrl remotePort();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#remoteServer 
             * 
             * The server hosting the remote resource 
             */
            SOPRANO_EXPORT QUrl remoteServer();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#remoteUser 
             * 
             * Remote resource user name 
             */
            SOPRANO_EXPORT QUrl remoteUser();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#replyTo 
             * 
             * ReplyTo: 
             */
            SOPRANO_EXPORT QUrl replyTo();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#rowCount 
             * 
             * Spreadsheet row count 
             */
            SOPRANO_EXPORT QUrl rowCount();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#rssFeed 
             * 
             * RSS feed that provided the message 
             */
            SOPRANO_EXPORT QUrl rssFeed();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#sampleConfiguration 
             * 
             * Media sample configuration/arrangement of components 
             */
            SOPRANO_EXPORT QUrl sampleConfiguration();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#sampleDataBitDepth 
             * 
             * Media sample data bit depth: 8, 16, 24, 32 etc 
             */
            SOPRANO_EXPORT QUrl sampleDataBitDepth();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#sampleDataType 
             * 
             * Media sample data type: int, float 
             */
            SOPRANO_EXPORT QUrl sampleDataType();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#secondaryRecipient 
             * 
             * Message secondary recipient 
             */
            SOPRANO_EXPORT QUrl secondaryRecipient();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#seenAttachedAs 
             * 
             * Name of block device seen to contain the Content when it was online. 
             */
            SOPRANO_EXPORT QUrl seenAttachedAs();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#setCount 
             * 
             * Media set count. Sample count for audio(set=one sample per 
             * channel), frame count for video 
             */
            SOPRANO_EXPORT QUrl setCount();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#setRate 
             * 
             * Media set rate. Sample rate for audio(set=one sample per channel), 
             * FPS for video 
             */
            SOPRANO_EXPORT QUrl setRate();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#sha1Hash 
             * 
             * SHA1 hash 
             */
            SOPRANO_EXPORT QUrl sha1Hash();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#size 
             * 
             * Content/data size in bytes. See also storageSize 
             */
            SOPRANO_EXPORT QUrl size();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#sourceCreated 
             * 
             * Local copy creation time 
             */
            SOPRANO_EXPORT QUrl sourceCreated();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#sourceModified 
             * 
             * Local copy modification time 
             */
            SOPRANO_EXPORT QUrl sourceModified();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#storageSize 
             * 
             * Actual space occupied by the object in the source storage. e.g. 
             * compressed file size in archive 
             */
            SOPRANO_EXPORT QUrl storageSize();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#subject 
             * 
             * Content subject. The shortest possible description of content 
             */
            SOPRANO_EXPORT QUrl subject();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#summary 
             * 
             * Content summary. Description of content an order of magnitude 
             * more elaborate than Description 
             */
            SOPRANO_EXPORT QUrl summary();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#supercedes 
             * 
             * Software supercedes 
             */
            SOPRANO_EXPORT QUrl supercedes();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#targetQuality 
             * 
             * The desired level of quality loss of lossy compression 
             */
            SOPRANO_EXPORT QUrl targetQuality();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#title 
             * 
             * Content title. Description of content an order of magnitude 
             * more elaborate than Subject 
             */
            SOPRANO_EXPORT QUrl title();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#to 
             * 
             * To: 
             */
            SOPRANO_EXPORT QUrl to();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#totalSpace 
             * 
             * File system total usable space, unlike size, which is the byte 
             * size of the entire filesystem including overhead. 
             */
            SOPRANO_EXPORT QUrl totalSpace();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#totalUncompressedSize 
             * 
             * Archive total uncompressed size 
             */
            SOPRANO_EXPORT QUrl totalUncompressedSize();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#trackGain 
             * 
             * Gain adjustment of track 
             */
            SOPRANO_EXPORT QUrl trackGain();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#trackNumber 
             * 
             * Audio track number 
             */
            SOPRANO_EXPORT QUrl trackNumber();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#trackPeakGain 
             * 
             * Peak gain adjustment of track 
             */
            SOPRANO_EXPORT QUrl trackPeakGain();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#url 
             * 
             * URL to access the content 
             */
            SOPRANO_EXPORT QUrl url();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#useCount 
             * 
             * How many times the content was used. Only usage by the user(not 
             * general software access) counts. 
             */
            SOPRANO_EXPORT QUrl useCount();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#userComment 
             * 
             * User-provided comment 
             */
            SOPRANO_EXPORT QUrl userComment();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#userKeyword 
             * 
             * User-provided keywords 
             */
            SOPRANO_EXPORT QUrl userKeyword();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#userRating 
             * 
             * User-provided rating of the object 
             */
            SOPRANO_EXPORT QUrl userRating();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#usesNamespace 
             * 
             * Namespace refereced by the XML 
             */
            SOPRANO_EXPORT QUrl usesNamespace();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#version 
             * 
             * Content version 
             */
            SOPRANO_EXPORT QUrl version();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#verticalResolution 
             * 
             * Visual content vertical resolution 
             */
            SOPRANO_EXPORT QUrl verticalResolution();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#videoBitrate 
             * 
             * Video Bitrate 
             */
            SOPRANO_EXPORT QUrl videoBitrate();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#videoCodec 
             * 
             * Video codec 
             */
            SOPRANO_EXPORT QUrl videoCodec();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#videoCodecType 
             * 
             * Video codec type 
             */
            SOPRANO_EXPORT QUrl videoCodecType();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#whiteBalance 
             * 
             * Photo white balance 
             */
            SOPRANO_EXPORT QUrl whiteBalance();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#width 
             * 
             * Visual content width 
             */
            SOPRANO_EXPORT QUrl width();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#wordCount 
             * 
             * Text word count 
             */
            SOPRANO_EXPORT QUrl wordCount();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#workEmailAddress 
             * 
             * Contact work email address 
             */
            SOPRANO_EXPORT QUrl workEmailAddress();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#workPhoneNumber 
             * 
             * Contact work phone number 
             */
            SOPRANO_EXPORT QUrl workPhoneNumber();

            /**
             * http://freedesktop.org/standards/xesam/1.0/core#workPostalAddress 
             * 
             * Contact work address 
             */
            SOPRANO_EXPORT QUrl workPostalAddress();
        }
    }
}

#endif