# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

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


class TestMainWindow(FileManagerTestCase):

    def setUp(self):
        self._patch_home()
        super(TestMainWindow, self).setUp()
        self.assertThat(
            self.main_view.visible, Eventually(Equals(True)))

    def _patch_home(self):
        temp_dir = tempfile.mkdtemp()
        self.addCleanup(shutil.rmtree, temp_dir)
        patcher = mock.patch.dict('os.environ', {'HOME': temp_dir})
        patcher.start()
        self.addCleanup(patcher.stop)

    def _get_place(self, name):
        """Returns the place/bookmark with index number."""
        self.main_view.open_toolbar()
        self.main_view.get_toolbar().click_button('places')
        places_popover = self.app.select_single(
            'Popover', objectName='placesPopover')
        places = places_popover.select_many('Standard')
        for place in places:
            if place.text == name:
                return place

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
        get_file_count = folder_list_page.get_number_of_files_from_list
        original_count = get_file_count()
        if type_ == 'file':
            _, path = tempfile.mkstemp(dir=os.environ['HOME'])
        else:
            path = tempfile.mkdtemp(dir=os.environ['HOME'])

        self.assertThat(get_file_count, Eventually(Equals(original_count + 1)))
        return path

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
        dir_name = os.path.split(dir_path)[1]
        file_path = self._make_file_in_home()
        file_name = os.path.split(file_path)[1]

        folder_list_page = self.main_view.get_folder_list_page()
        self.assertEqual(folder_list_page.get_number_of_files_from_header(), 2)

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

        self.pointing_device.click_object(first_dir)

        folder_list_page = self.main_view.get_folder_list_page()
        self.assertThat(
            folder_list_page.get_current_path, Eventually(Equals(dir_path)))
        self.assertThat(
            folder_list_page.get_number_of_files_from_list,
            Eventually(Equals(0)))
        self.assertThat(
            folder_list_page.get_number_of_files_from_header,
            Eventually(Equals(0)))
        # TODO check the label that says the directory is empty.
        # --elopio - 2013-07-25

    def test_cancel_rename_directory(self):
        dir_path = self._make_directory_in_home()
        dir_name = os.path.split(dir_path)[1]

        first_dir = self._get_file_by_index(0)
        first_dir.open_actions_popover()
        file_actions_popover = self.main_view.get_file_actions_popover()
        file_actions_popover.click_button('Rename')
        confirm_dialog = self.main_view.get_confirm_dialog()
        confirm_dialog.cancel()

        self.assertThat(
            self.main_view.get_confirm_dialog, Eventually(Equals(None)))
        self.assertThat(
            lambda: first_dir.fileName, Eventually(Equals(dir_name)))

    def test_rename_directory(self):
        self._make_directory_in_home()
        new_name = 'Renamed directory'

        first_dir = self._get_file_by_index(0)
        first_dir.open_actions_popover()
        file_actions_popover = self.main_view.get_file_actions_popover()
        file_actions_popover.click_button('Rename')
        confirm_dialog = self.main_view.get_confirm_dialog()
        confirm_dialog.enter_text(new_name)
        confirm_dialog.ok()

        self.assertThat(
            self.main_view.get_confirm_dialog, Eventually(Equals(None)))
        self.assertThat(
            lambda: first_dir.fileName, Eventually(Equals(new_name)))

    def test_cancel_rename_file(self):
        path = self._make_file_in_home()
        name = os.path.split(path)[1]
        new_name = 'New Test File'

        first_file = self.main_window.get_file_item(0)

        self.tap_item(first_file)
        action_popover = self.main_window.get_action_popover()
        action_popover.click_button('Rename')

        self._cancel_action()

        self.assertThat(
            lambda: self.main_window.get_filenames()[0], Eventually(
                Equals(name)))

        self.tap_item(first_file)
        action_popover = self.main_window.get_action_popover()
        self._run_action(action_popover, 'Rename')

        self._provide_input(new_name)

        self.assertThat(
            lambda: self.main_window.get_filenames()[0], Eventually(
                Equals(new_name)))

    def test_delete_directory(self):
        self._make_directory_in_home()
        first_folder = self.main_window.get_file_item(0)

        self.tap_item(first_folder)
        action_popover = self.main_window.get_action_popover()
        self._run_action(action_popover, 'Delete')

        self._cancel_action()

        self.assertThat(self.main_window.get_file_count, Eventually(Equals(1)))

        self.tap_item(first_folder)
        action_popover = self.main_window.get_action_popover()
        self._run_action(action_popover, 'Delete')

        self._confirm_action()

        self.assertThat(self.main_window.get_file_count, Eventually(Equals(0)))

    def test_delete_file(self):
        self._make_file_in_home()
        first_folder = self.main_window.get_file_item(0)

        self.tap_item(first_folder)
        action_popover = self.main_window.get_action_popover()
        self._run_action(action_popover, 'Delete')

        self._cancel_action()

        self.assertThat(self.main_window.get_file_count, Eventually(Equals(1)))

        self.tap_item(first_folder)
        action_popover = self.main_window.get_action_popover()
        self._run_action(action_popover, 'Delete')

        self._confirm_action()

        self.assertThat(self.main_window.get_file_count, Eventually(Equals(0)))

    def test_create_directory(self):
        name = 'Test Directory'

        self._run_folder_action('Create New Folder')
        self._provide_input(name)

        self.assertThat(self.main_window.get_file_count, Eventually(Equals(1)))

        self.assertThat(
            lambda: self.main_window.get_filenames()[0], Eventually(
                Equals(name)))

    def test_showing_directory_properties(self):
        path = self._make_directory_in_home()

        first_folder = self.main_window.get_file_item(0)
        self.tap_item(first_folder)

        popover = self.main_window.get_action_popover()
        self._run_action(popover, 'Properties')

        properties_popover = self.app.select_single('FileDetailsPopover')
        self.assertThat(lambda: properties_popover.opacity,
                        Eventually(Equals(1)))
        path_label = properties_popover.select_single(
            'Label', objectName='pathLabel')
        self.assertThat(lambda: path_label.text,
                        Eventually(Equals(path)))

    def test_showing_file_properties(self):
        path = self._make_file_in_home()

        first_file = self.main_window.get_file_item(0)
        self.tap_item(first_file)

        popover = self.app.select_single(
            "ActionSelectionPopover", objectName='fileActionsPopover')
        self._run_action(popover, 'Properties')

        properties_popover = self.app.select_single(
            'FileDetailsPopover')
        self.assertThat(lambda: properties_popover.opacity,
                        Eventually(Equals(1)))
        path_label = properties_popover.select_single(
            'Label', objectName='pathLabel')
        self.assertThat(lambda: path_label.text, Eventually(Equals(path)))

    def test_copy_folder(self):
        # Set up a folder to copy and a folder to copy it into
        sub_dir = os.environ['HOME'] + '/Destination'
        os.mkdir(sub_dir)
        copy_dir = os.environ['HOME'] + '/Folder to Copy'
        os.mkdir(copy_dir)

        self.assertThat(self.main_window.get_file_count, Eventually(Equals(2)))

        # Copy the folder
        first_file = self.main_window.get_file_item(1)
        self.assertThat(
            first_file.fileName,
            Eventually(Equals(os.path.split(copy_dir)[1])))
        self._run_file_action(first_file, 'Copy')

        # Go to the destination folder
        first_folder = self.main_window.get_file_item(0)
        self.assertThat(
            first_folder.fileName,
            Eventually(Equals(os.path.split(sub_dir)[1])))

        self.pointing_device.click_object(first_folder)

        # Paste it in
        self._run_folder_action('Paste 1 File')

        # Check that the folder is there
        self.assertThat(self.main_window.get_file_count, Eventually(Equals(1)))
        first_file = self.main_window.get_file_item(0)
        self.assertThat(
            first_file.fileName,
            Eventually(Equals(os.path.split(copy_dir)[1])))

        # Go back
        self._go_up()

        # Check that the file is still there
        self.assertThat(self.main_window.get_file_count, Eventually(Equals(2)))
        first_file = self.main_window.get_file_item(1)
        self.assertThat(
            first_file.fileName,
            Eventually(Equals(os.path.split(copy_dir)[1])))

    def test_cut_folder(self):
        # Set up a folder to cut and a folder to move it into
        sub_dir = os.environ['HOME'] + '/Destination'
        os.mkdir(sub_dir)
        copy_dir = os.environ['HOME'] + '/Folder to Cut'
        os.mkdir(copy_dir)

        self.assertThat(self.main_window.get_file_count, Eventually(Equals(2)))

        # Cut the folder
        first_file = self.main_window.get_file_item(1)
        self.assertThat(
            first_file.fileName,
            Eventually(Equals(os.path.split(copy_dir)[1])))
        self._run_file_action(first_file, 'Cut')

        # Go to the destination folder
        first_folder = self.main_window.get_file_item(0)
        self.assertThat(
            first_folder.fileName,
            Eventually(Equals(os.path.split(sub_dir)[1])))

        self.pointing_device.click_object(first_folder)

        # Paste it in
        self._run_folder_action('Paste 1 File')

        # Check that the folder is there
        self.assertThat(self.main_window.get_file_count, Eventually(Equals(1)))
        first_file = self.main_window.get_file_item(0)
        self.assertThat(
            first_file.fileName,
            Eventually(Equals(os.path.split(copy_dir)[1])))

        # Go back
        self._go_up()

        # Check that the folder is not there
        self.assertThat(self.main_window.get_file_count, Eventually(Equals(1)))

    def test_copy_file(self):
        pass
        # Set up a file to copy and a folder to copy it into
        # Copy the file
        # Go to the destination folder
        # Paste it in
        # Check that the file is there
        # Go back
        # Check that the folder is not there

    def test_cut_file(self):
        # Set up a file to cut and a folder to copy it into
        sub_dir = self._make_directory_in_home()
        fileName = self._make_file_in_home()

        # Cut the file
        first_file = self.main_window.get_file_item(1)
        self.assertThat(
            first_file.fileName,
            Eventually(Equals(os.path.split(fileName)[1])))
        self.tap_item(first_file)

        popover = self.app.select_single(
            "ActionSelectionPopover", objectName='fileActionsPopover')
        self._run_action(popover, 'Cut')

        # Go to the folder
        first_folder = self.main_window.get_file_item(0)
        self.assertThat(
            first_folder.fileName,
            Eventually(Equals(os.path.split(sub_dir)[1])))

        self.pointing_device.click_object(first_folder)

        # Paste it in
        self._run_folder_action('Paste 1 File')

        # Check that the file is there
        self.assertThat(self.main_window.get_file_count, Eventually(Equals(1)))
        first_file = self.main_window.get_file_item(0)
        self.assertThat(
            first_file.fileName,
            Eventually(Equals(os.path.split(fileName)[1])))

        # Go back
        self._go_up()

        # Check that the file is not there
        self.assertThat(self.main_window.get_file_count, Eventually(Equals(1)))

    def _go_up(self):
        self.ubuntusdk.click_toolbar_button('Up')

    def test_going_up(self):
        upDir = os.path.split(os.environ['HOME'])[0]
        upName = os.path.split(upDir)[1]

        #home_place = self._go_to_place("Home")

        self._check_location("Home", os.environ['HOME'])

        self.ubuntusdk.click_toolbar_button('Up')
        self._check_location(upName, upDir)

    def test_going_home(self):
        self._go_to_place('Home')

        self._check_location("Home", os.environ['HOME'])

    def test_going_to_root(self):
        self._go_to_place('File System')

        self._check_location("File System", "/")

    def _go_to_place(self, name):
        place = self._get_place(name)
        self.pointing_device.click_object(place)

    def _check_location(self, title, location):
        self.assertThat(
            self.main_window.get_page_title, Eventually(Equals(title)))

        self.assertThat(
            self.main_window.get_current_folder_name,
            Eventually(Equals(location)))

    def _run_file_action(self, item, name):
        self.tap_item(item)

        popover = self.app.select_single(
            "ActionSelectionPopover", objectName='fileActionsPopover')
        self._run_action(popover, name)

    def _run_folder_action(self, name):
        self.ubuntusdk.click_toolbar_button('Actions')
        popover = self.app.select_single(
            "ActionSelectionPopover", objectName='folderActionsPopover')
        self._run_action(popover, name)

    def _run_action(self, popover, name):
        actions = popover.select_many('Empty')
        requested = None
        for action in actions:
            if action.text == name:
                requested = action
        self.pointing_device.click_object(requested)

    def _confirm_action(self):
        dialog = self.app.select_single('ConfirmDialog')
        if dialog is None:
            dialog = self.app.select_single('ConfirmDialogWithInput')
        okButton = dialog.select_single('Button', objectName='okButton')
        self.pointing_device.click_object(okButton)

    def _cancel_action(self):
        dialog = self.app.select_single('ConfirmDialog')
        if dialog is None:
            dialog = self.app.select_single('ConfirmDialogWithInput')
        cancelButton = dialog.select_single(
            'Button', objectName='cancelButton')
        self.pointing_device.click_object(cancelButton)

    def _provide_input(self, text):
        """Fill in the input dialog"""
        dialog = self.app.select_single('ConfirmDialogWithInput')
        field = dialog.select_single('TextField')

        self.pointing_device.click_object(field)

        clearButton = field.select_single('AbstractButton')
        #if clearButton:
        self.pointing_device.click_object(clearButton)

        self.assertThat(field.text, Eventually(Equals("")))

        self.pointing_device.click_object(field)

        self.keyboard.type(text)
        self.assertThat(field.text, Eventually(Equals(text)))

        okButton = dialog.select_single('Button', objectName='okButton')
        self.pointing_device.click_object(okButton)
