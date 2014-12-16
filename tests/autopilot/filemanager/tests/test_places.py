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

from autopilot.matchers import Eventually
from testtools.matchers import Equals

from filemanager.tests import FileManagerTestCase


class PlacesTestCase(FileManagerTestCase):
    """Test cases for the places section of the file manager app."""

    def test_go_home_must_open_the_home_directory(self):
        """Test that opens the Home bookmark from the places section."""
        self.app.main_view.go_to_place('placeHome', None)

        folder_list_page = self.app.main_view.get_folder_list_page()
        self.assertThat(
            folder_list_page.get_current_path,
            Eventually(Equals(self.fakehome)))

    def test_go_to_root_must_open_the_root_directory(self):
        """Test that opens the Device bookmark from the places section."""
        self.app.main_view.go_to_place('placeDevice', None)

        folder_list_page = self.app.main_view.get_folder_list_page()
        self.assertThat(
            folder_list_page.get_current_path,
            Eventually(Equals('/')))
