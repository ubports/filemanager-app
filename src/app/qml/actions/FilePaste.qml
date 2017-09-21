/*
 * Copyright (C) 2013 Canonical Ltd
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
 *              Niklas Wenzel <nikwen.developer@gmail.com>
 */

import QtQuick 2.4
import Ubuntu.Components 1.3

Action {
    property int clipboardUrlsCounter

    // Translation message was implemented according to:
    // http://developer.ubuntu.com/api/qml/sdk-14.04/Ubuntu.Components.i18n/
    // It allows correct translation for languages with more than two plural forms:
    // http://localization-guide.readthedocs.org/en/latest/l10n/pluralforms.html
    text: i18n.tr("Paste %1 file", "Paste %1 files", clipboardUrlsCounter).arg(clipboardUrlsCounter)
    visible: clipboardUrlsCounter > 0
    iconName: "edit-paste"
}
