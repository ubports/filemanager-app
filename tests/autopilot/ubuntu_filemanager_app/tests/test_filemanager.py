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

"""File Manager app autopilot tests."""

from __future__ import absolute_import

import tempfile

import mock
import os
import shutil

from autopilot import process
from autopilot.matchers import Eventually
from testtools.matchers import Equals

from ubuntu_filemanager_app.tests import FileManagerTestCase


class TestFolderListPage(FileManagerTestCase):

    def setUp(self):
        self._patch_home()
        super(TestFolderListPage, self).setUp()
        self.assertThat(
            self.main_view.visible, Eventually(Equals(True)))

    def _patch_home(self):
        temp_dir = tempfile.mkdtemp()
        self.addCleanup(shutil.rmtree, temp_dir)
        patcher = mock.patch.dict('os.environ', {'HOME': temp_dir})
        patcher.start()
        self.addCleanup(patcher.stop)

    def test_file_context_menu_shows(self):
        """Checks to make sure that the file actions popover is shown."""
        self._make_file_in_home()

        first_file = self._get_file_by_index(0)
        first_file.open_actions_popover()

        file_actions_popover = self.main_view.get_file_actions_popover()
        self.assertThat(
            lambda: file_actions_popover.visible, Eventually(Equals(True)))

    def _make_file_in_home(self):
        return self._make_content_in_home('file')

    def _make_content_in_home(self, type_):
        if type_ != 'file' and type_ != 'directory':
            raise ValueError('Unknown content type: "{0}"', type_)
        folder_list_page = self.main_view.get_folder_list_page()
        original_count = (
            folder_list_page.get_number_of_files_from_list())
        if type_ == 'file':
            _, path = tempfile.mkstemp(dir=os.environ['HOME'])
        else:
            path = tempfile.mkdtemp(dir=os.environ['HOME'])

        self._assert_number_of_files(original_count + 1)
        return path

    def _assert_number_of_files(self, expected_number_of_files):
        folder_list_page = self.main_view.get_folder_list_page()
        self.assertThat(
            folder_list_page.get_number_of_files_from_list,
            Eventually(Equals(expected_number_of_files)))
        self.assertThat(
            folder_list_page.get_number_of_files_from_header,
            Eventually(Equals(expected_number_of_files)))

    def _get_file_by_index(self, index):
        folder_list_page = self.main_view.get_folder_list_page()
        return folder_list_page.get_file_by_index(index)

    def test_folder_context_menu_shows(self):
        """Checks to make sure that the folder actions popover is shown."""
        self._make_directory_in_home()

        first_file = self._get_file_by_index(0)
        first_file.open_actions_popover()

        file_actions_popover = self.main_view.get_file_actions_popover()
        self.assertThat(
            lambda: file_actions_popover.visible, Eventually(Equals(True)))

    def _make_directory_in_home(self):
        return self._make_content_in_home('directory')

    def test_list_folder_contents(self):
        dir_path = self._make_directory_in_home()
        dir_name = os.path.basename(dir_path)
        file_path = self._make_file_in_home()
        file_name = os.path.basename(file_path)

        self._assert_number_of_files(2)

        dir_ = self._get_file_by_index(0)
        self.assertThat(dir_.fileName, Eventually(Equals(dir_name)))

        file_ = self._get_file_by_index(1)
        self.assertThat(file_.fileName, Eventually(Equals(file_name)))

    def test_cancel_file_action_dialog(self):
        self._make_file_in_home()

        first_file = self._get_file_by_index(0)
        self.pointing_device.click_object(first_file)

        dialog = self.main_view.get_file_action_dialog()
        dialog.visible.wait_for(True)
        dialog.cancel()
        self.assertThat(
            self.main_view.get_file_action_dialog, Eventually(Equals(None)))

    def test_open_file(self):
        self._make_file_in_home()

        first_file = self._get_file_by_index(0)
        self.pointing_device.click_object(first_file)

        dialog = self.main_view.get_file_action_dialog()
        dialog.visible.wait_for(True)

        process_manager = process.ProcessManager.create()
        original_apps = process_manager.get_running_applications()

        dialog.open()
        self.assertThat(
            self.main_view.get_file_action_dialog, Eventually(Equals(None)))
        # Filtering copied from
        # AutopilotTestCase._compare_system_with_app_snapshot.
        current_apps = self.process_manager.get_running_applications()
        new_apps = filter(
            lambda i: i not in original_apps, current_apps)
        # Assert that only one window was opened.
        self.assertEqual(len(new_apps), 1)
        new_app = new_apps[0]
        self.assertEqual(len(new_app.get_windows()), 1)

        # TODO assert that the file was opened on the right application. This
        # depends on what's the default application to open a text file. Maybe
        # we can get this information with XDG. --elopio - 2013-07-25
        # Close the opened window.
        window = new_app.get_windows()[0]
        window.close()

    def test_open_directory(self):
        dir_path = self._make_directory_in_home()
        first_dir = self._get_file_by_index(0)

        first_dir.open_directory()

        folder_list_page = self.main_view.get_folder_list_page()
        self.assertThat(
            folder_list_page.get_current_path, Eventually(Equals(dir_path)))
        self._assert_number_of_files(0)
        # TODO check the label that says the directory is empty.
        # --elopio - 2013-07-25

    def test_cancel_rename_directory(self):
        dir_path = self._make_directory_in_home()
        dir_name = os.path.basename(dir_path)

        first_dir = self._get_file_by_index(0)
        self._do_action_on_file(first_dir, action='Rename')
        self._cancel_confirm_dialog()

        self.assertThat(
            self.main_view.get_confirm_dialog, Eventually(Equals(None)))
        self.assertThat(
            lambda: first_dir.fileName, Eventually(Equals(dir_name)))

    def _do_action_on_file(self, file_, action):
        file_.open_actions_popover()
        file_actions_popover = self.main_view.get_file_actions_popover()
        file_actions_popover.click_button(action)

    def _cancel_confirm_dialog(self):
        confirm_dialog = self.main_view.get_confirm_dialog()
        confirm_dialog.cancel()

    def test_rename_directory(self):
        self._make_directory_in_home()
        new_name = 'Renamed directory'

        first_dir = self._get_file_by_index(0)
        self._do_action_on_file(first_dir, action='Rename')
        self._confirm_dialog(new_name)

        self.assertThat(
            self.main_view.get_confirm_dialog, Eventually(Equals(None)))
        self.assertThat(
            lambda: first_dir.fileName, Eventually(Equals(new_name)))

    def _confirm_dialog(self, text=None):
        confirm_dialog = self.main_view.get_confirm_dialog()
        if text:
            confirm_dialog.enter_text(text)
        confirm_dialog.ok()

    def test_cancel_rename_file(self):
        file_path = self._make_file_in_home()
        file_name = os.path.basename(file_path)

        first_file = self._get_file_by_index(0)
        self._do_action_on_file(first_file, action='Rename')
        self._cancel_confirm_dialog()

        self.assertThat(
            self.main_view.get_confirm_dialog, Eventually(Equals(None)))
        self.assertThat(
            lambda: first_file.fileName, Eventually(Equals(file_name)))

    def test_rename_file(self):
        self._make_file_in_home()
        new_name = 'Renamed file'

        first_file = self._get_file_by_index(0)
        self._do_action_on_file(first_file, action='Rename')
        self._confirm_dialog(new_name)

        self.assertThat(
            self.main_view.get_confirm_dialog, Eventually(Equals(None)))
        self.assertThat(
            lambda: first_file.fileName, Eventually(Equals(new_name)))

    def test_cancel_delete_directory(self):
        self._make_directory_in_home()
        first_dir = self._get_file_by_index(0)

        self._do_action_on_file(first_dir, 'Delete')
        self._cancel_confirm_dialog()

        self._assert_number_of_files(1)

    def test_delete_directory(self):
        self._make_directory_in_home()
        first_dir = self._get_file_by_index(0)

        self._do_action_on_file(first_dir, 'Delete')
        self._confirm_dialog()

        self._assert_number_of_files(0)

    def test_cancel_delete_file(self):
        self._make_file_in_home()
        first_file = self._get_file_by_index(0)

        self._do_action_on_file(first_file, 'Delete')
        self._cancel_confirm_dialog()

        self._assert_number_of_files(1)

    def test_delete_file(self):
        self._make_file_in_home()
        first_file = self._get_file_by_index(0)

        self._do_action_on_file(first_file, 'Delete')
        self._confirm_dialog()

        self._assert_number_of_files(0)

    def test_create_directory(self):
        dir_name = 'Test Directory'

        toolbar = self.main_view.open_toolbar()
        toolbar.click_button('actions')

        folder_actions_popover = self.main_view.get_folder_actions_popover()
        folder_actions_popover.click_button('Create New Folder')
        self._confirm_dialog(dir_name)

        self._assert_number_of_files(1)

        dir_ = self._get_file_by_index(0)
        self.assertThat(dir_.fileName, Eventually(Equals(dir_name)))
        # TODO missing test, cancel create directory. --elopio - 2013-07-25

    def test_show_directory_properties_from_list(self):
        dir_path = self._make_directory_in_home()
        first_dir = self._get_file_by_index(0)

        self._do_action_on_file(first_dir, 'Properties')
        file_details_popover = self.main_view.get_file_details_popover()
        self.assertThat(file_details_popover.visible, Eventually(Equals(True)))
        self.assertThat(
            file_details_popover.get_path, Eventually(Equals(dir_path)))
        # TODO check the rest of the fields. --elopio - 2013-07-25
        # TODO missing test, show directory properties from toolbar.
        # --elopio - 2013-07-25

    def test_show_file_properties(self):
        file_path = self._make_file_in_home()
        first_file = self._get_file_by_index(0)

        self._do_action_on_file(first_file, 'Properties')
        file_details_popover = self.main_view.get_file_details_popover()
        self.assertThat(file_details_popover.visible, Eventually(Equals(True)))
        self.assertThat(
            file_details_popover.get_path, Eventually(Equals(file_path)))
        # TODO check the rest of the fields. --elopio - 2013-07-25

    def test_copy_directory(self):
        # Set up a directory to copy and a directory to copy it into.
        destination_dir_path = os.path.join(os.environ['HOME'], 'destination')
        destination_dir_name = os.path.basename(destination_dir_path)
        os.mkdir(destination_dir_path)
        dir_to_copy_path = os.path.join(os.environ['HOME'], 'to_copy')
        dir_to_copy_name = os.path.basename(dir_to_copy_path)
        os.mkdir(dir_to_copy_path)

        folder_list_page = self.main_view.get_folder_list_page()
        self._assert_number_of_files(2)

        # Copy the directory.
        dir_to_copy = folder_list_page.get_file_by_name(dir_to_copy_name)
        self._do_action_on_file(dir_to_copy, 'Copy')

        # Go to the destination directory.
        destination_dir = folder_list_page.get_file_by_name(
            destination_dir_name)
        destination_dir.open_directory()

        # Paste the directory.
        toolbar = self.main_view.open_toolbar()
        toolbar.click_button('actions')

        folder_actions_popover = self.main_view.get_folder_actions_popover()
        folder_actions_popover.click_button('Paste 1 File')
        self.main_view.get_folder_actions_popover().visible.wait_for(False)

        # Check that the directory is there.
        self._assert_number_of_files(1)
        first_dir = self._get_file_by_index(0)
        self.assertThat(
            first_dir.fileName, Eventually(Equals(dir_to_copy_name)))

        # Go back.
        toolbar = self.main_view.open_toolbar()
        toolbar.click_button('up')

        # Check that the directory is still there.
        self._assert_number_of_files(2)

    def test_cut_directory(self):
        # Set up a directory to cut and a directory to move it into.
        destination_dir_path = os.path.join(os.environ['HOME'], 'destination')
        destination_dir_name = os.path.basename(destination_dir_path)
        os.mkdir(destination_dir_path)
        dir_to_cut_path = os.path.join(os.environ['HOME'], 'to_cut')
        dir_to_cut_name = os.path.basename(dir_to_cut_path)
        os.mkdir(dir_to_cut_path)

        folder_list_page = self.main_view.get_folder_list_page()
        self._assert_number_of_files(2)

        # Cut the directory.
        dir_to_cut = folder_list_page.get_file_by_name(dir_to_cut_name)
        self._do_action_on_file(dir_to_cut, 'Cut')

        # Go to the destination directory.
        destination_dir = folder_list_page.get_file_by_name(
            destination_dir_name)
        destination_dir.open_directory()

        # Paste the directory.
        toolbar = self.main_view.open_toolbar()
        toolbar.click_button('actions')

        folder_actions_popover = self.main_view.get_folder_actions_popover()
        folder_actions_popover.click_button('Paste 1 File')
        self.main_view.get_folder_actions_popover().visible.wait_for(False)

        # Check that the directory is there.
        self._assert_number_of_files(1)
        first_dir = self._get_file_by_index(0)
        self.assertThat(
            first_dir.fileName, Eventually(Equals(dir_to_cut_name)))

        # Go back.
        toolbar = self.main_view.open_toolbar()
        toolbar.click_button('up')

        # Check that the directory is not there.
        self._assert_number_of_files(1)
        first_dir = self._get_file_by_index(0)
        self.assertThat(
            first_dir.fileName, Eventually(Equals(destination_dir_name)))

    def test_copy_file(self):
        # Set up a file to copy and a directory to copy it into.
        destination_dir_path = self._make_directory_in_home()
        destination_dir_name = os.path.basename(destination_dir_path)
        file_to_copy_path = self._make_file_in_home()
        file_to_copy_name = os.path.basename(file_to_copy_path)

        folder_list_page = self.main_view.get_folder_list_page()
        self._assert_number_of_files(2)

        # Copy the file.
        file_to_copy = folder_list_page.get_file_by_name(file_to_copy_name)
        self._do_action_on_file(file_to_copy, 'Copy')

        # Go to the destination directory.
        destination_dir = folder_list_page.get_file_by_name(
            destination_dir_name)
        destination_dir.open_directory()

        # Paste the file.
        toolbar = self.main_view.open_toolbar()
        toolbar.click_button('actions')

        folder_actions_popover = self.main_view.get_folder_actions_popover()
        folder_actions_popover.click_button('Paste 1 File')
        self.main_view.get_folder_actions_popover().visible.wait_for(False)

        # Check that the file is there.
        self._assert_number_of_files(1)
        first_dir = self._get_file_by_index(0)
        self.assertThat(
            first_dir.fileName, Eventually(Equals(file_to_copy_name)))

        # Go back.
        toolbar = self.main_view.open_toolbar()
        toolbar.click_button('up')

        # Check that the file is still there.
        self._assert_number_of_files(2)

    def test_cut_file(self):
        # Set up a file to cut and a directory to move it into.
        destination_dir_path = self._make_directory_in_home()
        destination_dir_name = os.path.basename(destination_dir_path)
        file_to_cut_path = self._make_file_in_home()
        file_to_cut_name = os.path.basename(file_to_cut_path)

        folder_list_page = self.main_view.get_folder_list_page()
        self._assert_number_of_files(2)

        # Cut the file.
        file_to_cut = folder_list_page.get_file_by_name(file_to_cut_name)
        self._do_action_on_file(file_to_cut, 'Cut')

        # Go to the destination directory.
        destination_dir = folder_list_page.get_file_by_name(
            destination_dir_name)
        destination_dir.open_directory()

        # Paste the file.
        toolbar = self.main_view.open_toolbar()
        toolbar.click_button('actions')

        folder_actions_popover = self.main_view.get_folder_actions_popover()
        folder_actions_popover.click_button('Paste 1 File')
        self.main_view.get_folder_actions_popover().visible.wait_for(False)

        # Check that the file is there.
        self._assert_number_of_files(1)
        first_dir = self._get_file_by_index(0)
        self.assertThat(
            first_dir.fileName, Eventually(Equals(file_to_cut_name)))

        # Go back.
        toolbar = self.main_view.open_toolbar()
        toolbar.click_button('up')

        # Check that the file is not there.
        self._assert_number_of_files(1)
        first_dir = self._get_file_by_index(0)
        self.assertThat(
            first_dir.fileName, Eventually(Equals(destination_dir_name)))

    def test_go_up(self):
        self._make_directory_in_home()
        first_dir = self._get_file_by_index(0)
        first_dir.open_directory()

        toolbar = self.main_view.open_toolbar()
        toolbar.click_button('up')

        folder_list_page = self.main_view.get_folder_list_page()
        self.assertThat(
            folder_list_page.get_current_path,
            Eventually(Equals(os.environ['HOME'])))

    def test_go_home(self):
        self._go_to_place('Home')

        folder_list_page = self.main_view.get_folder_list_page()
        self.assertThat(
            folder_list_page.get_current_path,
            Eventually(Equals(os.environ['HOME'])))

    def test_go_to_root(self):
        self._go_to_place('File System')

        folder_list_page = self.main_view.get_folder_list_page()
        self.assertThat(
            folder_list_page.get_current_path,
            Eventually(Equals('/')))

    def _go_to_place(self, text):
        # XXX We are receiving the text because there's no way to set the
        # objectName on the ListElement. This is reported at
        # https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1205201
        # --elopio - 2013-07-25
        self.main_view.open_toolbar()
        self.main_view.get_toolbar().click_button('places')
        place = self._get_place(text)
        self.pointing_device.click_object(place)

    def _get_place(self, text):
        places_popover = self.main_view.get_places_popover()
        places = places_popover.select_many('Standard')
        for place in places:
            if place.text == text:
                return place
        raise ValueError(
            'Place "{0}" not found.'.format(text))
