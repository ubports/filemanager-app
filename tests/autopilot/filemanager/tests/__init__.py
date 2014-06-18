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

"""Filemanager app autopilot tests."""

import logging
import os
import shutil

import fixtures
from autopilot import logging as autopilot_logging
from autopilot.input import Mouse, Touch, Pointer
from autopilot.matchers import Eventually
from autopilot.platform import model
from autopilot.testcase import AutopilotTestCase
from testtools.matchers import Equals
from ubuntuuitoolkit import (
    emulators as toolkit_emulators,
    fixture_setup as toolkit_fixtures
)

from filemanager import emulators, fixture_setup

logger = logging.getLogger(__name__)


class FileManagerTestCase(AutopilotTestCase):

    """A common test case class that provides several useful methods for
    filemanager-app tests.

    """
    if model() == 'Desktop':
        scenarios = [('with mouse', dict(input_device_class=Mouse))]
    else:
        scenarios = [('with touch', dict(input_device_class=Touch))]

    local_location = os.path.dirname(os.path.dirname(os.getcwd()))
    local_location_qml = os.path.join(local_location,
                                      'src/app/qml/filemanager.qml')
    local_location_binary = os.path.join(local_location, 'src/app/filemanager')
    installed_location_qml = '/usr/share/filemanager/qml/filemanager.qml'
    installed_location_binary = '/usr/bin/filemanager'

    def get_launcher_and_type(self):
        if os.path.exists(self.local_location_binary):
            launcher = self.launch_test_local
            test_type = 'local'
        elif os.path.exists(self.installed_location_binary):
            launcher = self.launch_test_installed
            test_type = 'deb'
        else:
            launcher = self.launch_test_click
            test_type = 'click'
        return launcher, test_type

    def setUp(self):
        launcher, self.test_type = self.get_launcher_and_type()
        self.home_dir = self._patch_home()
        self.pointing_device = Pointer(self.input_device_class.create())
        super(FileManagerTestCase, self).setUp()

        #turn off the OSK so it doesn't block screen elements
        if model() != 'Desktop':
            os.system('stop maliit-server')
            self.addCleanup(os.system, 'start maliit-server')

        self.original_file_count = \
            len([i for i in os.listdir(self.home_dir)
                 if not i.startswith('.')])
        logger.debug('Directory Listing for HOME\n%s' %
                     os.listdir(self.home_dir))
        logger.debug('File count in HOME is %s' % self.original_file_count)

        self.app = launcher()

        self.assertThat(
            self.main_view.visible, Eventually(Equals(True)))

    @autopilot_logging.log_action(logger.info)
    def launch_test_local(self):
        self.useFixture(fixtures.EnvironmentVariable(
            'QML2_IMPORT_PATH', newvalue=os.path.join(self.local_location,
                                                      'src', 'plugin')))
        return self.launch_test_application(
            self.local_location_binary,
            '-q', self.local_location_qml,
            app_type='qt',
            emulator_base=toolkit_emulators.UbuntuUIToolkitEmulatorBase)

    @autopilot_logging.log_action(logger.info)
    def launch_test_installed(self):
        return self.launch_test_application(
            self.installed_location_binary,
            '-q', self.installed_location_qml,
            app_type='qt',
            emulator_base=toolkit_emulators.UbuntuUIToolkitEmulatorBase)

    @autopilot_logging.log_action(logger.info)
    def launch_test_click(self):
        return self.launch_click_package(
            'com.ubuntu.filemanager',
            emulator_base=toolkit_emulators.UbuntuUIToolkitEmulatorBase)

    def _copy_xauthority_file(self, directory):
        """Copy .Xauthority file to directory, if it exists in /home"""
        xauth = os.path.join(os.environ.get('HOME'), '.Xauthority')
        if os.path.isfile(xauth):
            logger.debug("Copying .Xauthority to " + directory)
            shutil.copyfile(
                os.path.join(os.environ.get('HOME'), '.Xauthority'),
                os.path.join(directory, '.Xauthority'))

    def _patch_home(self):
        """mock /home for testing purposes to preserve user data"""
        temp_dir_fixture = fixtures.TempDir()
        self.useFixture(temp_dir_fixture)
        temp_dir = temp_dir_fixture.path

        #If running under xvfb, as jenkins does,
        #xsession will fail to start without xauthority file
        #Thus if the Xauthority file is in the home directory
        #make sure we copy it to our temp home directory
        self._copy_xauthority_file(temp_dir)

        #click requires using initctl env (upstart), but the desktop can set
        #an environment variable instead
        if self.test_type == 'click':
            self.useFixture(toolkit_fixtures.InitctlEnvironmentVariable(
                            HOME=temp_dir))
        else:
            self.useFixture(fixtures.EnvironmentVariable('HOME',
                                                         newvalue=temp_dir))

        logger.debug('Patched home to fake home directory ' + temp_dir)

        return temp_dir

    @property
    def main_view(self):
        return self.app.wait_select_single(emulators.MainView)

    def make_file_in_home(self):
        return self.make_content_in_home('file')

    def make_directory_in_home(self):
        return self.make_content_in_home('directory')

    def make_content_in_home(self, type_):
        if type_ != 'file' and type_ != 'directory':
            raise ValueError('Unknown content type: "{0}"', type_)
        if type_ == 'file':
            temp_file = fixture_setup.TemporaryFileInDirectory(self.home_dir)
            self.useFixture(temp_file)
            path = temp_file.path
        else:
            temp_dir = fixture_setup.TemporaryDirectoryInDirectory(
                self.home_dir)
            self.useFixture(temp_dir)
            path = temp_dir.path
        logger.debug('Directory Listing for HOME\n%s' %
                     os.listdir(self.home_dir))
        self._assert_number_of_files(1)
        return path

    def _assert_number_of_files(self, expected_number_of_files, home=True):
        if home:
            expected_number_of_files += self.original_file_count
        folder_list_page = self.main_view.get_folder_list_page()
        self.assertThat(
            folder_list_page.get_number_of_files_from_list,
            Eventually(Equals(expected_number_of_files), timeout=60))
        self.assertThat(
            folder_list_page.get_number_of_files_from_header,
            Eventually(Equals(expected_number_of_files), timeout=60))
