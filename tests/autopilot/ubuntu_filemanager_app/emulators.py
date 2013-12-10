# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2013 Canonical Ltd.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; version 3.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

"""File Manager app autopilot emulators."""

import re
import time

from autopilot import input
from autopilot.introspection import dbus

from ubuntuuitoolkit import emulators as toolkit_emulators


class MainView(toolkit_emulators.MainView):
    """File Manager MainView Autopilot emulator."""

    def get_folder_list_page(self):
        """Return the FolderListPage emulator of the MainView."""
        page = self.select_single(FolderListPage)
        page.main_view = self
        return page

    def get_file_actions_popover(self):
        """Return the ActionSelectionPopover emulator of the file actions."""
        return self.wait_select_single(
            ActionSelectionPopover, objectName='fileActionsPopover')

    def get_folder_actions_popover(self):
        """Return the ActionSelectionPopover emulator of the folder actions."""
        return self.wait_select_single(
            ActionSelectionPopover, objectName='folderActionsPopover')

    def folder_actions_popover_exists(self):
        """Boolean, checks if the Actions Popover exists."""
        popover = self.select_many(
            ActionSelectionPopover, objectName='folderActionsPopover')
        if popover == '[]':
            return True
        return False

    def get_places_popover(self):
        """Return the Places popover."""
        if not(self.wideAspect):
            # XXX It would be easier if the places popover was an object
            # that inherits from Popover, like the
            # ActionSelectionPopover does.
            # --elopio - 2013-07-25
            return self.select_single('Popover', objectName='placesPopover')
        else:
            raise ValueError(
                'Places sidebar is hidden in wide mode.')

    def get_file_details_popover(self):
        """Return the FileDetailsPopover emulator."""
        return self.wait_select_single(FileDetailsPopover)

    def get_file_action_dialog(self):
        """Return the FileActionDialog emulator."""
        return self.wait_select_single(FileActionDialog)

    def file_action_dialog_exists(self):
        """Boolean checks if the FileActionDialog exists."""
        dialog = self.select_many(FileActionDialog)
        if dialog == '[]':
            return True
        return False

    def get_confirm_dialog(self):
        """Return a confirm dialog emulator"""
        try:
            dialog = self.wait_select_single(ConfirmDialog)
        except dbus.StateNotFoundError:
            time.sleep(1)
            dialog = self.wait_select_single(ConfirmDialogWithInput)
        return dialog

    def confirm_dialog_exists(self):
        """Boolean checks if a confirm dialog exists"""
        dialog = self.select_many(ConfirmDialog)
        if dialog == '[]':
            dialog = self.select_many(ConfirmDialogWithInput)
        if dialog == '[]':
            return True
        return False

    def get_dialog(self):
        """Return a dialog emulator"""
        return self.wait_select_single(Dialog)

    def get_popover(self):
        """Return a popover emulator"""
        return self.wait_select_single(Popover)


class Sidebar(toolkit_emulators.UbuntuUIToolkitEmulatorBase):
    """PlacesSidebar Autopilot emulator."""

    def get_place(self, text):
        places = self.select_many('Standard')
        for place in places:
            if place.text == text:
                return place
        raise ValueError(
            'Place "{0}" not found.'.format(text))


class FolderListPage(toolkit_emulators.UbuntuUIToolkitEmulatorBase):
    """FolderListPage Autopilot emulator."""

    def get_number_of_files_from_list(self):
        """Return the number of files shown on the folder."""
        return len(self.select_many(FolderListDelegate))

    def get_file_by_index(self, index):
        """Return the FolderListDelegate emulator of the file or folder.

        :parameter index: The index of file or folder.

        """
        file_ = self.select_many(FolderListDelegate)[index]
        file_.list_view = self.select_single(FolderListView)
        return file_

    def get_file_by_name(self, name):
        """Return the FolderListDelegate emulator of the file or folder.

        :parameter name: The name of the file or folder.

        """
        files = self.select_many(FolderListDelegate)
        for file_ in files:
            if file_.fileName == name:
                file_.list_view = self.select_single(FolderListView)
                return file_
        raise ValueError(
            'File with name "{0}" not found.'.format(name))

    def get_current_path(self):
        return self.select_single(FolderListView).get_current_path()

    def get_number_of_files_from_header(self):
        return self.select_single(FolderListView).get_number_of_files()

    def get_sidebar(self):
        if self.main_view.wideAspect:
            return self.select_single(Sidebar)
        else:
            raise ValueError(
                'Places sidebar is hidden in wide mode.')


class FolderListView(toolkit_emulators.UbuntuUIToolkitEmulatorBase):
    """FolderListView Autopilot emulator."""

    SPLIT_HEADER_REGEX = '(.+) \((\d+) \w+\)$'
    # Regular expression to split the header text. The header text has the form
    # /path/to/dir (# files). So with this expression, we can split the header
    # in two groups, (.+) will match the path and (\d+) the number of files.

    def get_current_path(self):
        path, _ = self._split_header_text()
        return path

    def _split_header_text(self):
        header_text = self.select_single(
            'Header', objectName='directoryHeader').text
        match = re.match(self.SPLIT_HEADER_REGEX, header_text)
        if match:
            path = match.group(1)
            number_of_files = match.group(2)
            return path, number_of_files

    def get_number_of_files(self):
        _, number_of_files = self._split_header_text()
        return int(number_of_files)


class FolderListDelegate(toolkit_emulators.UbuntuUIToolkitEmulatorBase):
    """FolderListPage Autopilot emulator.

    This is a file or folder on the FolderListPage.

    """

    def __init__(self, *args):
        super(FolderListDelegate, self).__init__(*args)
        self.pointing_device = toolkit_emulators.get_pointing_device()

    def open_directory(self):
        """Open the directory."""
        # TODO Check if it is a directory. If not, raise an error.
        # This is not currently possible because Autopilot is overwriting the
        # path attribute. Reported on
        # https://bugs.launchpad.net/autopilot/+bug/1205204
        # --elopio - 2013-07-25
        self.pointing_device.click_object(self)

    def open_file(self):
        # TODO For this we would need to access the FileActionDialog that's
        # child of the MainView, but that's not currently possible with
        # autopilot. Reported on
        # bug https://bugs.launchpad.net/autopilot/+bug/1195141
        # --elopio - 2013-07-25
        raise NotImplementedError()

    def open_actions_popover(self):
        """Open the actions popover of the file or folder."""
        self.pointing_device.move_to_object(self)
        self.pointing_device.press()
        time.sleep(1)
        self.pointing_device.release()
        # TODO wait for the popover to be opened. For this we would need to
        # access the MainView, but that's not currently possible with
        # autopilot. Reported on
        # https://bugs.launchpad.net/autopilot/+bug/1195141
        # --elopio - 2013-07-25


class FileActionDialog(toolkit_emulators.UbuntuUIToolkitEmulatorBase):
    """FileActionDialog Autopilot emulator."""

    def __init__(self, *args):
        super(FileActionDialog, self).__init__(*args)
        self.pointing_device = toolkit_emulators.get_pointing_device()

    def open(self):
        open_button = self.select_single('Button', objectName='openButton')
        self.pointing_device.click_object(open_button)

    def cancel(self):
        cancel_button = self.select_single('Button', objectName='cancelButton')
        self.pointing_device.click_object(cancel_button)


class ActionSelectionPopover(toolkit_emulators.UbuntuUIToolkitEmulatorBase):
    """ActionSelectionPopover Autopilot emulator."""
    # TODO Move this to the ubuntu-ui-toolkit. Reported on
    # https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1205205
    # --elopio - 2013-07-25

    def __init__(self, *args):
        super(ActionSelectionPopover, self).__init__(*args)
        self.pointing_device = toolkit_emulators.get_pointing_device()

    def click_button(self, text):
        """Click a button on the popover.

        XXX We are receiving the text because there's no way to set the
        objectName on the action. This is reported at
        https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1205144
        --elopio - 2013-07-25

        :parameter text: The text of the button.

        """
        button = self._get_button(text)
        self.pointing_device.click_object(button)

    def _get_button(self, text):
        buttons = self.select_many('Empty')
        for button in buttons:
            if button.text == text:
                return button


class ConfirmDialog(toolkit_emulators.UbuntuUIToolkitEmulatorBase):
    """ConfirmDialog Autopilot emulator."""

    def __init__(self, *args):
        super(ConfirmDialog, self).__init__(*args)
        self.pointing_device = toolkit_emulators.get_pointing_device()

    def ok(self):
        okButton = self.select_single('Button', objectName='okButton')
        self.pointing_device.click_object(okButton)

    def cancel(self):
        cancel_button = self.select_single('Button', objectName='cancelButton')
        self.pointing_device.click_object(cancel_button)


class ConfirmDialogWithInput(ConfirmDialog):
    """ConfirmDialogWithInput Autopilot emulator."""

    def __init__(self, *args):
        super(ConfirmDialogWithInput, self).__init__(*args)
        self.keyboard = input.Keyboard.create()

    def enter_text(self, text, clear=True):
        if clear:
            self.clear_text()
        text_field = self._select_text_field()
        self.pointing_device.click_object(text_field)
        self.keyboard.type(text)
        text_field.text.wait_for(text)

    def clear_text(self):
        text_field = self._select_text_field()
        # XXX The clear button doesn't have an objectName. Reported on
        # https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1205208
        # --elopio - 2013-07-25
        clear_button = text_field.select_single('AbstractButton')
        # XXX for some reason, we need to click the button twice.
        # More investigation is needed. --elopio - 2013-07-25
        self.pointing_device.click_object(clear_button)
        self.pointing_device.click_object(clear_button)
        text_field.text.wait_for('')

    def _select_text_field(self):
        return self.select_single('TextField', objectName='inputField')


class Dialog(ConfirmDialogWithInput):
    """Dialog Autopilot emulator."""

    def __init__(self, *args):
        super(Dialog, self).__init__(*args)


class Popover(ConfirmDialogWithInput):
    """Popover Autopilot emulator, containing buttons and an inputfield"""

    def __init__(self, *args):
        super(Popover, self).__init__(*args)

    def click_button(self, text):
        """Click a button on the popover.

        XXX We are receiving the text because there's no way to set the
        objectName on the action. This is reported at
        https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1205144
        --elopio - 2013-07-25

        :parameter text: The text of the button.

        """
        button = self._get_button(text)
        self.pointing_device.click_object(button)

    def _get_button(self, text):
        buttons = self.select_many('Empty')
        for button in buttons:
            if button.text == text:
                return button


class FileDetailsPopover(toolkit_emulators.UbuntuUIToolkitEmulatorBase):
    """FileDetailsPopover Autopilot emulator."""

    def get_path(self):
        return self.select_single('Label', objectName='pathLabel').text
