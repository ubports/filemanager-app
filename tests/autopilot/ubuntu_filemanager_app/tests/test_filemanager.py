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
import os.path
import shutil

from autopilot.matchers import Eventually
from testtools.matchers import Equals

from ubuntu_filemanager_app.tests import FileManagerTestCase


class TestMainWindow(FileManagerTestCase):

    def setUp(self):
        self._patch_home()
        super(TestMainWindow, self).setUp()
        self.assertThat(
            self.ubuntusdk.get_qml_view().visible, Eventually(Equals(True)))

    def _patch_home(self):
        temp_dir = tempfile.mkdtemp()
        self.addCleanup(shutil.rmtree, temp_dir)
        patcher = mock.patch.dict('os.environ', {'HOME': temp_dir})
        patcher.start()
        self.addCleanup(patcher.stop)

    def _get_place(self, name):
        """Returns the place/bookmark with index number."""
        self.ubuntusdk.click_toolbar_button('Places')
        places_popover = self.app.select_single('Popover', objectName='placesPopover')
        places = places_popover.select_many('Standard')
        for place in places:
            if place.text == name:
                return place

    def test_file_actions_shows(self):
        """Checks to make sure that the file actions popover is shown."""
        self._make_directory_in_home()

        first_folder = self.main_window.get_folder(0)
        self.tap_item(first_folder)

        action_popover = self.main_window.get_action_popover()
        self.assertThat(lambda: action_popover.opacity, Eventually(Equals(1)))

    def _make_directory_in_home(self):
        path = tempfile.mkdtemp(dir=os.environ['HOME'])
        # Currently, we need to open again the home folder to show the newly
        # created one. See bug #1190676.
        # TODO when the bug is fixed, remove the following lines up to the assert line
        home_place = self._get_place("Home")
        self.pointing_device.click_object(home_place)

        self.assertThat(self.main_window.get_folder_count, Eventually(Equals(1)))

        return path

    def test_open_directory(self):
        sub_dir = self._make_directory_in_home()

        first_folder = self.main_window.get_folder(0)
        self.pointing_device.click_object(first_folder)
        self.assertThat(
            self.main_window.get_current_folder_name,
            Eventually(Equals(sub_dir)))

    def test_going_up(self):
        upDir = os.path.split(os.environ['HOME'])[0]
        upName = os.path.split(upDir)[1]

        home_place = self._get_place("Home")
        self.pointing_device.click_object(home_place)

        self._check_location("Home", os.environ['HOME'])

        self.ubuntusdk.click_toolbar_button('Up')
        self._check_location(upName, upDir)

    def test_going_home(self):
        home_place = self._get_place("Home")
        self.pointing_device.click_object(home_place)

        self._check_location("Home", os.environ['HOME'])

    def test_going_to_root(self):
        root_place = self._get_place("File System")
        self.pointing_device.click_object(root_place)

        self._check_location("File System", "/")

    def _check_location(self,title,location):
        self.assertThat(self.main_window.get_page_title, Eventually(Equals(title)))

        self.assertThat(self.main_window.get_current_folder_name,
            Eventually(Equals(location)))
