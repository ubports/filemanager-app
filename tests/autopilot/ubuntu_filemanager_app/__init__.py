# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

"""filemanager-app tests and emulators - top level package."""

import logging

from autopilot import logging as autopilot_logging
from autopilot.introspection import dbus
from ubuntuuitoolkit import emulators as toolkit_emulators


logger = logging.getLogger(__name__)


class FileManagerApp(object):
    """Autopilot helper object for the FileManager application."""

    def __init__(self, app_proxy):
        self.app = app_proxy
        self.main_view = self.app.select_single(MainView)

class MainView(toolkit_emulators.MainView):
    """File Manager MainView Autopilot emulator."""

    def get_folder_list_page(self):
        """Return the FolderListPage emulator of the MainView."""
        page = self.wait_select_single(FolderListPage)
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
        if not(self.internal_wideAspect):
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
