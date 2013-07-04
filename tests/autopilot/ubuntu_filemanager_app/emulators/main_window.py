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
        return self.app.select_single('FolderListPage').select_many("Subtitled")[index]

    def get_folder_count(self):
        return len(self.app.select_single('FolderListPage').select_many("Subtitled"))

    def get_action_popover(self):
        # Returns all instances, but with current one as first index
        return self.app.select_many("ActionSelectionPopover")[0]

    def get_current_folder_name(self):
        return self.app.select_single('FolderListView').folderPath

    def get_page_title(self):
        return self.app.select_single('FolderListPage').title
