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

from autopilot.matchers import Eventually
from testtools.matchers import Equals

from ubuntu_filemanager_app.tests import FileManagerTestCase


class TestMainWindow(FileManagerTestCase):

    def setUp(self):
        self._patch_home()
        super(TestMainWindow, self).setUp()
        self.assertThat(
            self.main_window.get_qml_view().visible, Eventually(Equals(True)))

    def _patch_home(self):
        temp_dir = tempfile.mkdtemp()
        self.addCleanup(shutil.rmtree, temp_dir)
        patcher = mock.patch.dict('os.environ', {'HOME': temp_dir})
        patcher.start()
        self.addCleanup(patcher.stop)

    def test_toolbar_shows(self):
        """Dragging from the bottom reveals the hidden toolbar."""
        tempfile.mkdtemp(dir=os.environ['HOME'])
        # Currently, we need to open again the home folder to show the newly
        # created one. See bug #1190676.
        # TODO when the bug is fixed, remove the next line.
        self.pointing_device.click_object(self.main_window.get_home_button())

        first_folder = self.main_window.get_folder(0)
        self.tap_item(first_folder)

        action_popover = self.main_window.get_action_popover()
        self.assertThat(lambda: action_popover.opacity, Eventually(Equals(1)))
