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
.import Ubuntu.Content 1.3 as CH

/**
 * For now a simple static mapping of file extensions to ContentHub types.
 * This is enough for RTM, but in the future a better strategy would be
 * for asking for the MIME-type of the file and mapping that to the
 * ContentType
 */

function extToContentType(ext) {
    switch(ext) {
        case "art":
        case "bmp":
        case "cdr":
        case "cdt":
        case "cpt":
        case "cr2":
        case "crw":
        case "djv":
        case "djvu":
        case "erf":
        case "gif":
        case "ico":
        case "ief":
        case "jng":
        case "jp2":
        case "jpe":
        case "jpeg":
        case "jpf":
        case "jpg":
        case "jpg2":
        case "jpm":
        case "jpx":
        case "nef":
        case "orf":
        case "pat":
        case "pbm":
        case "pcx":
        case "pgm":
        case "png":
        case "pnm":
        case "ppm":
        case "psd":
        case "ras":
        case "rgb":
        case "svg":
        case "svgz":
        case "tif":
        case "tiff":
        case "wbmp":
        case "xbm":
        case "xpm":
        case "xwd":
            return CH.ContentType.Pictures;
        case "3gp":
        case "asf":
        case "asx":
        case "avi":
        case "axv":
        case "dif":
        case "dl":
        case "dv":
        case "fli":
        case "flv":
        case "gl":
        case "lsf":
        case "lsx":
        case "m4v":
        case "mkv":
        case "mng":
        case "mov":
        case "movie":
        case "mp4":
        case "mpe":
        case "mpeg":
        case "mpg":
        case "mpv":
        case "mxu":
        case "ogv":
        case "qt":
        case "ts":
        case "webm":
        case "wm":
        case "wmv":
        case "wmx":
        case "wvx":
            return CH.ContentType.Videos;
        case "aif":
        case "aifc":
        case "aiff":
        case "amr":
        case "au":
        case "awb":
        case "axa":
        case "csd":
        case "flac":
        case "gsm":
        case "kar":
        case "m3u":
        case "m4a":
        case "mid":
        case "midi":
        case "mp2":
        case "mp3":
        case "mpega":
        case "mpga":
        case "oga":
        case "ogg":
        case "opus":
        case "orc":
        case "pls":
        case "ra":
        case "ram":
        case "rm":
        case "sco":
        case "sd2":
        case "sid":
        case "snd":
        case "spx":
        case "wav":
        case "wax":
        case "wma":
            return CH.ContentType.Music;
        case "vcard":
        case "vcf":
            return CH.ContentType.Contacts;
        case "323":
        case "appcache":
        case "asc":
        case "bib":
        case "boo":
        case "brf":
        case "c":
        case "c++":
        case "cc":
        case "cls":
        case "cpp":
        case "csh":
        case "css":
        case "csv":
        case "cxx":
        case "d":
        case "diff":
        case "doc":
        case "docx":
        case "etx":
        case "gcd":
        case "h":
        case "hh":
        case "h++":
        case "hpp":
        case "hs":
        case "htc":
        case "htm":
        case "html":
        case "hxx":
        case "ics":
        case "icz":
        case "jad":
        case "java":
        case "lhs":
        case "ltx":
        case "ly":
        case "mml":
        case "moc":
        case "odp":
        case "ods":
        case "odt":
        case "p":
        case "pas":
        case "patch":
        case "pdf":
        case "pl":
        case "pm":
        case "pot":
        case "ppt":
        case "pptx":
        case "py":
        case "rtx":
        case "scala":
        case "sct":
        case "sfv":
        case "sh":
        case "shtml":
        case "srt":
        case "sty":
        case "tcl":
        case "tex":
        case "text":
        case "tk":
        case "tm":
        case "tsv":
        case "ttl":
        case "txt":
        case "uls":
        case "vcs":
        case "wml":
        case "wmls":
        case "wsc":
        case "xls":
        case "xlsx":
            return CH.ContentType.Documents;
        case "epub":
        case "mobi":
        case "lit":
        case "fb2":
        case "azw":
        case "tpz":
            return CH.ContentType.EBooks;
        default:
            return CH.ContentType.Unknown;
    }
}

function resolveContentType(fileUrl) {
    console.log("resolveContentType for file", fileUrl)
    var extension = __fileExtension(fileUrl)
    extension = extension.toLowerCase()
    console.log("file extension:", extension)
    var contentType = extToContentType(extension)

    if (typeof contentType === undefined) {
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
