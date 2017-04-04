# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2013, 2014 Canonical Ltd.
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

import logging
import re
import time
import os
import shutil

import autopilot.logging
from autopilot import input
from autopilot.introspection import dbus
import ubuntuuitoolkit

logger = logging.getLogger(__name__)


class FilemanagerException(ubuntuuitoolkit.ToolkitException):

    """Exception raised when there are problems with the Filemanager."""


class Filemanager(object):

    """Autopilot helper object for the filemanager application."""

    def __init__(self, app_proxy, test_type):
        self.app = app_proxy
        self.test_type = test_type
        self.main_view = self.app.select_single(MainView)

    @property
    def pointing_device(self):
        return self.app.pointing_device


class MainView(ubuntuuitoolkit.MainView):
    """File Manager MainView Autopilot emulator."""

    def __init__(self, *args):
        super(MainView, self).__init__(*args)

    @autopilot.logging.log_action(logger.info)
    def go_to_place(self, object_name, file_to_extract):
        """Open one of the bookmarked place folders.

        :param object_name: The objectName property of the place to open.

        """

        if self.showSidebar:
            self._go_to_place_from_side_bar(object_name)
        else:
            self._go_to_place_from_places_page(object_name, file_to_extract)

    def _go_to_place_from_side_bar(self, object_name):
        dir_path = os.getenv('HOME')
        if object_name == 'placePath':
            self.click_header_action('Find')
            go_to_dialog = self.get_go_to_dialog()
            go_to_dialog.enter_text(dir_path)
            go_to_dialog.ok()
        else:
            side_bar = self.get_folder_list_page().get_sidebar()
            side_bar.go_to_place(object_name)

    def get_folder_list_page(self):
        """Return the FolderListPage emulator of the MainView."""
        page = self.wait_select_single(FolderListPage)
        page.main_view = self
        return page

    def get_places_page(self):
        return self.wait_select_single(PlacesPage)

    def _go_to_place_from_places_page(self, object_name, file_to_extract):
        dir_path = os.getenv('HOME')
        if object_name == 'placePath':
            self.copy_file_from_source_dir(dir_path, file_to_extract)
        placespage = self.open_places()
        placespage.go_to_place(object_name, dir_path)

    @autopilot.logging.log_action(logger.info)
    def open_places(self):
        if not self.showSidebar:
            self._drag_bottomedge_to_open_places()
            return self.wait_select_single(PlacesPage)
        else:
            raise FilemanagerException(
                'The places page cannot be opened on wide mode.')

    def _drag_bottomedge_to_open_places(self):
        """Bring the places page to the screen"""
        try:
            action_item = self.wait_select_single(
                'UCUbuntuShape', objectName='bottomEdgeTip')
            action_item.visible.wait_for(True)
            action_item.isAnimating.wait_for(False)
            start_x = (action_item.globalRect.x +
                       (action_item.globalRect.width * 0.5))
            start_y = (action_item.globalRect.y +
                       (action_item.height * 0.5))
            stop_y = start_y - (self.height * 0.7)
            self.pointing_device.drag(start_x, start_y,
                                      start_x, stop_y, rate=2)

        except dbus.StateNotFoundError:
            logger.error('BottomEdge element not found.')
            raise

        flickable = self.wait_select_single(
            "QQuickFlickable", objectName="placesFlickable")
        flickable.wait_until_destroyed()

    @autopilot.logging.log_action(logger.info)
    def rename(self, original_name, new_name):
        """Rename a file or directory.

        :param original_name: The name of the file or directory to rename.
        :param new_name: The new name to set for the file or directory.

        """
        actions_popover = self.open_actions(original_name)
        actions_popover.click_button_by_text('Rename')
        confirm_dialog = self.wait_select_single(ConfirmDialogWithInput)
        confirm_dialog.enter_text(new_name)
        confirm_dialog.ok()

    @autopilot.logging.log_action(logger.info)
    def open_actions(self, name):
        """Open the list of available actions of a file or directory."""
        folder_list_page = self.get_folder_list_page()
        folder_list_page.open_file_actions(name)
        actions_popover = self.get_action_selection_popover(
            'fileActionsPopover')
        actions_popover.visible.wait_for(True)
        return actions_popover

    @autopilot.logging.log_action(logger.info)
    def delete(self, name):
        """Delete a file or directory.

        :param name: The name of the file or directory to delete.

        """
        actions_popover = self.open_actions(name)
        actions_popover.click_button_by_text('Delete')
        confirm_dialog = self.wait_select_single(ConfirmDialog)
        confirm_dialog.ok()

    def get_file_actions_popover(self):
        """Return the ActionSelectionPopover emulator of the file actions."""
        return self.get_action_selection_popover('fileActionsPopover')

    def click_header_action(self, objectName):
        header = self.get_header()
        header.click_action_button(objectName)

    def click_back(self):
        header = self.get_header()
        header.click_custom_back_button()

    # TODO: can probably be removed
    def get_folder_actions_popover(self):
        """Return the ActionSelectionPopover emulator of the folder actions."""
        return self.get_action_selection_popover('folderActionsPopover')

    # TODO: can probably be removed
    def folder_actions_popover_exists(self):
        """Boolean, checks if the folder actions popover exists."""
        try:
            popover = self.get_folder_actions_popover()
            if popover:
                return True
        except:
            return False

    # TODO: can probably be removed
    def file_actions_popover_exists(self):
        """Boolean, checks if the file actions popover exists."""
        try:
            popover = self.get_file_actions_popover()
            if popover:
                return True
        except:
            return False

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

    def get_go_to_dialog(self):
        """Return a go to dialog """
        return self.wait_select_single(objectName='goToDialog')

    def copy_file_from_source_dir(self, dir_path, file_to_extract):
        content_dir_file = os.path.join(os.path.dirname(
            __file__), 'content', file_to_extract)
        shutil.copy(content_dir_file, dir_path)


class PlacesSidebar(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):
    """PlacesSidebar Autopilot emulator."""

    @autopilot.logging.log_action(logger.info)
    def go_to_place(self, object_name):
        """Open one of the bookmarked place folders or content folder depending
        on object_name parameter

        :param object_name: The objectName property of the place to open
                            If value is inputField then open content folder

        """
        place = self.wait_select_single('Standard', objectName=object_name)
        self.pointing_device.click_object(place)


class FolderListPage(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):
    """FolderListPage Autopilot emulator."""

    def open_file_actions(self, name):
        """Open the actions menu of a file or folder.

        :param name: The name of the file or folder.

        """
        delegate = self.get_file_by_name(name)
        delegate.open_actions_popover()

    def get_files_and_folders(self):
        """Return the list of files and folders of the opened directory.

        The list returned will contain the names of the files and folders.

        """
        if self.showingListView:
            view = self.select_single(FolderListView)
        else:
            view = self.select_single(FolderIconView)
        return view.get_files_and_folders()

    def get_number_of_files_from_list(self):
        """Return the number of files shown on the folder."""
        if self.showingListView:
            return len(self.select_many(FolderListDelegate))
        else:
            return len(self.select_many(FolderIconDelegate))

    def get_file_by_index(self, index):
        """Return the FolderListDelegate emulator of the file or folder.

        :parameter index: The index of file or folder.

        """
        if self.showingListView:
            file_ = self.select_many(FolderListDelegate)[index]
        else:
            file_ = self.select_many(FolderIconDelegate)[index]
        file_.list_view = self.select_single(FolderListView)
        return file_

    def get_file_by_name(self, name):
        """Return the FolderListDelegate emulator of the file or folder.

        :parameter name: The name of the file or folder.

        """
        files = self.select_many(FolderListDelegate)
        if not self.showingListView:
            files = self.select_many(FolderIconDelegate)
        for file_ in files:
            if file_.fileName == name:
                if self.showingListView:
                    file_.list_view = self.select_single(FolderListView)
                else:
                    file_.list_view = self.select_single(FolderIconView)
                return file_
        raise ValueError(
            'File with name "{0}" not found.'.format(name))

    def get_current_path(self):
        """Get the path of the folder currently displayed."""
        if self.showingListView:
            return self.select_single(FolderListView).get_current_path()
        else:
            return self.select_single(FolderIconView).get_current_path()

    def get_number_of_files_from_header(self):
        if self.showingListView:
            return self.select_single(FolderListView).get_number_of_files()
        else:
            return self.select_single(FolderIconView).get_number_of_files()

    def get_sidebar(self):
        if self.main_view.showSidebar:
            return self.select_single(PlacesSidebar)
        else:
            raise ValueError(
                'Places sidebar is hidden in small mode.')

    def get_pathbar(self):
        if self.main_view.showSidebar:
            return self.main_view.get_toolbar().select_single(PathBar)
        else:
            raise ValueError(
                'Path bar is hidden in small mode.')


class PlacesPage(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):
    """ Places Page Autopilot emulator.     """

    @autopilot.logging.log_action(logger.info)
    def go_to_place(self, object_name, dir_path):
        """Open one of the bookmarked place folders or content folder depending
            on object_name value

        :param object_name: The objectName property of the place to open
                            If equals inputField then open content folder

        """

        if object_name == 'placePath':
            place = self.wait_select_single(
                'TextField', objectName=object_name)
            place.write(dir_path, clear=True)
            ok_button = self.wait_select_single(
                "Button", objectName="okButton")
            self.pointing_device.click_object(ok_button)
        else:
            place = self.wait_select_single('Standard', objectName=object_name)
            self.pointing_device.click_object(place)


class FolderListView(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):
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
            'ListItemHeader', objectName='listViewSmallHeader').text
        match = re.match(self.SPLIT_HEADER_REGEX, header_text)
        if match:
            path = match.group(1)
            number_of_files = match.group(2)
            return path, number_of_files

    def get_number_of_files(self):
        _, number_of_files = self._split_header_text()
        return int(number_of_files)

    def get_files_and_folders(self):
        """Return the list of files and folders of the opened directory."""
        list_delegates = self.select_many(FolderListDelegate)
        # sort by y, x
        list_delegates = sorted(
            list_delegates,
            key=lambda item: (item.globalRect.y, item.globalRect.x))

        return [item.fileName for item in list_delegates]


class FolderIconView(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):
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
            'ListItemHeader', objectName='iconViewHeader').text
        match = re.match(self.SPLIT_HEADER_REGEX, header_text)
        if match:
            path = match.group(1)
            number_of_files = match.group(2)
            return path, number_of_files

    def get_number_of_files(self):
        _, number_of_files = self._split_header_text()
        return int(number_of_files)

    def get_files_and_folders(self):
        """Return the list of files and folders of the opened directory."""
        icon_delegates = self.select_many(FolderIconDelegate)
        # sort by y, x
        icon_delegates = sorted(
            icon_delegates,
            key=lambda icon: (icon.globalRect.y, icon.globalRect.x))

        return [icon.fileName for icon in icon_delegates]


class FolderListDelegate(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):
    """FolderListPage Autopilot emulator.

    This is a file or folder on the FolderListPage.

    """

    def __init__(self, *args):
        super(FolderListDelegate, self).__init__(*args)
        self.pointing_device = ubuntuuitoolkit.get_pointing_device()

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
        time.sleep(2)
        self.pointing_device.release()


class FolderIconDelegate(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):
    """FolderIconPage Autopilot emulator.

    This is a file or folder on the FolderListPage.

    """

    def __init__(self, *args):
        super(FolderIconDelegate, self).__init__(*args)
        self.pointing_device = ubuntuuitoolkit.get_pointing_device()

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
        raise NotImplementedError

    def open_actions_popover(self):
        """Open the actions popover of the file or folder."""
        self.pointing_device.move_to_object(self)
        self.pointing_device.press()
        time.sleep(2)
        self.pointing_device.release()


class FileActionDialog(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):
    """FileActionDialog Autopilot emulator."""

    def __init__(self, *args):
        super(FileActionDialog, self).__init__(*args)
        self.pointing_device = ubuntuuitoolkit.get_pointing_device()

    def open(self):
        open_button = self.select_single('Button', objectName='openButton')
        self.pointing_device.click_object(open_button)

    def cancel(self):
        cancel_button = self.select_single('Button', objectName='cancelButton')
        self.pointing_device.click_object(cancel_button)


class ConfirmDialog(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):
    """ConfirmDialog Autopilot emulator."""

    def __init__(self, *args):
        super(ConfirmDialog, self).__init__(*args)
        self.pointing_device = ubuntuuitoolkit.get_pointing_device()

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
        text_field = self._select_text_field()
        text_field.write(text, clear)

    def _select_text_field(self):
        return self.select_single(
            ubuntuuitoolkit.TextField, objectName='inputField')


class Dialog(ConfirmDialogWithInput):
    """Dialog Autopilot emulator."""


class Popover(ConfirmDialogWithInput):
    """Popover Autopilot emulator, containing buttons and an inputfield"""

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


class FileDetailsPopover(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):
    """FileDetailsPopover Autopilot emulator."""

    def get_path(self):
        return self.select_single('UCLabel', objectName='pathLabel').text


class PathBar(ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase):

    def go_to_location(self):
        editButton = self.select_single(objectName='goToButton')
        self.pointing_device.click_object(editButton)
