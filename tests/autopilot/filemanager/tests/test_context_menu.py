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

import os
from filemanager.tests import FileManagerTestCase


class ContextMenuTestCase(FileManagerTestCase):
    """Test cases for the context menu of the file manager app."""

    def get_current_directory_files_and_folders(self):
        folder_list_page = self.app.main_view.get_folder_list_page()
        return folder_list_page.get_files_and_folders()

    def test_rename_directory(self):
        """Test renaming a directory.

        The name of the directory must be updated on the list of folder
        contents.

        """
        original_directory = os.path.basename(self.make_directory_in_home())
        new_name = original_directory + 'edit'

        self.app.main_view.rename(original_directory, new_name)

        files_and_folders = self.get_current_directory_files_and_folders()
        self.assertTrue(new_name in files_and_folders)

    def test_rename_file(self):
        """Test renaming a file.

        The name of the file must be updated on the list of folder contents.

        """
        original_file = os.path.basename(self.make_file_in_home())
        new_name = original_file + 'edit'

        self.app.main_view.rename(original_file, new_name)

        files_and_folders = self.get_current_directory_files_and_folders()
        self.assertTrue(new_name in files_and_folders)

    def test_delete_directory(self):
        """Test deleting a directory.

        The directory must no longer be displayed on the list of folder
        contents.

        """
        dir_name = os.path.basename(self.make_directory_in_home())

        self.app.main_view.delete(dir_name)

        files_and_folders = self.get_current_directory_files_and_folders()
        self.assertFalse(dir_name in files_and_folders)

    def test_delete_file(self):
        """Test deleting a file.

        The file must no longer be displayed on the list of folder contents.

        """
        file_name = os.path.basename(self.make_file_in_home())

        self.app.main_view.delete(file_name)

        files_and_folders = self.get_current_directory_files_and_folders()
        self.assertFalse(file_name in files_and_folders)
