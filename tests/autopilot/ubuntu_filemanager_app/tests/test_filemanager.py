# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

"""File Manager app autopilot tests."""

from __future__ import absolute_import

from autopilot.matchers import Eventually
from testtools.matchers import Equals

from ubuntu_filemanager_app.tests import FileManagerTestCase


class TestMainWindow(FileManagerTestCase):

    def setUp(self):
        super(TestMainWindow, self).setUp()
        self.assertThat(
            self.main_window.get_qml_view().visible, Eventually(Equals(True)))

    def test_toolbar_shows(self):
        """Make sure that dragging from the bottom reveals the hidden
        toolbar."""
        first_folder = self.main_window.get_folder(0)
        self.tap_item(first_folder)

        action_popover = self.main_window.get_action_popover()
        self.assertThat(lambda: action_popover.opacity, Eventually(Equals(1)))
