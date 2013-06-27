# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

"""File Manager app autopilot emulators."""


class MainWindow(object):
    """An emulator class that makes it easy to interact with the
    filemanager-app.

    """
    def __init__(self, app):
        self.app = app

    def get_folder(self, index):
        """Returns the list view folder with index number."""
        return self.app.select_many("Subtitled")[index]

    def get_action_popover(self):
        return self.app.select_single("ActionSelectionPopover")

    def get_current_folder_name(self):
        # XXX the label with the folder name doesn't have an objectName.
        # See bug #1190855.
        return self.app.select_single('FolderListView').select_many(
            'Label')[0].text
