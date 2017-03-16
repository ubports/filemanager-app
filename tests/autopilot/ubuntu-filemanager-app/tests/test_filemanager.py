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

"""File Manager app autopilot tests."""

import unittest
import logging
import mimetypes

import os

from autopilot import process
from autopilot.platform import model
from autopilot.matchers import Eventually
from testtools.matchers import Equals, NotEquals

from filemanager.tests import FileManagerTestCase

logger = logging.getLogger(__name__)


class TestFolderListPage(FileManagerTestCase):
    """Tests the Folder List  page features"""

    scenarios = [
        ('zip',
            {'file_to_extract': 'Test.zip',
             'extracted_dir_name': 'Test',
             'extracted_text_file_name': 'CodeOfConduct.txt',
             'extracted_image_dir_name': 'images',
             'extracted_image_name': 'ubuntu.jpg'
             }),

        ('tar',
            {'file_to_extract': 'Test.tar',
             'extracted_dir_name': 'Test',
             'extracted_text_file_name': 'CodeOfConduct.txt',
             'extracted_image_dir_name': 'images',
             'extracted_image_name': 'ubuntu.jpg'
             }),

        ('tar_gz',
            {'file_to_extract': 'Test.tar.gz',
             'extracted_dir_name': 'Test',
             'extracted_text_file_name': 'CodeOfConduct.txt',
             'extracted_image_dir_name': 'images',
             'extracted_image_name': 'ubuntu.jpg'
             }),

        ('tar_bz2',
            {'file_to_extract': 'Test.tar.bz2',
             'extracted_dir_name': 'Test',
             'extracted_text_file_name': 'CodeOfConduct.txt',
             'extracted_image_dir_name': 'images',
             'extracted_image_name': 'ubuntu.jpg'
             })
    ]

    def _get_file_by_name(self, name):
        folder_list_page = self.app.main_view.get_folder_list_page()
        fileDelegate = lambda: folder_list_page.get_file_by_name(name)
        self.assertThat(
            fileDelegate,
            Eventually(NotEquals(None)))
        return folder_list_page.get_file_by_name(name)

    def _get_file_by_index(self, index):
        folder_list_page = self.app.main_view.get_folder_list_page()
        fileDelegate = lambda: folder_list_page.get_file_by_index(index)
        self.assertThat(
            fileDelegate,
            Eventually(NotEquals(None)))
        return folder_list_page.get_file_by_index(index)

    def _go_to_location(self, location):
        # go to specified location
        # on wide UI display, we get the location dialog
        # on phone UI display, we get places popover
        device = model()
        if self.app.main_view.showSidebar:
            logger.debug("Using goto to goto %s on %s" % (location, device))
            (self.app.main_view.get_folder_list_page().get_pathbar()
                .go_to_location())
            goto_location = self.app.main_view.get_dialog()
        else:
            logger.debug("Using places to goto %s on %s" % (location, device))
            self._safe_open_popover(self.app.main_view.open_places)
            goto_location = self.app.main_view.get_popover()
        goto_location.enter_text(location)
        goto_location.ok()

    def _open_directory(self, item):
        expected_path = item.filePath
        list_view = item.list_view

        # item.open_directory()
        self.app.pointing_device.click_object(item)
        self.assertThat(
            list_view.get_current_path, Eventually(Equals(expected_path)))

    def _check_popover_exists(self):
        """Boolean, checks if popover exists."""
        try:
            popover = \
                self.app.main_view.wait_select_single('ActionSelectionPopover')
            if popover:
                return True
        except:
                try:
                    popover = self.app.main_view.wait_select_single('Popover')
                    if popover:
                        return True
                except:
                    return False

    def _safe_open_popover(self, popover_open_function):
        timeout = 0
        popover_exists = False
        while timeout < 3 and not popover_exists:
            popover_open_function()
            popover_exists = self._check_popover_exists()
            timeout += 1

    def _do_action_on_file(self, file_, action):
        logger.debug("Performing %s on file %s" % (action, file_))
        self._safe_open_popover(file_.open_actions_popover)
        file_actions_popover = self.app.main_view.get_file_actions_popover()
        file_actions_popover.click_button_by_text(action)

    def _cancel_confirm_dialog(self):
        confirm_dialog = self.app.main_view.get_confirm_dialog()
        confirm_dialog.cancel()

    def _confirm_dialog(self, text=None):
        confirm_dialog = self.app.main_view.get_confirm_dialog()
        if text:
            confirm_dialog.enter_text(text)
        confirm_dialog.ok()

    def _create_file(self):
        dir_path = os.getenv('HOME')
        self.app.main_view.copy_file_from_source_dir(
            dir_path, self.file_to_extract)

    # We can't do this testcase on phablet devices because of a lack of
    # Mir backend in autopilot
    # see https://bugs.launchpad.net/autopilot/+bug/1209004
    @unittest.skip("Can't do this properly on desktop or phablet")
    def test_open_file(self):
        self.make_file_in_home()

        first_file = self._get_file_by_index(0)
        self.app.pointing_device.click_object(first_file)

        dialog = self.app.main_view.get_file_action_dialog()
        dialog.visible.wait_for(True)

        process_manager = process.ProcessManager.create()
        original_apps = process_manager.get_running_applications()

        dialog.open()
        # make sure the dialog is open
        self.app.main_view.get_file_action_dialog()
        # Filtering copied from
        # AutopilotTestCase._compare_system_with_app_snapshot.
        current_apps = self.process_manager.get_running_applications()
        new_apps = filter(
            lambda i: i not in original_apps, current_apps)
        # Assert that only one window was opened.
        self.assertEqual(len(new_apps), 1)
        new_app = new_apps[0]
        self.assertEqual(len(new_app.get_windows()), 1)

        # TODO assert that the file was opened on the right
        # application. This depends on what's the default application
        # to open a text file. Maybe we can get this information
        # with XDG. --elopio - 2013-07-25

        # Close the opened window.
        window = new_app.get_windows()[0]
        window.close()

    def test_open_directory(self):
        dir_path = self.make_directory_in_home()
        first_dir = self._get_file_by_name(os.path.basename(dir_path))

        self._open_directory(first_dir)

        folder_list_page = self.app.main_view.get_folder_list_page()
        self.assertThat(
            folder_list_page.get_current_path, Eventually(Equals(dir_path)))
        self._assert_number_of_files(0, home=False)
        # TODO check the label that says the directory is empty.
        # --elopio - 2013-07-25

    def test_list_folder_contents(self):
        dir_path = self.make_directory_in_home()
        dir_name = os.path.basename(dir_path)
        file_path = self.make_file_in_home()
        file_name = os.path.basename(file_path)

        self._assert_number_of_files(2)

        dir_ = self._get_file_by_name(dir_name)
        self.assertThat(dir_.fileName, Eventually(Equals(dir_name)))

        file_ = self._get_file_by_name(file_name)
        self.assertThat(file_.fileName, Eventually(Equals(file_name)))

    def test_cancel_rename_directory(self):
        dir_path = self.make_directory_in_home()
        dir_name = os.path.basename(dir_path)

        first_dir = self._get_file_by_name(dir_name)
        self._do_action_on_file(first_dir, action='Rename')
        self._cancel_confirm_dialog()

        self.assertThat(
            self.app.main_view.confirm_dialog_exists,
            Eventually(Equals(False)))
        self.assertThat(
            lambda: first_dir.fileName, Eventually(Equals(dir_name)))

    def test_cancel_rename_file(self):
        file_path = self.make_file_in_home()
        file_name = os.path.basename(file_path)

        first_file = self._get_file_by_name(file_name)
        self._do_action_on_file(first_file, action='Rename')
        self._cancel_confirm_dialog()

        self.assertThat(
            self.app.main_view.confirm_dialog_exists,
            Eventually(Equals(False)))
        self.assertThat(
            lambda: first_file.fileName,
            Eventually(Equals(file_name)))

    def test_cancel_delete_directory(self):
        dir_name = os.path.basename(self.make_directory_in_home())
        first_dir = self._get_file_by_name(dir_name)

        self._do_action_on_file(first_dir, 'Delete')
        self._cancel_confirm_dialog()

        self._assert_number_of_files(1)

    def test_cancel_delete_file(self):
        file_name = os.path.basename(self.make_file_in_home())
        first_file = self._get_file_by_name(file_name)

        self._do_action_on_file(first_file, 'Delete')
        self._cancel_confirm_dialog()

        self._assert_number_of_files(1)

    def test_create_directory(self):
        dir_name = 'Test Directory'

        self.app.main_view.click_header_action('createFolder')
        self._confirm_dialog(dir_name)

        self._assert_number_of_files(1)

        dir_ = self._get_file_by_name(dir_name)
        self.assertThat(dir_.fileName, Eventually(Equals(dir_name)))

    def test_cancel_create_directory(self):
        self.app.main_view.click_header_action('createFolder')
        self._cancel_confirm_dialog()

        self._assert_number_of_files(0)

    def test_show_directory_properties_from_list(self):
        dir_path = self.make_directory_in_home()
        dir_name = os.path.basename(dir_path)
        first_dir = self._get_file_by_name(dir_name)

        self._do_action_on_file(first_dir, 'Properties')
        file_details_popover = self.app.main_view.get_file_details_popover()
        self.assertThat(file_details_popover.visible, Eventually(Equals(True)))
        self.assertThat(
            file_details_popover.get_path, Eventually(Equals(dir_path)))
        # TODO check the rest of the fields. --elopio - 2013-07-25
        # TODO missing test, show directory properties from toolbar.
        # --elopio - 2013-07-25

    def test_show_file_properties(self):
        file_path = self.make_file_in_home()
        file_name = os.path.basename(file_path)
        first_file = self._get_file_by_name(file_name)

        self._do_action_on_file(first_file, 'Properties')
        file_details_popover = self.app.main_view.get_file_details_popover()
        self.assertThat(file_details_popover.visible, Eventually(Equals(True)))
        self.assertThat(
            file_details_popover.get_path, Eventually(Equals(file_path)))
        # TODO check the rest of the fields. --elopio - 2013-07-25

    def test_copy_directory(self):
        # Set up a directory to copy and a directory to copy it into.
        destination_dir_path = self.make_directory_in_home()
        destination_dir_name = os.path.basename(destination_dir_path)
        dir_to_copy_path = self.make_directory_in_home()
        dir_to_copy_name = os.path.basename(dir_to_copy_path)

        folder_list_page = self.app.main_view.get_folder_list_page()
        self._assert_number_of_files(2)

        # Copy the directory.
        dir_to_copy = folder_list_page.get_file_by_name(dir_to_copy_name)
        self._do_action_on_file(dir_to_copy, 'Copy')

        # Go to the destination directory.
        destination_dir = folder_list_page.get_file_by_name(
            destination_dir_name)
        self._open_directory(destination_dir)

        # Paste the directory.
        self.app.main_view.click_header_action('paste')

        # Check that the directory is there.
        self._assert_number_of_files(1, home=False)
        first_dir = self._get_file_by_name(dir_to_copy_name)
        self.assertThat(
            first_dir.fileName, Eventually(Equals(dir_to_copy_name)))

        # Go back.
        self.app.main_view.click_back()

        # Check that the directory is still there.
        self._assert_number_of_files(2)

    def test_cut_directory(self):
        # Set up a directory to cut and a directory to move it into.
        destination_dir_path = self.make_directory_in_home()
        destination_dir_name = os.path.basename(destination_dir_path)
        dir_to_cut_path = self.make_directory_in_home()
        dir_to_cut_name = os.path.basename(dir_to_cut_path)

        folder_list_page = self.app.main_view.get_folder_list_page()
        self._assert_number_of_files(2)

        # Cut the directory.
        dir_to_cut = folder_list_page.get_file_by_name(dir_to_cut_name)
        self._do_action_on_file(dir_to_cut, 'Cut')

        # Go to the destination directory.
        destination_dir = folder_list_page.get_file_by_name(
            destination_dir_name)
        self._open_directory(destination_dir)

        # Paste the directory.
        self.app.main_view.click_header_action('paste')

        # Check that the directory is there.
        self._assert_number_of_files(1, home=False)
        first_dir = self._get_file_by_name(dir_to_cut_name)
        self.assertThat(
            first_dir.fileName, Eventually(Equals(dir_to_cut_name)))

        # Go back.
        self.app.main_view.click_back()

        # Check that the directory is not there.
        self._assert_number_of_files(1)
        first_dir = self._get_file_by_name(destination_dir_name)
        self.assertThat(
            first_dir.fileName, Eventually(Equals(destination_dir_name)))

    def test_copy_file(self):
        # Set up a file to copy and a directory to copy it into.
        destination_dir_path = self.make_directory_in_home()
        destination_dir_name = os.path.basename(destination_dir_path)
        file_to_copy_path = self.make_file_in_home()
        file_to_copy_name = os.path.basename(file_to_copy_path)

        folder_list_page = self.app.main_view.get_folder_list_page()
        self._assert_number_of_files(2)

        # Copy the file.
        file_to_copy = folder_list_page.get_file_by_name(file_to_copy_name)
        self._do_action_on_file(file_to_copy, 'Copy')

        # Go to the destination directory.
        destination_dir = folder_list_page.get_file_by_name(
            destination_dir_name)
        self._open_directory(destination_dir)

        # Paste the file.
        self.app.main_view.click_header_action('paste')

        # Check that the file is there.
        self._assert_number_of_files(1)
        first_dir = self._get_file_by_name(file_to_copy_name)
        self.assertThat(
            first_dir.fileName, Eventually(Equals(file_to_copy_name)))

        # Go back.
        self.app.main_view.click_back()

        # Check that the file is still there.
        self._assert_number_of_files(2)
        first_dir = self._get_file_by_name(destination_dir_name)
        self.assertThat(
            first_dir.fileName, Eventually(Equals(destination_dir_name)))

    def test_cut_file(self):
        # Set up a file to cut and a directory to move it into.
        destination_dir_path = self.make_directory_in_home()
        destination_dir_name = os.path.basename(destination_dir_path)
        file_to_cut_path = self.make_file_in_home()
        file_to_cut_name = os.path.basename(file_to_cut_path)

        folder_list_page = self.app.main_view.get_folder_list_page()
        self._assert_number_of_files(2)

        # Cut the file.
        file_to_cut = folder_list_page.get_file_by_name(file_to_cut_name)
        self._do_action_on_file(file_to_cut, 'Cut')

        # Go to the destination directory.
        destination_dir = folder_list_page.get_file_by_name(
            destination_dir_name)
        self._open_directory(destination_dir)

        # Paste the file.
        self.app.main_view.click_header_action('paste')

        # Check that the file is there.
        self._assert_number_of_files(1, home=False)
        first_dir = self._get_file_by_name(file_to_cut_name)
        self.assertThat(
            first_dir.fileName, Eventually(Equals(file_to_cut_name)))

        # Go back.
        self.app.main_view.click_back()

        # Check that the file is not there.
        self._assert_number_of_files(1)
        first_dir = self._get_file_by_name(destination_dir_name)
        self.assertThat(
            first_dir.fileName, Eventually(Equals(destination_dir_name)))

    def test_go_back(self):
        dir_name = os.path.basename(self.make_directory_in_home())
        first_dir = self._get_file_by_name(dir_name)
        self._open_directory(first_dir)

        self.app.main_view.click_back()

        folder_list_page = self.app.main_view.get_folder_list_page()
        self.assertThat(
            folder_list_page.get_current_path,
            Eventually(Equals(self.fakehome)))

    def test_extract_archive_file(self):
        """Test that extracts an archive file from content directory."""
        self._create_file()
        self.app.main_view.go_to_place('placePath', self.file_to_extract)

        self._do_action_on_file(
            self._get_file_by_name(self.file_to_extract), 'Extract archive')
        self._confirm_dialog()
        self._assert_number_of_files(2)

        # Verify that the extraced directory name is correct
        self.assertThat(
            self._get_file_by_name(self.extracted_dir_name).text,
            Eventually(Equals(self.extracted_dir_name)))

        # Verify that the extraced text file name is a text file
        self._open_directory(self._get_file_by_name(
            self.extracted_dir_name))
        self.assertThat(
            self._get_file_by_name(self.extracted_text_file_name).text,
            Eventually(Equals(self.extracted_text_file_name)))

        self.assertThat(
            mimetypes.guess_type(self.extracted_text_file_name)[0],
            Equals('text/plain'))

        # Verify that the extraced image directory name is correct
        self.assertThat(
            self._get_file_by_name(self.extracted_image_dir_name).text,
            Eventually(Equals(self.extracted_image_dir_name)))

        # Verify that the extraced image file name is an image
        self._open_directory(self._get_file_by_name(
            self.extracted_image_dir_name))
        self.assertThat(
            self._get_file_by_name(self.extracted_image_name).text,
            Eventually(Equals(self.extracted_image_name)))

        self.assertThat(
            mimetypes.guess_type(self.extracted_image_name)[0],
            Equals('image/jpeg'))

    def test_cancel_extract_archive_file(self):
        """Test that cancels opening an archive file from content directory."""
        self._create_file()

        self.app.main_view.go_to_place('placePath', self.file_to_extract)
        self._do_action_on_file(
            self._get_file_by_name(self.file_to_extract), 'Extract archive')
        self._cancel_confirm_dialog()

        self._assert_number_of_files(1)
