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

#include "xesam.h"

class XesamPrivate
{
public:
    XesamPrivate()
        : xesam_namespace( "http://freedesktop.org/standards/xesam/1.0/core#" ),
          xesam_35mmEquivalent( "http://freedesktop.org/standards/xesam/1.0/core#35mmEquivalent" ),
          xesam_Annotation( "http://freedesktop.org/standards/xesam/1.0/core#Annotation" ),
          xesam_Archive( "http://freedesktop.org/standards/xesam/1.0/core#Archive" ),
          xesam_ArchivedFile( "http://freedesktop.org/standards/xesam/1.0/core#ArchivedFile" ),
          xesam_Audio( "http://freedesktop.org/standards/xesam/1.0/core#Audio" ),
          xesam_AudioList( "http://freedesktop.org/standards/xesam/1.0/core#AudioList" ),
          xesam_BlockDevice( "http://freedesktop.org/standards/xesam/1.0/core#BlockDevice" ),
          xesam_Bookmark( "http://freedesktop.org/standards/xesam/1.0/core#Bookmark" ),
          xesam_CommunicationChannel( "http://freedesktop.org/standards/xesam/1.0/core#CommunicationChannel" ),
          xesam_Contact( "http://freedesktop.org/standards/xesam/1.0/core#Contact" ),
          xesam_ContactGroup( "http://freedesktop.org/standards/xesam/1.0/core#ContactGroup" ),
          xesam_Content( "http://freedesktop.org/standards/xesam/1.0/core#Content" ),
          xesam_DataObject( "http://freedesktop.org/standards/xesam/1.0/core#DataObject" ),
          xesam_DeletedFile( "http://freedesktop.org/standards/xesam/1.0/core#DeletedFile" ),
          xesam_Document( "http://freedesktop.org/standards/xesam/1.0/core#Document" ),
          xesam_Documentation( "http://freedesktop.org/standards/xesam/1.0/core#Documentation" ),
          xesam_Email( "http://freedesktop.org/standards/xesam/1.0/core#Email" ),
          xesam_EmailAttachment( "http://freedesktop.org/standards/xesam/1.0/core#EmailAttachment" ),
          xesam_EmbeddedObject( "http://freedesktop.org/standards/xesam/1.0/core#EmbeddedObject" ),
          xesam_File( "http://freedesktop.org/standards/xesam/1.0/core#File" ),
          xesam_FileSystem( "http://freedesktop.org/standards/xesam/1.0/core#FileSystem" ),
          xesam_Filelike( "http://freedesktop.org/standards/xesam/1.0/core#Filelike" ),
          xesam_Folder( "http://freedesktop.org/standards/xesam/1.0/core#Folder" ),
          xesam_IMAP4Message( "http://freedesktop.org/standards/xesam/1.0/core#IMAP4Message" ),
          xesam_IMMessage( "http://freedesktop.org/standards/xesam/1.0/core#IMMessage" ),
          xesam_Image( "http://freedesktop.org/standards/xesam/1.0/core#Image" ),
          xesam_MailingList( "http://freedesktop.org/standards/xesam/1.0/core#MailingList" ),
          xesam_MailingListEmail( "http://freedesktop.org/standards/xesam/1.0/core#MailingListEmail" ),
          xesam_Media( "http://freedesktop.org/standards/xesam/1.0/core#Media" ),
          xesam_MediaList( "http://freedesktop.org/standards/xesam/1.0/core#MediaList" ),
          xesam_Message( "http://freedesktop.org/standards/xesam/1.0/core#Message" ),
          xesam_MessageboxMessage( "http://freedesktop.org/standards/xesam/1.0/core#MessageboxMessage" ),
          xesam_Music( "http://freedesktop.org/standards/xesam/1.0/core#Music" ),
          xesam_NewsGroup( "http://freedesktop.org/standards/xesam/1.0/core#NewsGroup" ),
          xesam_NewsGroupEmail( "http://freedesktop.org/standards/xesam/1.0/core#NewsGroupEmail" ),
          xesam_OfflineMedia( "http://freedesktop.org/standards/xesam/1.0/core#OfflineMedia" ),
          xesam_Organization( "http://freedesktop.org/standards/xesam/1.0/core#Organization" ),
          xesam_POP3Message( "http://freedesktop.org/standards/xesam/1.0/core#POP3Message" ),
          xesam_Person( "http://freedesktop.org/standards/xesam/1.0/core#Person" ),
          xesam_PersonalEmail( "http://freedesktop.org/standards/xesam/1.0/core#PersonalEmail" ),
          xesam_Photo( "http://freedesktop.org/standards/xesam/1.0/core#Photo" ),
          xesam_Presentation( "http://freedesktop.org/standards/xesam/1.0/core#Presentation" ),
          xesam_Project( "http://freedesktop.org/standards/xesam/1.0/core#Project" ),
          xesam_RSSFeed( "http://freedesktop.org/standards/xesam/1.0/core#RSSFeed" ),
          xesam_RSSMessage( "http://freedesktop.org/standards/xesam/1.0/core#RSSMessage" ),
          xesam_RemoteFile( "http://freedesktop.org/standards/xesam/1.0/core#RemoteFile" ),
          xesam_RemoteMessageboxMessage( "http://freedesktop.org/standards/xesam/1.0/core#RemoteMessageboxMessage" ),
          xesam_RemoteResource( "http://freedesktop.org/standards/xesam/1.0/core#RemoteResource" ),
          xesam_RevisionControlledFile( "http://freedesktop.org/standards/xesam/1.0/core#RevisionControlledFile" ),
          xesam_RevisionControlledRepository( "http://freedesktop.org/standards/xesam/1.0/core#RevisionControlledRepository" ),
          xesam_SoftwarePackage( "http://freedesktop.org/standards/xesam/1.0/core#SoftwarePackage" ),
          xesam_Source( "http://freedesktop.org/standards/xesam/1.0/core#Source" ),
          xesam_SourceCode( "http://freedesktop.org/standards/xesam/1.0/core#SourceCode" ),
          xesam_Spreadsheet( "http://freedesktop.org/standards/xesam/1.0/core#Spreadsheet" ),
          xesam_SystemResource( "http://freedesktop.org/standards/xesam/1.0/core#SystemResource" ),
          xesam_Tag( "http://freedesktop.org/standards/xesam/1.0/core#Tag" ),
          xesam_Text( "http://freedesktop.org/standards/xesam/1.0/core#Text" ),
          xesam_TextDocument( "http://freedesktop.org/standards/xesam/1.0/core#TextDocument" ),
          xesam_UncategorizedText( "http://freedesktop.org/standards/xesam/1.0/core#UncategorizedText" ),
          xesam_Video( "http://freedesktop.org/standards/xesam/1.0/core#Video" ),
          xesam_Visual( "http://freedesktop.org/standards/xesam/1.0/core#Visual" ),
          xesam_XML( "http://freedesktop.org/standards/xesam/1.0/core#XML" ),
          xesam_acl( "http://freedesktop.org/standards/xesam/1.0/core#acl" ),
          xesam_album( "http://freedesktop.org/standards/xesam/1.0/core#album" ),
          xesam_albumArtist( "http://freedesktop.org/standards/xesam/1.0/core#albumArtist" ),
          xesam_albumGain( "http://freedesktop.org/standards/xesam/1.0/core#albumGain" ),
          xesam_albumPeakGain( "http://freedesktop.org/standards/xesam/1.0/core#albumPeakGain" ),
          xesam_albumTrackCount( "http://freedesktop.org/standards/xesam/1.0/core#albumTrackCount" ),
          xesam_aperture( "http://freedesktop.org/standards/xesam/1.0/core#aperture" ),
          xesam_artist( "http://freedesktop.org/standards/xesam/1.0/core#artist" ),
          xesam_asText( "http://freedesktop.org/standards/xesam/1.0/core#asText" ),
          xesam_aspectRatio( "http://freedesktop.org/standards/xesam/1.0/core#aspectRatio" ),
          xesam_attachmentEncoding( "http://freedesktop.org/standards/xesam/1.0/core#attachmentEncoding" ),
          xesam_audioBPM( "http://freedesktop.org/standards/xesam/1.0/core#audioBPM" ),
          xesam_audioBitrate( "http://freedesktop.org/standards/xesam/1.0/core#audioBitrate" ),
          xesam_audioChannels( "http://freedesktop.org/standards/xesam/1.0/core#audioChannels" ),
          xesam_audioCodec( "http://freedesktop.org/standards/xesam/1.0/core#audioCodec" ),
          xesam_audioCodecType( "http://freedesktop.org/standards/xesam/1.0/core#audioCodecType" ),
          xesam_audioSampleBitDepth( "http://freedesktop.org/standards/xesam/1.0/core#audioSampleBitDepth" ),
          xesam_audioSampleCount( "http://freedesktop.org/standards/xesam/1.0/core#audioSampleCount" ),
          xesam_audioSampleDataType( "http://freedesktop.org/standards/xesam/1.0/core#audioSampleDataType" ),
          xesam_audioSampleRate( "http://freedesktop.org/standards/xesam/1.0/core#audioSampleRate" ),
          xesam_author( "http://freedesktop.org/standards/xesam/1.0/core#author" ),
          xesam_autoRating( "http://freedesktop.org/standards/xesam/1.0/core#autoRating" ),
          xesam_baseRevisionID( "http://freedesktop.org/standards/xesam/1.0/core#baseRevisionID" ),
          xesam_bcc( "http://freedesktop.org/standards/xesam/1.0/core#bcc" ),
          xesam_birthDate( "http://freedesktop.org/standards/xesam/1.0/core#birthDate" ),
          xesam_blogContactURL( "http://freedesktop.org/standards/xesam/1.0/core#blogContactURL" ),
          xesam_cameraManufacturer( "http://freedesktop.org/standards/xesam/1.0/core#cameraManufacturer" ),
          xesam_cameraModel( "http://freedesktop.org/standards/xesam/1.0/core#cameraModel" ),
          xesam_cc( "http://freedesktop.org/standards/xesam/1.0/core#cc" ),
          xesam_ccdWidth( "http://freedesktop.org/standards/xesam/1.0/core#ccdWidth" ),
          xesam_cellPhoneNumber( "http://freedesktop.org/standards/xesam/1.0/core#cellPhoneNumber" ),
          xesam_changeCommitTime( "http://freedesktop.org/standards/xesam/1.0/core#changeCommitTime" ),
          xesam_changeCommitter( "http://freedesktop.org/standards/xesam/1.0/core#changeCommitter" ),
          xesam_characterCount( "http://freedesktop.org/standards/xesam/1.0/core#characterCount" ),
          xesam_charset( "http://freedesktop.org/standards/xesam/1.0/core#charset" ),
          xesam_chatRoom( "http://freedesktop.org/standards/xesam/1.0/core#chatRoom" ),
          xesam_colorCount( "http://freedesktop.org/standards/xesam/1.0/core#colorCount" ),
          xesam_colorSpace( "http://freedesktop.org/standards/xesam/1.0/core#colorSpace" ),
          xesam_columnCount( "http://freedesktop.org/standards/xesam/1.0/core#columnCount" ),
          xesam_comment( "http://freedesktop.org/standards/xesam/1.0/core#comment" ),
          xesam_commentCharacterCount( "http://freedesktop.org/standards/xesam/1.0/core#commentCharacterCount" ),
          xesam_commitDiff( "http://freedesktop.org/standards/xesam/1.0/core#commitDiff" ),
          xesam_communicationChannel( "http://freedesktop.org/standards/xesam/1.0/core#communicationChannel" ),
          xesam_composer( "http://freedesktop.org/standards/xesam/1.0/core#composer" ),
          xesam_compressionAlgorithm( "http://freedesktop.org/standards/xesam/1.0/core#compressionAlgorithm" ),
          xesam_compressionLevel( "http://freedesktop.org/standards/xesam/1.0/core#compressionLevel" ),
          xesam_conflicts( "http://freedesktop.org/standards/xesam/1.0/core#conflicts" ),
          xesam_contactMedium( "http://freedesktop.org/standards/xesam/1.0/core#contactMedium" ),
          xesam_contactNick( "http://freedesktop.org/standards/xesam/1.0/core#contactNick" ),
          xesam_contactURL( "http://freedesktop.org/standards/xesam/1.0/core#contactURL" ),
          xesam_contains( "http://freedesktop.org/standards/xesam/1.0/core#contains" ),
          xesam_contentComment( "http://freedesktop.org/standards/xesam/1.0/core#contentComment" ),
          xesam_contentCreated( "http://freedesktop.org/standards/xesam/1.0/core#contentCreated" ),
          xesam_contentKeyword( "http://freedesktop.org/standards/xesam/1.0/core#contentKeyword" ),
          xesam_contentModified( "http://freedesktop.org/standards/xesam/1.0/core#contentModified" ),
          xesam_contentType( "http://freedesktop.org/standards/xesam/1.0/core#contentType" ),
          xesam_contributor( "http://freedesktop.org/standards/xesam/1.0/core#contributor" ),
          xesam_copyright( "http://freedesktop.org/standards/xesam/1.0/core#copyright" ),
          xesam_creator( "http://freedesktop.org/standards/xesam/1.0/core#creator" ),
          xesam_definesClass( "http://freedesktop.org/standards/xesam/1.0/core#definesClass" ),
          xesam_definesFunction( "http://freedesktop.org/standards/xesam/1.0/core#definesFunction" ),
          xesam_definesGlobalVariable( "http://freedesktop.org/standards/xesam/1.0/core#definesGlobalVariable" ),
          xesam_deletionTime( "http://freedesktop.org/standards/xesam/1.0/core#deletionTime" ),
          xesam_depends( "http://freedesktop.org/standards/xesam/1.0/core#depends" ),
          xesam_derivedFrom( "http://freedesktop.org/standards/xesam/1.0/core#derivedFrom" ),
          xesam_description( "http://freedesktop.org/standards/xesam/1.0/core#description" ),
          xesam_discNumber( "http://freedesktop.org/standards/xesam/1.0/core#discNumber" ),
          xesam_disclaimer( "http://freedesktop.org/standards/xesam/1.0/core#disclaimer" ),
          xesam_documentCategory( "http://freedesktop.org/standards/xesam/1.0/core#documentCategory" ),
          xesam_emailAddress( "http://freedesktop.org/standards/xesam/1.0/core#emailAddress" ),
          xesam_exposureBias( "http://freedesktop.org/standards/xesam/1.0/core#exposureBias" ),
          xesam_exposureProgram( "http://freedesktop.org/standards/xesam/1.0/core#exposureProgram" ),
          xesam_exposureTime( "http://freedesktop.org/standards/xesam/1.0/core#exposureTime" ),
          xesam_familyName( "http://freedesktop.org/standards/xesam/1.0/core#familyName" ),
          xesam_faxPhoneNumber( "http://freedesktop.org/standards/xesam/1.0/core#faxPhoneNumber" ),
          xesam_fileExtension( "http://freedesktop.org/standards/xesam/1.0/core#fileExtension" ),
          xesam_fileSystemType( "http://freedesktop.org/standards/xesam/1.0/core#fileSystemType" ),
          xesam_fingerprint( "http://freedesktop.org/standards/xesam/1.0/core#fingerprint" ),
          xesam_firstUsed( "http://freedesktop.org/standards/xesam/1.0/core#firstUsed" ),
          xesam_flashUsed( "http://freedesktop.org/standards/xesam/1.0/core#flashUsed" ),
          xesam_focalLength( "http://freedesktop.org/standards/xesam/1.0/core#focalLength" ),
          xesam_focusDistance( "http://freedesktop.org/standards/xesam/1.0/core#focusDistance" ),
          xesam_formatSubtype( "http://freedesktop.org/standards/xesam/1.0/core#formatSubtype" ),
          xesam_frameCount( "http://freedesktop.org/standards/xesam/1.0/core#frameCount" ),
          xesam_frameRate( "http://freedesktop.org/standards/xesam/1.0/core#frameRate" ),
          xesam_freeSpace( "http://freedesktop.org/standards/xesam/1.0/core#freeSpace" ),
          xesam_gender( "http://freedesktop.org/standards/xesam/1.0/core#gender" ),
          xesam_generator( "http://freedesktop.org/standards/xesam/1.0/core#generator" ),
          xesam_generatorOptions( "http://freedesktop.org/standards/xesam/1.0/core#generatorOptions" ),
          xesam_genre( "http://freedesktop.org/standards/xesam/1.0/core#genre" ),
          xesam_givenName( "http://freedesktop.org/standards/xesam/1.0/core#givenName" ),
          xesam_group( "http://freedesktop.org/standards/xesam/1.0/core#group" ),
          xesam_height( "http://freedesktop.org/standards/xesam/1.0/core#height" ),
          xesam_homeEmailAddress( "http://freedesktop.org/standards/xesam/1.0/core#homeEmailAddress" ),
          xesam_homePhoneNumber( "http://freedesktop.org/standards/xesam/1.0/core#homePhoneNumber" ),
          xesam_homePostalAddress( "http://freedesktop.org/standards/xesam/1.0/core#homePostalAddress" ),
          xesam_homepageContactURL( "http://freedesktop.org/standards/xesam/1.0/core#homepageContactURL" ),
          xesam_honorificPrefix( "http://freedesktop.org/standards/xesam/1.0/core#honorificPrefix" ),
          xesam_honorificSuffix( "http://freedesktop.org/standards/xesam/1.0/core#honorificSuffix" ),
          xesam_horizontalResolution( "http://freedesktop.org/standards/xesam/1.0/core#horizontalResolution" ),
          xesam_id( "http://freedesktop.org/standards/xesam/1.0/core#id" ),
          xesam_imContactMedium( "http://freedesktop.org/standards/xesam/1.0/core#imContactMedium" ),
          xesam_inReplyTo( "http://freedesktop.org/standards/xesam/1.0/core#inReplyTo" ),
          xesam_interest( "http://freedesktop.org/standards/xesam/1.0/core#interest" ),
          xesam_interlaceMode( "http://freedesktop.org/standards/xesam/1.0/core#interlaceMode" ),
          xesam_ircContactMedium( "http://freedesktop.org/standards/xesam/1.0/core#ircContactMedium" ),
          xesam_isContentEncrypted( "http://freedesktop.org/standards/xesam/1.0/core#isContentEncrypted" ),
          xesam_isEncrypted( "http://freedesktop.org/standards/xesam/1.0/core#isEncrypted" ),
          xesam_isImportant( "http://freedesktop.org/standards/xesam/1.0/core#isImportant" ),
          xesam_isInProgress( "http://freedesktop.org/standards/xesam/1.0/core#isInProgress" ),
          xesam_isPublicChannel( "http://freedesktop.org/standards/xesam/1.0/core#isPublicChannel" ),
          xesam_isRead( "http://freedesktop.org/standards/xesam/1.0/core#isRead" ),
          xesam_isSourceEncrypted( "http://freedesktop.org/standards/xesam/1.0/core#isSourceEncrypted" ),
          xesam_isoEquivalent( "http://freedesktop.org/standards/xesam/1.0/core#isoEquivalent" ),
          xesam_jabberContactMedium( "http://freedesktop.org/standards/xesam/1.0/core#jabberContactMedium" ),
          xesam_keyword( "http://freedesktop.org/standards/xesam/1.0/core#keyword" ),
          xesam_knows( "http://freedesktop.org/standards/xesam/1.0/core#knows" ),
          xesam_language( "http://freedesktop.org/standards/xesam/1.0/core#language" ),
          xesam_lastRefreshed( "http://freedesktop.org/standards/xesam/1.0/core#lastRefreshed" ),
          xesam_lastUsed( "http://freedesktop.org/standards/xesam/1.0/core#lastUsed" ),
          xesam_legal( "http://freedesktop.org/standards/xesam/1.0/core#legal" ),
          xesam_license( "http://freedesktop.org/standards/xesam/1.0/core#license" ),
          xesam_licenseType( "http://freedesktop.org/standards/xesam/1.0/core#licenseType" ),
          xesam_lineCount( "http://freedesktop.org/standards/xesam/1.0/core#lineCount" ),
          xesam_links( "http://freedesktop.org/standards/xesam/1.0/core#links" ),
          xesam_localRevision( "http://freedesktop.org/standards/xesam/1.0/core#localRevision" ),
          xesam_lyricist( "http://freedesktop.org/standards/xesam/1.0/core#lyricist" ),
          xesam_mailingList( "http://freedesktop.org/standards/xesam/1.0/core#mailingList" ),
          xesam_mailingPostalAddress( "http://freedesktop.org/standards/xesam/1.0/core#mailingPostalAddress" ),
          xesam_maintainer( "http://freedesktop.org/standards/xesam/1.0/core#maintainer" ),
          xesam_markupCharacterCount( "http://freedesktop.org/standards/xesam/1.0/core#markupCharacterCount" ),
          xesam_md5Hash( "http://freedesktop.org/standards/xesam/1.0/core#md5Hash" ),
          xesam_mediaBitrate( "http://freedesktop.org/standards/xesam/1.0/core#mediaBitrate" ),
          xesam_mediaCodec( "http://freedesktop.org/standards/xesam/1.0/core#mediaCodec" ),
          xesam_mediaCodecType( "http://freedesktop.org/standards/xesam/1.0/core#mediaCodecType" ),
          xesam_mediaDuration( "http://freedesktop.org/standards/xesam/1.0/core#mediaDuration" ),
          xesam_mergeConflict( "http://freedesktop.org/standards/xesam/1.0/core#mergeConflict" ),
          xesam_meteringMode( "http://freedesktop.org/standards/xesam/1.0/core#meteringMode" ),
          xesam_mimeType( "http://freedesktop.org/standards/xesam/1.0/core#mimeType" ),
          xesam_mountPoint( "http://freedesktop.org/standards/xesam/1.0/core#mountPoint" ),
          xesam_name( "http://freedesktop.org/standards/xesam/1.0/core#name" ),
          xesam_newsGroup( "http://freedesktop.org/standards/xesam/1.0/core#newsGroup" ),
          xesam_occupiedSpace( "http://freedesktop.org/standards/xesam/1.0/core#occupiedSpace" ),
          xesam_orientation( "http://freedesktop.org/standards/xesam/1.0/core#orientation" ),
          xesam_originURL( "http://freedesktop.org/standards/xesam/1.0/core#originURL" ),
          xesam_originalLocation( "http://freedesktop.org/standards/xesam/1.0/core#originalLocation" ),
          xesam_otherName( "http://freedesktop.org/standards/xesam/1.0/core#otherName" ),
          xesam_owner( "http://freedesktop.org/standards/xesam/1.0/core#owner" ),
          xesam_pageCount( "http://freedesktop.org/standards/xesam/1.0/core#pageCount" ),
          xesam_paragraphCount( "http://freedesktop.org/standards/xesam/1.0/core#paragraphCount" ),
          xesam_performer( "http://freedesktop.org/standards/xesam/1.0/core#performer" ),
          xesam_permissions( "http://freedesktop.org/standards/xesam/1.0/core#permissions" ),
          xesam_personPhoto( "http://freedesktop.org/standards/xesam/1.0/core#personPhoto" ),
          xesam_phoneNumber( "http://freedesktop.org/standards/xesam/1.0/core#phoneNumber" ),
          xesam_pixelDataBitDepth( "http://freedesktop.org/standards/xesam/1.0/core#pixelDataBitDepth" ),
          xesam_pixelDataType( "http://freedesktop.org/standards/xesam/1.0/core#pixelDataType" ),
          xesam_postalAddress( "http://freedesktop.org/standards/xesam/1.0/core#postalAddress" ),
          xesam_primaryRecipient( "http://freedesktop.org/standards/xesam/1.0/core#primaryRecipient" ),
          xesam_programmingLanguage( "http://freedesktop.org/standards/xesam/1.0/core#programmingLanguage" ),
          xesam_receptionTime( "http://freedesktop.org/standards/xesam/1.0/core#receptionTime" ),
          xesam_recipient( "http://freedesktop.org/standards/xesam/1.0/core#recipient" ),
          xesam_related( "http://freedesktop.org/standards/xesam/1.0/core#related" ),
          xesam_remotePassword( "http://freedesktop.org/standards/xesam/1.0/core#remotePassword" ),
          xesam_remotePort( "http://freedesktop.org/standards/xesam/1.0/core#remotePort" ),
          xesam_remoteServer( "http://freedesktop.org/standards/xesam/1.0/core#remoteServer" ),
          xesam_remoteUser( "http://freedesktop.org/standards/xesam/1.0/core#remoteUser" ),
          xesam_replyTo( "http://freedesktop.org/standards/xesam/1.0/core#replyTo" ),
          xesam_rowCount( "http://freedesktop.org/standards/xesam/1.0/core#rowCount" ),
          xesam_rssFeed( "http://freedesktop.org/standards/xesam/1.0/core#rssFeed" ),
          xesam_sampleConfiguration( "http://freedesktop.org/standards/xesam/1.0/core#sampleConfiguration" ),
          xesam_sampleDataBitDepth( "http://freedesktop.org/standards/xesam/1.0/core#sampleDataBitDepth" ),
          xesam_sampleDataType( "http://freedesktop.org/standards/xesam/1.0/core#sampleDataType" ),
          xesam_secondaryRecipient( "http://freedesktop.org/standards/xesam/1.0/core#secondaryRecipient" ),
          xesam_seenAttachedAs( "http://freedesktop.org/standards/xesam/1.0/core#seenAttachedAs" ),
          xesam_setCount( "http://freedesktop.org/standards/xesam/1.0/core#setCount" ),
          xesam_setRate( "http://freedesktop.org/standards/xesam/1.0/core#setRate" ),
          xesam_sha1Hash( "http://freedesktop.org/standards/xesam/1.0/core#sha1Hash" ),
          xesam_size( "http://freedesktop.org/standards/xesam/1.0/core#size" ),
          xesam_sourceCreated( "http://freedesktop.org/standards/xesam/1.0/core#sourceCreated" ),
          xesam_sourceModified( "http://freedesktop.org/standards/xesam/1.0/core#sourceModified" ),
          xesam_storageSize( "http://freedesktop.org/standards/xesam/1.0/core#storageSize" ),
          xesam_subject( "http://freedesktop.org/standards/xesam/1.0/core#subject" ),
          xesam_summary( "http://freedesktop.org/standards/xesam/1.0/core#summary" ),
          xesam_supercedes( "http://freedesktop.org/standards/xesam/1.0/core#supercedes" ),
          xesam_targetQuality( "http://freedesktop.org/standards/xesam/1.0/core#targetQuality" ),
          xesam_title( "http://freedesktop.org/standards/xesam/1.0/core#title" ),
          xesam_to( "http://freedesktop.org/standards/xesam/1.0/core#to" ),
          xesam_totalSpace( "http://freedesktop.org/standards/xesam/1.0/core#totalSpace" ),
          xesam_totalUncompressedSize( "http://freedesktop.org/standards/xesam/1.0/core#totalUncompressedSize" ),
          xesam_trackGain( "http://freedesktop.org/standards/xesam/1.0/core#trackGain" ),
          xesam_trackNumber( "http://freedesktop.org/standards/xesam/1.0/core#trackNumber" ),
          xesam_trackPeakGain( "http://freedesktop.org/standards/xesam/1.0/core#trackPeakGain" ),
          xesam_url( "http://freedesktop.org/standards/xesam/1.0/core#url" ),
          xesam_useCount( "http://freedesktop.org/standards/xesam/1.0/core#useCount" ),
          xesam_userComment( "http://freedesktop.org/standards/xesam/1.0/core#userComment" ),
          xesam_userKeyword( "http://freedesktop.org/standards/xesam/1.0/core#userKeyword" ),
          xesam_userRating( "http://freedesktop.org/standards/xesam/1.0/core#userRating" ),
          xesam_usesNamespace( "http://freedesktop.org/standards/xesam/1.0/core#usesNamespace" ),
          xesam_version( "http://freedesktop.org/standards/xesam/1.0/core#version" ),
          xesam_verticalResolution( "http://freedesktop.org/standards/xesam/1.0/core#verticalResolution" ),
          xesam_videoBitrate( "http://freedesktop.org/standards/xesam/1.0/core#videoBitrate" ),
          xesam_videoCodec( "http://freedesktop.org/standards/xesam/1.0/core#videoCodec" ),
          xesam_videoCodecType( "http://freedesktop.org/standards/xesam/1.0/core#videoCodecType" ),
          xesam_whiteBalance( "http://freedesktop.org/standards/xesam/1.0/core#whiteBalance" ),
          xesam_width( "http://freedesktop.org/standards/xesam/1.0/core#width" ),
          xesam_wordCount( "http://freedesktop.org/standards/xesam/1.0/core#wordCount" ),
          xesam_workEmailAddress( "http://freedesktop.org/standards/xesam/1.0/core#workEmailAddress" ),
          xesam_workPhoneNumber( "http://freedesktop.org/standards/xesam/1.0/core#workPhoneNumber" ),
          xesam_workPostalAddress( "http://freedesktop.org/standards/xesam/1.0/core#workPostalAddress" ) {
    }

    QUrl xesam_namespace;
    QUrl xesam_35mmEquivalent;
    QUrl xesam_Annotation;
    QUrl xesam_Archive;
    QUrl xesam_ArchivedFile;
    QUrl xesam_Audio;
    QUrl xesam_AudioList;
    QUrl xesam_BlockDevice;
    QUrl xesam_Bookmark;
    QUrl xesam_CommunicationChannel;
    QUrl xesam_Contact;
    QUrl xesam_ContactGroup;
    QUrl xesam_Content;
    QUrl xesam_DataObject;
    QUrl xesam_DeletedFile;
    QUrl xesam_Document;
    QUrl xesam_Documentation;
    QUrl xesam_Email;
    QUrl xesam_EmailAttachment;
    QUrl xesam_EmbeddedObject;
    QUrl xesam_File;
    QUrl xesam_FileSystem;
    QUrl xesam_Filelike;
    QUrl xesam_Folder;
    QUrl xesam_IMAP4Message;
    QUrl xesam_IMMessage;
    QUrl xesam_Image;
    QUrl xesam_MailingList;
    QUrl xesam_MailingListEmail;
    QUrl xesam_Media;
    QUrl xesam_MediaList;
    QUrl xesam_Message;
    QUrl xesam_MessageboxMessage;
    QUrl xesam_Music;
    QUrl xesam_NewsGroup;
    QUrl xesam_NewsGroupEmail;
    QUrl xesam_OfflineMedia;
    QUrl xesam_Organization;
    QUrl xesam_POP3Message;
    QUrl xesam_Person;
    QUrl xesam_PersonalEmail;
    QUrl xesam_Photo;
    QUrl xesam_Presentation;
    QUrl xesam_Project;
    QUrl xesam_RSSFeed;
    QUrl xesam_RSSMessage;
    QUrl xesam_RemoteFile;
    QUrl xesam_RemoteMessageboxMessage;
    QUrl xesam_RemoteResource;
    QUrl xesam_RevisionControlledFile;
    QUrl xesam_RevisionControlledRepository;
    QUrl xesam_SoftwarePackage;
    QUrl xesam_Source;
    QUrl xesam_SourceCode;
    QUrl xesam_Spreadsheet;
    QUrl xesam_SystemResource;
    QUrl xesam_Tag;
    QUrl xesam_Text;
    QUrl xesam_TextDocument;
    QUrl xesam_UncategorizedText;
    QUrl xesam_Video;
    QUrl xesam_Visual;
    QUrl xesam_XML;
    QUrl xesam_acl;
    QUrl xesam_album;
    QUrl xesam_albumArtist;
    QUrl xesam_albumGain;
    QUrl xesam_albumPeakGain;
    QUrl xesam_albumTrackCount;
    QUrl xesam_aperture;
    QUrl xesam_artist;
    QUrl xesam_asText;
    QUrl xesam_aspectRatio;
    QUrl xesam_attachmentEncoding;
    QUrl xesam_audioBPM;
    QUrl xesam_audioBitrate;
    QUrl xesam_audioChannels;
    QUrl xesam_audioCodec;
    QUrl xesam_audioCodecType;
    QUrl xesam_audioSampleBitDepth;
    QUrl xesam_audioSampleCount;
    QUrl xesam_audioSampleDataType;
    QUrl xesam_audioSampleRate;
    QUrl xesam_author;
    QUrl xesam_autoRating;
    QUrl xesam_baseRevisionID;
    QUrl xesam_bcc;
    QUrl xesam_birthDate;
    QUrl xesam_blogContactURL;
    QUrl xesam_cameraManufacturer;
    QUrl xesam_cameraModel;
    QUrl xesam_cc;
    QUrl xesam_ccdWidth;
    QUrl xesam_cellPhoneNumber;
    QUrl xesam_changeCommitTime;
    QUrl xesam_changeCommitter;
    QUrl xesam_characterCount;
    QUrl xesam_charset;
    QUrl xesam_chatRoom;
    QUrl xesam_colorCount;
    QUrl xesam_colorSpace;
    QUrl xesam_columnCount;
    QUrl xesam_comment;
    QUrl xesam_commentCharacterCount;
    QUrl xesam_commitDiff;
    QUrl xesam_communicationChannel;
    QUrl xesam_composer;
    QUrl xesam_compressionAlgorithm;
    QUrl xesam_compressionLevel;
    QUrl xesam_conflicts;
    QUrl xesam_contactMedium;
    QUrl xesam_contactNick;
    QUrl xesam_contactURL;
    QUrl xesam_contains;
    QUrl xesam_contentComment;
    QUrl xesam_contentCreated;
    QUrl xesam_contentKeyword;
    QUrl xesam_contentModified;
    QUrl xesam_contentType;
    QUrl xesam_contributor;
    QUrl xesam_copyright;
    QUrl xesam_creator;
    QUrl xesam_definesClass;
    QUrl xesam_definesFunction;
    QUrl xesam_definesGlobalVariable;
    QUrl xesam_deletionTime;
    QUrl xesam_depends;
    QUrl xesam_derivedFrom;
    QUrl xesam_description;
    QUrl xesam_discNumber;
    QUrl xesam_disclaimer;
    QUrl xesam_documentCategory;
    QUrl xesam_emailAddress;
    QUrl xesam_exposureBias;
    QUrl xesam_exposureProgram;
    QUrl xesam_exposureTime;
    QUrl xesam_familyName;
    QUrl xesam_faxPhoneNumber;
    QUrl xesam_fileExtension;
    QUrl xesam_fileSystemType;
    QUrl xesam_fingerprint;
    QUrl xesam_firstUsed;
    QUrl xesam_flashUsed;
    QUrl xesam_focalLength;
    QUrl xesam_focusDistance;
    QUrl xesam_formatSubtype;
    QUrl xesam_frameCount;
    QUrl xesam_frameRate;
    QUrl xesam_freeSpace;
    QUrl xesam_gender;
    QUrl xesam_generator;
    QUrl xesam_generatorOptions;
    QUrl xesam_genre;
    QUrl xesam_givenName;
    QUrl xesam_group;
    QUrl xesam_height;
    QUrl xesam_homeEmailAddress;
    QUrl xesam_homePhoneNumber;
    QUrl xesam_homePostalAddress;
    QUrl xesam_homepageContactURL;
    QUrl xesam_honorificPrefix;
    QUrl xesam_honorificSuffix;
    QUrl xesam_horizontalResolution;
    QUrl xesam_id;
    QUrl xesam_imContactMedium;
    QUrl xesam_inReplyTo;
    QUrl xesam_interest;
    QUrl xesam_interlaceMode;
    QUrl xesam_ircContactMedium;
    QUrl xesam_isContentEncrypted;
    QUrl xesam_isEncrypted;
    QUrl xesam_isImportant;
    QUrl xesam_isInProgress;
    QUrl xesam_isPublicChannel;
    QUrl xesam_isRead;
    QUrl xesam_isSourceEncrypted;
    QUrl xesam_isoEquivalent;
    QUrl xesam_jabberContactMedium;
    QUrl xesam_keyword;
    QUrl xesam_knows;
    QUrl xesam_language;
    QUrl xesam_lastRefreshed;
    QUrl xesam_lastUsed;
    QUrl xesam_legal;
    QUrl xesam_license;
    QUrl xesam_licenseType;
    QUrl xesam_lineCount;
    QUrl xesam_links;
    QUrl xesam_localRevision;
    QUrl xesam_lyricist;
    QUrl xesam_mailingList;
    QUrl xesam_mailingPostalAddress;
    QUrl xesam_maintainer;
    QUrl xesam_markupCharacterCount;
    QUrl xesam_md5Hash;
    QUrl xesam_mediaBitrate;
    QUrl xesam_mediaCodec;
    QUrl xesam_mediaCodecType;
    QUrl xesam_mediaDuration;
    QUrl xesam_mergeConflict;
    QUrl xesam_meteringMode;
    QUrl xesam_mimeType;
    QUrl xesam_mountPoint;
    QUrl xesam_name;
    QUrl xesam_newsGroup;
    QUrl xesam_occupiedSpace;
    QUrl xesam_orientation;
    QUrl xesam_originURL;
    QUrl xesam_originalLocation;
    QUrl xesam_otherName;
    QUrl xesam_owner;
    QUrl xesam_pageCount;
    QUrl xesam_paragraphCount;
    QUrl xesam_performer;
    QUrl xesam_permissions;
    QUrl xesam_personPhoto;
    QUrl xesam_phoneNumber;
    QUrl xesam_pixelDataBitDepth;
    QUrl xesam_pixelDataType;
    QUrl xesam_postalAddress;
    QUrl xesam_primaryRecipient;
    QUrl xesam_programmingLanguage;
    QUrl xesam_receptionTime;
    QUrl xesam_recipient;
    QUrl xesam_related;
    QUrl xesam_remotePassword;
    QUrl xesam_remotePort;
    QUrl xesam_remoteServer;
    QUrl xesam_remoteUser;
    QUrl xesam_replyTo;
    QUrl xesam_rowCount;
    QUrl xesam_rssFeed;
    QUrl xesam_sampleConfiguration;
    QUrl xesam_sampleDataBitDepth;
    QUrl xesam_sampleDataType;
    QUrl xesam_secondaryRecipient;
    QUrl xesam_seenAttachedAs;
    QUrl xesam_setCount;
    QUrl xesam_setRate;
    QUrl xesam_sha1Hash;
    QUrl xesam_size;
    QUrl xesam_sourceCreated;
    QUrl xesam_sourceModified;
    QUrl xesam_storageSize;
    QUrl xesam_subject;
    QUrl xesam_summary;
    QUrl xesam_supercedes;
    QUrl xesam_targetQuality;
    QUrl xesam_title;
    QUrl xesam_to;
    QUrl xesam_totalSpace;
    QUrl xesam_totalUncompressedSize;
    QUrl xesam_trackGain;
    QUrl xesam_trackNumber;
    QUrl xesam_trackPeakGain;
    QUrl xesam_url;
    QUrl xesam_useCount;
    QUrl xesam_userComment;
    QUrl xesam_userKeyword;
    QUrl xesam_userRating;
    QUrl xesam_usesNamespace;
    QUrl xesam_version;
    QUrl xesam_verticalResolution;
    QUrl xesam_videoBitrate;
    QUrl xesam_videoCodec;
    QUrl xesam_videoCodecType;
    QUrl xesam_whiteBalance;
    QUrl xesam_width;
    QUrl xesam_wordCount;
    QUrl xesam_workEmailAddress;
    QUrl xesam_workPhoneNumber;
    QUrl xesam_workPostalAddress;
};

Q_GLOBAL_STATIC( XesamPrivate, s_xesam )

QUrl Soprano::Vocabulary::Xesam::xesamNamespace()
{
    return s_xesam()->xesam_namespace;
}

QUrl Soprano::Vocabulary::Xesam::xesam35mmEquivalent()
{
    return s_xesam()->xesam_35mmEquivalent;
}

QUrl Soprano::Vocabulary::Xesam::Annotation()
{
    return s_xesam()->xesam_Annotation;
}

QUrl Soprano::Vocabulary::Xesam::Archive()
{
    return s_xesam()->xesam_Archive;
}

QUrl Soprano::Vocabulary::Xesam::ArchivedFile()
{
    return s_xesam()->xesam_ArchivedFile;
}

QUrl Soprano::Vocabulary::Xesam::Audio()
{
    return s_xesam()->xesam_Audio;
}

QUrl Soprano::Vocabulary::Xesam::AudioList()
{
    return s_xesam()->xesam_AudioList;
}

QUrl Soprano::Vocabulary::Xesam::BlockDevice()
{
    return s_xesam()->xesam_BlockDevice;
}

QUrl Soprano::Vocabulary::Xesam::Bookmark()
{
    return s_xesam()->xesam_Bookmark;
}

QUrl Soprano::Vocabulary::Xesam::CommunicationChannel()
{
    return s_xesam()->xesam_CommunicationChannel;
}

QUrl Soprano::Vocabulary::Xesam::Contact()
{
    return s_xesam()->xesam_Contact;
}

QUrl Soprano::Vocabulary::Xesam::ContactGroup()
{
    return s_xesam()->xesam_ContactGroup;
}

QUrl Soprano::Vocabulary::Xesam::Content()
{
    return s_xesam()->xesam_Content;
}

QUrl Soprano::Vocabulary::Xesam::DataObject()
{
    return s_xesam()->xesam_DataObject;
}

QUrl Soprano::Vocabulary::Xesam::DeletedFile()
{
    return s_xesam()->xesam_DeletedFile;
}

QUrl Soprano::Vocabulary::Xesam::Document()
{
    return s_xesam()->xesam_Document;
}

QUrl Soprano::Vocabulary::Xesam::Documentation()
{
    return s_xesam()->xesam_Documentation;
}

QUrl Soprano::Vocabulary::Xesam::Email()
{
    return s_xesam()->xesam_Email;
}

QUrl Soprano::Vocabulary::Xesam::EmailAttachment()
{
    return s_xesam()->xesam_EmailAttachment;
}

QUrl Soprano::Vocabulary::Xesam::EmbeddedObject()
{
    return s_xesam()->xesam_EmbeddedObject;
}

QUrl Soprano::Vocabulary::Xesam::File()
{
    return s_xesam()->xesam_File;
}

QUrl Soprano::Vocabulary::Xesam::FileSystem()
{
    return s_xesam()->xesam_FileSystem;
}

QUrl Soprano::Vocabulary::Xesam::Filelike()
{
    return s_xesam()->xesam_Filelike;
}

QUrl Soprano::Vocabulary::Xesam::Folder()
{
    return s_xesam()->xesam_Folder;
}

QUrl Soprano::Vocabulary::Xesam::IMAP4Message()
{
    return s_xesam()->xesam_IMAP4Message;
}

QUrl Soprano::Vocabulary::Xesam::IMMessage()
{
    return s_xesam()->xesam_IMMessage;
}

QUrl Soprano::Vocabulary::Xesam::Image()
{
    return s_xesam()->xesam_Image;
}

QUrl Soprano::Vocabulary::Xesam::MailingList()
{
    return s_xesam()->xesam_MailingList;
}

QUrl Soprano::Vocabulary::Xesam::MailingListEmail()
{
    return s_xesam()->xesam_MailingListEmail;
}

QUrl Soprano::Vocabulary::Xesam::Media()
{
    return s_xesam()->xesam_Media;
}

QUrl Soprano::Vocabulary::Xesam::MediaList()
{
    return s_xesam()->xesam_MediaList;
}

QUrl Soprano::Vocabulary::Xesam::Message()
{
    return s_xesam()->xesam_Message;
}

QUrl Soprano::Vocabulary::Xesam::MessageboxMessage()
{
    return s_xesam()->xesam_MessageboxMessage;
}

QUrl Soprano::Vocabulary::Xesam::Music()
{
    return s_xesam()->xesam_Music;
}

QUrl Soprano::Vocabulary::Xesam::NewsGroup()
{
    return s_xesam()->xesam_NewsGroup;
}

QUrl Soprano::Vocabulary::Xesam::NewsGroupEmail()
{
    return s_xesam()->xesam_NewsGroupEmail;
}

QUrl Soprano::Vocabulary::Xesam::OfflineMedia()
{
    return s_xesam()->xesam_OfflineMedia;
}

QUrl Soprano::Vocabulary::Xesam::Organization()
{
    return s_xesam()->xesam_Organization;
}

QUrl Soprano::Vocabulary::Xesam::POP3Message()
{
    return s_xesam()->xesam_POP3Message;
}

QUrl Soprano::Vocabulary::Xesam::Person()
{
    return s_xesam()->xesam_Person;
}

QUrl Soprano::Vocabulary::Xesam::PersonalEmail()
{
    return s_xesam()->xesam_PersonalEmail;
}

QUrl Soprano::Vocabulary::Xesam::Photo()
{
    return s_xesam()->xesam_Photo;
}

QUrl Soprano::Vocabulary::Xesam::Presentation()
{
    return s_xesam()->xesam_Presentation;
}

QUrl Soprano::Vocabulary::Xesam::Project()
{
    return s_xesam()->xesam_Project;
}

QUrl Soprano::Vocabulary::Xesam::RSSFeed()
{
    return s_xesam()->xesam_RSSFeed;
}

QUrl Soprano::Vocabulary::Xesam::RSSMessage()
{
    return s_xesam()->xesam_RSSMessage;
}

QUrl Soprano::Vocabulary::Xesam::RemoteFile()
{
    return s_xesam()->xesam_RemoteFile;
}

QUrl Soprano::Vocabulary::Xesam::RemoteMessageboxMessage()
{
    return s_xesam()->xesam_RemoteMessageboxMessage;
}

QUrl Soprano::Vocabulary::Xesam::RemoteResource()
{
    return s_xesam()->xesam_RemoteResource;
}

QUrl Soprano::Vocabulary::Xesam::RevisionControlledFile()
{
    return s_xesam()->xesam_RevisionControlledFile;
}

QUrl Soprano::Vocabulary::Xesam::RevisionControlledRepository()
{
    return s_xesam()->xesam_RevisionControlledRepository;
}

QUrl Soprano::Vocabulary::Xesam::SoftwarePackage()
{
    return s_xesam()->xesam_SoftwarePackage;
}

QUrl Soprano::Vocabulary::Xesam::Source()
{
    return s_xesam()->xesam_Source;
}

QUrl Soprano::Vocabulary::Xesam::SourceCode()
{
    return s_xesam()->xesam_SourceCode;
}

QUrl Soprano::Vocabulary::Xesam::Spreadsheet()
{
    return s_xesam()->xesam_Spreadsheet;
}

QUrl Soprano::Vocabulary::Xesam::SystemResource()
{
    return s_xesam()->xesam_SystemResource;
}

QUrl Soprano::Vocabulary::Xesam::Tag()
{
    return s_xesam()->xesam_Tag;
}

QUrl Soprano::Vocabulary::Xesam::Text()
{
    return s_xesam()->xesam_Text;
}

QUrl Soprano::Vocabulary::Xesam::TextDocument()
{
    return s_xesam()->xesam_TextDocument;
}

QUrl Soprano::Vocabulary::Xesam::UncategorizedText()
{
    return s_xesam()->xesam_UncategorizedText;
}

QUrl Soprano::Vocabulary::Xesam::Video()
{
    return s_xesam()->xesam_Video;
}

QUrl Soprano::Vocabulary::Xesam::Visual()
{
    return s_xesam()->xesam_Visual;
}

QUrl Soprano::Vocabulary::Xesam::XML()
{
    return s_xesam()->xesam_XML;
}

QUrl Soprano::Vocabulary::Xesam::acl()
{
    return s_xesam()->xesam_acl;
}

QUrl Soprano::Vocabulary::Xesam::album()
{
    return s_xesam()->xesam_album;
}

QUrl Soprano::Vocabulary::Xesam::albumArtist()
{
    return s_xesam()->xesam_albumArtist;
}

QUrl Soprano::Vocabulary::Xesam::albumGain()
{
    return s_xesam()->xesam_albumGain;
}

QUrl Soprano::Vocabulary::Xesam::albumPeakGain()
{
    return s_xesam()->xesam_albumPeakGain;
}

QUrl Soprano::Vocabulary::Xesam::albumTrackCount()
{
    return s_xesam()->xesam_albumTrackCount;
}

QUrl Soprano::Vocabulary::Xesam::aperture()
{
    return s_xesam()->xesam_aperture;
}

QUrl Soprano::Vocabulary::Xesam::artist()
{
    return s_xesam()->xesam_artist;
}

QUrl Soprano::Vocabulary::Xesam::asText()
{
    return s_xesam()->xesam_asText;
}

QUrl Soprano::Vocabulary::Xesam::aspectRatio()
{
    return s_xesam()->xesam_aspectRatio;
}

QUrl Soprano::Vocabulary::Xesam::attachmentEncoding()
{
    return s_xesam()->xesam_attachmentEncoding;
}

QUrl Soprano::Vocabulary::Xesam::audioBPM()
{
    return s_xesam()->xesam_audioBPM;
}

QUrl Soprano::Vocabulary::Xesam::audioBitrate()
{
    return s_xesam()->xesam_audioBitrate;
}

QUrl Soprano::Vocabulary::Xesam::audioChannels()
{
    return s_xesam()->xesam_audioChannels;
}

QUrl Soprano::Vocabulary::Xesam::audioCodec()
{
    return s_xesam()->xesam_audioCodec;
}

QUrl Soprano::Vocabulary::Xesam::audioCodecType()
{
    return s_xesam()->xesam_audioCodecType;
}

QUrl Soprano::Vocabulary::Xesam::audioSampleBitDepth()
{
    return s_xesam()->xesam_audioSampleBitDepth;
}

QUrl Soprano::Vocabulary::Xesam::audioSampleCount()
{
    return s_xesam()->xesam_audioSampleCount;
}

QUrl Soprano::Vocabulary::Xesam::audioSampleDataType()
{
    return s_xesam()->xesam_audioSampleDataType;
}

QUrl Soprano::Vocabulary::Xesam::audioSampleRate()
{
    return s_xesam()->xesam_audioSampleRate;
}

QUrl Soprano::Vocabulary::Xesam::author()
{
    return s_xesam()->xesam_author;
}

QUrl Soprano::Vocabulary::Xesam::autoRating()
{
    return s_xesam()->xesam_autoRating;
}

QUrl Soprano::Vocabulary::Xesam::baseRevisionID()
{
    return s_xesam()->xesam_baseRevisionID;
}

QUrl Soprano::Vocabulary::Xesam::bcc()
{
    return s_xesam()->xesam_bcc;
}

QUrl Soprano::Vocabulary::Xesam::birthDate()
{
    return s_xesam()->xesam_birthDate;
}

QUrl Soprano::Vocabulary::Xesam::blogContactURL()
{
    return s_xesam()->xesam_blogContactURL;
}

QUrl Soprano::Vocabulary::Xesam::cameraManufacturer()
{
    return s_xesam()->xesam_cameraManufacturer;
}

QUrl Soprano::Vocabulary::Xesam::cameraModel()
{
    return s_xesam()->xesam_cameraModel;
}

QUrl Soprano::Vocabulary::Xesam::cc()
{
    return s_xesam()->xesam_cc;
}

QUrl Soprano::Vocabulary::Xesam::ccdWidth()
{
    return s_xesam()->xesam_ccdWidth;
}

QUrl Soprano::Vocabulary::Xesam::cellPhoneNumber()
{
    return s_xesam()->xesam_cellPhoneNumber;
}

QUrl Soprano::Vocabulary::Xesam::changeCommitTime()
{
    return s_xesam()->xesam_changeCommitTime;
}

QUrl Soprano::Vocabulary::Xesam::changeCommitter()
{
    return s_xesam()->xesam_changeCommitter;
}

QUrl Soprano::Vocabulary::Xesam::characterCount()
{
    return s_xesam()->xesam_characterCount;
}

QUrl Soprano::Vocabulary::Xesam::charset()
{
    return s_xesam()->xesam_charset;
}

QUrl Soprano::Vocabulary::Xesam::chatRoom()
{
    return s_xesam()->xesam_chatRoom;
}

QUrl Soprano::Vocabulary::Xesam::colorCount()
{
    return s_xesam()->xesam_colorCount;
}

QUrl Soprano::Vocabulary::Xesam::colorSpace()
{
    return s_xesam()->xesam_colorSpace;
}

QUrl Soprano::Vocabulary::Xesam::columnCount()
{
    return s_xesam()->xesam_columnCount;
}

QUrl Soprano::Vocabulary::Xesam::comment()
{
    return s_xesam()->xesam_comment;
}

QUrl Soprano::Vocabulary::Xesam::commentCharacterCount()
{
    return s_xesam()->xesam_commentCharacterCount;
}

QUrl Soprano::Vocabulary::Xesam::commitDiff()
{
    return s_xesam()->xesam_commitDiff;
}

QUrl Soprano::Vocabulary::Xesam::communicationChannel()
{
    return s_xesam()->xesam_communicationChannel;
}

QUrl Soprano::Vocabulary::Xesam::composer()
{
    return s_xesam()->xesam_composer;
}

QUrl Soprano::Vocabulary::Xesam::compressionAlgorithm()
{
    return s_xesam()->xesam_compressionAlgorithm;
}

QUrl Soprano::Vocabulary::Xesam::compressionLevel()
{
    return s_xesam()->xesam_compressionLevel;
}

QUrl Soprano::Vocabulary::Xesam::conflicts()
{
    return s_xesam()->xesam_conflicts;
}

QUrl Soprano::Vocabulary::Xesam::contactMedium()
{
    return s_xesam()->xesam_contactMedium;
}

QUrl Soprano::Vocabulary::Xesam::contactNick()
{
    return s_xesam()->xesam_contactNick;
}

QUrl Soprano::Vocabulary::Xesam::contactURL()
{
    return s_xesam()->xesam_contactURL;
}

QUrl Soprano::Vocabulary::Xesam::contains()
{
    return s_xesam()->xesam_contains;
}

QUrl Soprano::Vocabulary::Xesam::contentComment()
{
    return s_xesam()->xesam_contentComment;
}

QUrl Soprano::Vocabulary::Xesam::contentCreated()
{
    return s_xesam()->xesam_contentCreated;
}

QUrl Soprano::Vocabulary::Xesam::contentKeyword()
{
    return s_xesam()->xesam_contentKeyword;
}

QUrl Soprano::Vocabulary::Xesam::contentModified()
{
    return s_xesam()->xesam_contentModified;
}

QUrl Soprano::Vocabulary::Xesam::contentType()
{
    return s_xesam()->xesam_contentType;
}

QUrl Soprano::Vocabulary::Xesam::contributor()
{
    return s_xesam()->xesam_contributor;
}

QUrl Soprano::Vocabulary::Xesam::copyright()
{
    return s_xesam()->xesam_copyright;
}

QUrl Soprano::Vocabulary::Xesam::creator()
{
    return s_xesam()->xesam_creator;
}

QUrl Soprano::Vocabulary::Xesam::definesClass()
{
    return s_xesam()->xesam_definesClass;
}

QUrl Soprano::Vocabulary::Xesam::definesFunction()
{
    return s_xesam()->xesam_definesFunction;
}

QUrl Soprano::Vocabulary::Xesam::definesGlobalVariable()
{
    return s_xesam()->xesam_definesGlobalVariable;
}

QUrl Soprano::Vocabulary::Xesam::deletionTime()
{
    return s_xesam()->xesam_deletionTime;
}

QUrl Soprano::Vocabulary::Xesam::depends()
{
    return s_xesam()->xesam_depends;
}

QUrl Soprano::Vocabulary::Xesam::derivedFrom()
{
    return s_xesam()->xesam_derivedFrom;
}

QUrl Soprano::Vocabulary::Xesam::description()
{
    return s_xesam()->xesam_description;
}

QUrl Soprano::Vocabulary::Xesam::discNumber()
{
    return s_xesam()->xesam_discNumber;
}

QUrl Soprano::Vocabulary::Xesam::disclaimer()
{
    return s_xesam()->xesam_disclaimer;
}

QUrl Soprano::Vocabulary::Xesam::documentCategory()
{
    return s_xesam()->xesam_documentCategory;
}

QUrl Soprano::Vocabulary::Xesam::emailAddress()
{
    return s_xesam()->xesam_emailAddress;
}

QUrl Soprano::Vocabulary::Xesam::exposureBias()
{
    return s_xesam()->xesam_exposureBias;
}

QUrl Soprano::Vocabulary::Xesam::exposureProgram()
{
    return s_xesam()->xesam_exposureProgram;
}

QUrl Soprano::Vocabulary::Xesam::exposureTime()
{
    return s_xesam()->xesam_exposureTime;
}

QUrl Soprano::Vocabulary::Xesam::familyName()
{
    return s_xesam()->xesam_familyName;
}

QUrl Soprano::Vocabulary::Xesam::faxPhoneNumber()
{
    return s_xesam()->xesam_faxPhoneNumber;
}

QUrl Soprano::Vocabulary::Xesam::fileExtension()
{
    return s_xesam()->xesam_fileExtension;
}

QUrl Soprano::Vocabulary::Xesam::fileSystemType()
{
    return s_xesam()->xesam_fileSystemType;
}

QUrl Soprano::Vocabulary::Xesam::fingerprint()
{
    return s_xesam()->xesam_fingerprint;
}

QUrl Soprano::Vocabulary::Xesam::firstUsed()
{
    return s_xesam()->xesam_firstUsed;
}

QUrl Soprano::Vocabulary::Xesam::flashUsed()
{
    return s_xesam()->xesam_flashUsed;
}

QUrl Soprano::Vocabulary::Xesam::focalLength()
{
    return s_xesam()->xesam_focalLength;
}

QUrl Soprano::Vocabulary::Xesam::focusDistance()
{
    return s_xesam()->xesam_focusDistance;
}

QUrl Soprano::Vocabulary::Xesam::formatSubtype()
{
    return s_xesam()->xesam_formatSubtype;
}

QUrl Soprano::Vocabulary::Xesam::frameCount()
{
    return s_xesam()->xesam_frameCount;
}

QUrl Soprano::Vocabulary::Xesam::frameRate()
{
    return s_xesam()->xesam_frameRate;
}

QUrl Soprano::Vocabulary::Xesam::freeSpace()
{
    return s_xesam()->xesam_freeSpace;
}

QUrl Soprano::Vocabulary::Xesam::gender()
{
    return s_xesam()->xesam_gender;
}

QUrl Soprano::Vocabulary::Xesam::generator()
{
    return s_xesam()->xesam_generator;
}

QUrl Soprano::Vocabulary::Xesam::generatorOptions()
{
    return s_xesam()->xesam_generatorOptions;
}

QUrl Soprano::Vocabulary::Xesam::genre()
{
    return s_xesam()->xesam_genre;
}

QUrl Soprano::Vocabulary::Xesam::givenName()
{
    return s_xesam()->xesam_givenName;
}

QUrl Soprano::Vocabulary::Xesam::group()
{
    return s_xesam()->xesam_group;
}

QUrl Soprano::Vocabulary::Xesam::height()
{
    return s_xesam()->xesam_height;
}

QUrl Soprano::Vocabulary::Xesam::homeEmailAddress()
{
    return s_xesam()->xesam_homeEmailAddress;
}

QUrl Soprano::Vocabulary::Xesam::homePhoneNumber()
{
    return s_xesam()->xesam_homePhoneNumber;
}

QUrl Soprano::Vocabulary::Xesam::homePostalAddress()
{
    return s_xesam()->xesam_homePostalAddress;
}

QUrl Soprano::Vocabulary::Xesam::homepageContactURL()
{
    return s_xesam()->xesam_homepageContactURL;
}

QUrl Soprano::Vocabulary::Xesam::honorificPrefix()
{
    return s_xesam()->xesam_honorificPrefix;
}

QUrl Soprano::Vocabulary::Xesam::honorificSuffix()
{
    return s_xesam()->xesam_honorificSuffix;
}

QUrl Soprano::Vocabulary::Xesam::horizontalResolution()
{
    return s_xesam()->xesam_horizontalResolution;
}

QUrl Soprano::Vocabulary::Xesam::id()
{
    return s_xesam()->xesam_id;
}

QUrl Soprano::Vocabulary::Xesam::imContactMedium()
{
    return s_xesam()->xesam_imContactMedium;
}

QUrl Soprano::Vocabulary::Xesam::inReplyTo()
{
    return s_xesam()->xesam_inReplyTo;
}

QUrl Soprano::Vocabulary::Xesam::interest()
{
    return s_xesam()->xesam_interest;
}

QUrl Soprano::Vocabulary::Xesam::interlaceMode()
{
    return s_xesam()->xesam_interlaceMode;
}

QUrl Soprano::Vocabulary::Xesam::ircContactMedium()
{
    return s_xesam()->xesam_ircContactMedium;
}

QUrl Soprano::Vocabulary::Xesam::isContentEncrypted()
{
    return s_xesam()->xesam_isContentEncrypted;
}

QUrl Soprano::Vocabulary::Xesam::isEncrypted()
{
    return s_xesam()->xesam_isEncrypted;
}

QUrl Soprano::Vocabulary::Xesam::isImportant()
{
    return s_xesam()->xesam_isImportant;
}

QUrl Soprano::Vocabulary::Xesam::isInProgress()
{
    return s_xesam()->xesam_isInProgress;
}

QUrl Soprano::Vocabulary::Xesam::isPublicChannel()
{
    return s_xesam()->xesam_isPublicChannel;
}

QUrl Soprano::Vocabulary::Xesam::isRead()
{
    return s_xesam()->xesam_isRead;
}

QUrl Soprano::Vocabulary::Xesam::isSourceEncrypted()
{
    return s_xesam()->xesam_isSourceEncrypted;
}

QUrl Soprano::Vocabulary::Xesam::isoEquivalent()
{
    return s_xesam()->xesam_isoEquivalent;
}

QUrl Soprano::Vocabulary::Xesam::jabberContactMedium()
{
    return s_xesam()->xesam_jabberContactMedium;
}

QUrl Soprano::Vocabulary::Xesam::keyword()
{
    return s_xesam()->xesam_keyword;
}

QUrl Soprano::Vocabulary::Xesam::knows()
{
    return s_xesam()->xesam_knows;
}

QUrl Soprano::Vocabulary::Xesam::language()
{
    return s_xesam()->xesam_language;
}

QUrl Soprano::Vocabulary::Xesam::lastRefreshed()
{
    return s_xesam()->xesam_lastRefreshed;
}

QUrl Soprano::Vocabulary::Xesam::lastUsed()
{
    return s_xesam()->xesam_lastUsed;
}

QUrl Soprano::Vocabulary::Xesam::legal()
{
    return s_xesam()->xesam_legal;
}

QUrl Soprano::Vocabulary::Xesam::license()
{
    return s_xesam()->xesam_license;
}

QUrl Soprano::Vocabulary::Xesam::licenseType()
{
    return s_xesam()->xesam_licenseType;
}

QUrl Soprano::Vocabulary::Xesam::lineCount()
{
    return s_xesam()->xesam_lineCount;
}

QUrl Soprano::Vocabulary::Xesam::links()
{
    return s_xesam()->xesam_links;
}

QUrl Soprano::Vocabulary::Xesam::localRevision()
{
    return s_xesam()->xesam_localRevision;
}

QUrl Soprano::Vocabulary::Xesam::lyricist()
{
    return s_xesam()->xesam_lyricist;
}

QUrl Soprano::Vocabulary::Xesam::mailingList()
{
    return s_xesam()->xesam_mailingList;
}

QUrl Soprano::Vocabulary::Xesam::mailingPostalAddress()
{
    return s_xesam()->xesam_mailingPostalAddress;
}

QUrl Soprano::Vocabulary::Xesam::maintainer()
{
    return s_xesam()->xesam_maintainer;
}

QUrl Soprano::Vocabulary::Xesam::markupCharacterCount()
{
    return s_xesam()->xesam_markupCharacterCount;
}

QUrl Soprano::Vocabulary::Xesam::md5Hash()
{
    return s_xesam()->xesam_md5Hash;
}

QUrl Soprano::Vocabulary::Xesam::mediaBitrate()
{
    return s_xesam()->xesam_mediaBitrate;
}

QUrl Soprano::Vocabulary::Xesam::mediaCodec()
{
    return s_xesam()->xesam_mediaCodec;
}

QUrl Soprano::Vocabulary::Xesam::mediaCodecType()
{
    return s_xesam()->xesam_mediaCodecType;
}

QUrl Soprano::Vocabulary::Xesam::mediaDuration()
{
    return s_xesam()->xesam_mediaDuration;
}

QUrl Soprano::Vocabulary::Xesam::mergeConflict()
{
    return s_xesam()->xesam_mergeConflict;
}

QUrl Soprano::Vocabulary::Xesam::meteringMode()
{
    return s_xesam()->xesam_meteringMode;
}

QUrl Soprano::Vocabulary::Xesam::mimeType()
{
    return s_xesam()->xesam_mimeType;
}

QUrl Soprano::Vocabulary::Xesam::mountPoint()
{
    return s_xesam()->xesam_mountPoint;
}

QUrl Soprano::Vocabulary::Xesam::name()
{
    return s_xesam()->xesam_name;
}

QUrl Soprano::Vocabulary::Xesam::newsGroup()
{
    return s_xesam()->xesam_newsGroup;
}

QUrl Soprano::Vocabulary::Xesam::occupiedSpace()
{
    return s_xesam()->xesam_occupiedSpace;
}

QUrl Soprano::Vocabulary::Xesam::orientation()
{
    return s_xesam()->xesam_orientation;
}

QUrl Soprano::Vocabulary::Xesam::originURL()
{
    return s_xesam()->xesam_originURL;
}

QUrl Soprano::Vocabulary::Xesam::originalLocation()
{
    return s_xesam()->xesam_originalLocation;
}

QUrl Soprano::Vocabulary::Xesam::otherName()
{
    return s_xesam()->xesam_otherName;
}

QUrl Soprano::Vocabulary::Xesam::owner()
{
    return s_xesam()->xesam_owner;
}

QUrl Soprano::Vocabulary::Xesam::pageCount()
{
    return s_xesam()->xesam_pageCount;
}

QUrl Soprano::Vocabulary::Xesam::paragraphCount()
{
    return s_xesam()->xesam_paragraphCount;
}

QUrl Soprano::Vocabulary::Xesam::performer()
{
    return s_xesam()->xesam_performer;
}

QUrl Soprano::Vocabulary::Xesam::permissions()
{
    return s_xesam()->xesam_permissions;
}

QUrl Soprano::Vocabulary::Xesam::personPhoto()
{
    return s_xesam()->xesam_personPhoto;
}

QUrl Soprano::Vocabulary::Xesam::phoneNumber()
{
    return s_xesam()->xesam_phoneNumber;
}

QUrl Soprano::Vocabulary::Xesam::pixelDataBitDepth()
{
    return s_xesam()->xesam_pixelDataBitDepth;
}

QUrl Soprano::Vocabulary::Xesam::pixelDataType()
{
    return s_xesam()->xesam_pixelDataType;
}

QUrl Soprano::Vocabulary::Xesam::postalAddress()
{
    return s_xesam()->xesam_postalAddress;
}

QUrl Soprano::Vocabulary::Xesam::primaryRecipient()
{
    return s_xesam()->xesam_primaryRecipient;
}

QUrl Soprano::Vocabulary::Xesam::programmingLanguage()
{
    return s_xesam()->xesam_programmingLanguage;
}

QUrl Soprano::Vocabulary::Xesam::receptionTime()
{
    return s_xesam()->xesam_receptionTime;
}

QUrl Soprano::Vocabulary::Xesam::recipient()
{
    return s_xesam()->xesam_recipient;
}

QUrl Soprano::Vocabulary::Xesam::related()
{
    return s_xesam()->xesam_related;
}

QUrl Soprano::Vocabulary::Xesam::remotePassword()
{
    return s_xesam()->xesam_remotePassword;
}

QUrl Soprano::Vocabulary::Xesam::remotePort()
{
    return s_xesam()->xesam_remotePort;
}

QUrl Soprano::Vocabulary::Xesam::remoteServer()
{
    return s_xesam()->xesam_remoteServer;
}

QUrl Soprano::Vocabulary::Xesam::remoteUser()
{
    return s_xesam()->xesam_remoteUser;
}

QUrl Soprano::Vocabulary::Xesam::replyTo()
{
    return s_xesam()->xesam_replyTo;
}

QUrl Soprano::Vocabulary::Xesam::rowCount()
{
    return s_xesam()->xesam_rowCount;
}

QUrl Soprano::Vocabulary::Xesam::rssFeed()
{
    return s_xesam()->xesam_rssFeed;
}

QUrl Soprano::Vocabulary::Xesam::sampleConfiguration()
{
    return s_xesam()->xesam_sampleConfiguration;
}

QUrl Soprano::Vocabulary::Xesam::sampleDataBitDepth()
{
    return s_xesam()->xesam_sampleDataBitDepth;
}

QUrl Soprano::Vocabulary::Xesam::sampleDataType()
{
    return s_xesam()->xesam_sampleDataType;
}

QUrl Soprano::Vocabulary::Xesam::secondaryRecipient()
{
    return s_xesam()->xesam_secondaryRecipient;
}

QUrl Soprano::Vocabulary::Xesam::seenAttachedAs()
{
    return s_xesam()->xesam_seenAttachedAs;
}

QUrl Soprano::Vocabulary::Xesam::setCount()
{
    return s_xesam()->xesam_setCount;
}

QUrl Soprano::Vocabulary::Xesam::setRate()
{
    return s_xesam()->xesam_setRate;
}

QUrl Soprano::Vocabulary::Xesam::sha1Hash()
{
    return s_xesam()->xesam_sha1Hash;
}

QUrl Soprano::Vocabulary::Xesam::size()
{
    return s_xesam()->xesam_size;
}

QUrl Soprano::Vocabulary::Xesam::sourceCreated()
{
    return s_xesam()->xesam_sourceCreated;
}

QUrl Soprano::Vocabulary::Xesam::sourceModified()
{
    return s_xesam()->xesam_sourceModified;
}

QUrl Soprano::Vocabulary::Xesam::storageSize()
{
    return s_xesam()->xesam_storageSize;
}

QUrl Soprano::Vocabulary::Xesam::subject()
{
    return s_xesam()->xesam_subject;
}

QUrl Soprano::Vocabulary::Xesam::summary()
{
    return s_xesam()->xesam_summary;
}

QUrl Soprano::Vocabulary::Xesam::supercedes()
{
    return s_xesam()->xesam_supercedes;
}

QUrl Soprano::Vocabulary::Xesam::targetQuality()
{
    return s_xesam()->xesam_targetQuality;
}

QUrl Soprano::Vocabulary::Xesam::title()
{
    return s_xesam()->xesam_title;
}

QUrl Soprano::Vocabulary::Xesam::to()
{
    return s_xesam()->xesam_to;
}

QUrl Soprano::Vocabulary::Xesam::totalSpace()
{
    return s_xesam()->xesam_totalSpace;
}

QUrl Soprano::Vocabulary::Xesam::totalUncompressedSize()
{
    return s_xesam()->xesam_totalUncompressedSize;
}

QUrl Soprano::Vocabulary::Xesam::trackGain()
{
    return s_xesam()->xesam_trackGain;
}

QUrl Soprano::Vocabulary::Xesam::trackNumber()
{
    return s_xesam()->xesam_trackNumber;
}

QUrl Soprano::Vocabulary::Xesam::trackPeakGain()
{
    return s_xesam()->xesam_trackPeakGain;
}

QUrl Soprano::Vocabulary::Xesam::url()
{
    return s_xesam()->xesam_url;
}

QUrl Soprano::Vocabulary::Xesam::useCount()
{
    return s_xesam()->xesam_useCount;
}

QUrl Soprano::Vocabulary::Xesam::userComment()
{
    return s_xesam()->xesam_userComment;
}

QUrl Soprano::Vocabulary::Xesam::userKeyword()
{
    return s_xesam()->xesam_userKeyword;
}

QUrl Soprano::Vocabulary::Xesam::userRating()
{
    return s_xesam()->xesam_userRating;
}

QUrl Soprano::Vocabulary::Xesam::usesNamespace()
{
    return s_xesam()->xesam_usesNamespace;
}

QUrl Soprano::Vocabulary::Xesam::version()
{
    return s_xesam()->xesam_version;
}

QUrl Soprano::Vocabulary::Xesam::verticalResolution()
{
    return s_xesam()->xesam_verticalResolution;
}

QUrl Soprano::Vocabulary::Xesam::videoBitrate()
{
    return s_xesam()->xesam_videoBitrate;
}

QUrl Soprano::Vocabulary::Xesam::videoCodec()
{
    return s_xesam()->xesam_videoCodec;
}

QUrl Soprano::Vocabulary::Xesam::videoCodecType()
{
    return s_xesam()->xesam_videoCodecType;
}

QUrl Soprano::Vocabulary::Xesam::whiteBalance()
{
    return s_xesam()->xesam_whiteBalance;
}

QUrl Soprano::Vocabulary::Xesam::width()
{
    return s_xesam()->xesam_width;
}

QUrl Soprano::Vocabulary::Xesam::wordCount()
{
    return s_xesam()->xesam_wordCount;
}

QUrl Soprano::Vocabulary::Xesam::workEmailAddress()
{
    return s_xesam()->xesam_workEmailAddress;
}

QUrl Soprano::Vocabulary::Xesam::workPhoneNumber()
{
    return s_xesam()->xesam_workPhoneNumber;
}

QUrl Soprano::Vocabulary::Xesam::workPostalAddress()
{
    return s_xesam()->xesam_workPostalAddress;
}
