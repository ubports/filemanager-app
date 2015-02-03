/*
 * Copyright (C) 2014 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Arto Jalkanen <ajalkane@gmail.com>
 */
.import Ubuntu.Content 0.1 as CH

/**
 * For now a simple static mapping of file extensions to ContentHub types.
 * This is enough for RTM, but in the future a better strategy would be
 * for asking for the MIME-type of the file and mapping that to the
 * ContentType
 */
var __mapping = {
    'png': CH.ContentType.Pictures,
    'jpg': CH.ContentType.Pictures,
    'jpeg': CH.ContentType.Pictures,
    'bmp': CH.ContentType.Pictures,
    'gif': CH.ContentType.Pictures,

    'mp3': CH.ContentType.Music,
    'ogg': CH.ContentType.Music,
    'wav': CH.ContentType.Music,
    'm4a': CH.ContentType.Music,
    'opus': CH.ContentType.Music,
    'flac': CH.ContentType.Music,

    'avi': CH.ContentType.Videos,
    'mpeg': CH.ContentType.Videos,
    'mp4': CH.ContentType.Videos,
    'mkv': CH.ContentType.Videos,
    'mov': CH.ContentType.Videos,
    'wmv': CH.ContentType.Videos,

    'txt': CH.ContentType.Documents,
    'doc': CH.ContentType.Documents,
    'docx': CH.ContentType.Documents,
    'xls': CH.ContentType.Documents,
    'xlsx': CH.ContentType.Documents,
    'pdf': CH.ContentType.Documents,
}

function resolveContentType(fileUrl) {
    console.log("resolveContentType for file", fileUrl)
    var extension = __fileExtension(fileUrl)
    extension = extension.toLowerCase()
    console.log("file extension:", extension)
    var contentType = __mapping[extension]

    if (contentType === null) {
        console.log("Unrecognized extension", extension)
        contentType = CH.ContentType.Unknown
    }

    console.log("returning contentType:", contentType)

    return contentType
}

function __fileExtension(fileUrl) {
    var lastDotIndex = fileUrl.lastIndexOf('.')
    return lastDotIndex > -1 ? fileUrl.substring(lastDotIndex + 1) : ''
}

