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
import tempfile

import fixtures
from autopilot import logging as autopilot_logging
from filemanager import CMakePluginParser

from autopilot.matchers import Eventually
from autopilot.testcase import AutopilotTestCase
from testtools.matchers import Equals
import ubuntuuitoolkit

import filemanager
from filemanager import fixture_setup as fm_fixtures
import gi
gi.require_version('Click', '0.4')
from gi.repository import Click

logger = logging.getLogger(__name__)


class BaseTestCaseWithPatchedHome(AutopilotTestCase):

    """A common test case class that provides several useful methods for
    filemanager-app tests.

    """

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
        self.binary = 'filemanager'
        self.source_dir = os.path.dirname(
            os.path.dirname(os.path.abspath('.')))
        self.build_dir = self._get_build_dir()

        self.local_location = self.build_dir
        self.local_location_qml = os.path.join(self.build_dir,
                                               'src', 'app',
                                               'qml', self.binary + '.qml')
        self.local_location_binary = os.path.join(self.build_dir,
                                                  'src', 'app', self.binary)
        self.installed_location_binary = os.path.join('/usr/bin/', self.binary)
        self.installed_location_qml = \
            '/usr/share/filemanager/qml/filemanager.qml'
        super(BaseTestCaseWithPatchedHome, self).setUp()
        self.launcher, self.test_type = self.get_launcher_and_type()
        self.real_home_dir = os.getenv('HOME')
        self.patch_home()

    @autopilot_logging.log_action(logger.info)
    def launch_test_local(self):
        self.useFixture(fixtures.EnvironmentVariable(
            'QML2_IMPORT_PATH', newvalue=os.path.join(self.local_location,
                                                      'src', 'plugin')))
        return self.launch_test_application(
            self.local_location_binary,
            '-p',
            '-q', self.local_location_qml,
            app_type='qt',
            emulator_base=ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase)

    @autopilot_logging.log_action(logger.info)
    def launch_test_installed(self):
        return self.launch_test_application(
            self.installed_location_binary,
            '-q', self.installed_location_qml,
            app_type='qt',
            emulator_base=ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase)

    @autopilot_logging.log_action(logger.info)
    def launch_test_click(self):
        # We need to pass the "--forceAuth false" argument to the filemanager
        # binary, but ubuntu-app-launch doesn't pass arguments to the exec line
        # on the desktop file. So we make a test desktop file that has the
        # "--forceAuth false"  on the exec line.
        desktop_file_path = self.write_sandbox_desktop_file()
        desktop_file_name = os.path.basename(desktop_file_path)
        application_name, _ = os.path.splitext(desktop_file_name)
        return self.launch_upstart_application(
            application_name,
            emulator_base=ubuntuuitoolkit.UbuntuUIToolkitCustomProxyObjectBase)

    def write_sandbox_desktop_file(self):
        desktop_file_dir = self.get_local_desktop_file_directory()
        desktop_file = self.get_named_temporary_file(
            suffix='.desktop', dir=desktop_file_dir)
        desktop_file.write('[Desktop Entry]\n')
        version, installed_path = self.get_installed_version_and_directory()
        filemanager_sandbox_exec = (
            'aa-exec-click -p com.ubuntu.filemanager_filemanager_{}'
            ' -- filemanager --forceAuth false'.format(version))
        desktop_file_dict = {
            'Type': 'Application',
            'Name': 'filemanager',
            'Exec': filemanager_sandbox_exec,
            'Icon': 'Not important',
            'Path': installed_path
        }
        for key, value in desktop_file_dict.items():
            desktop_file.write('{key}={value}\n'.format(key=key, value=value))
        desktop_file.close()
        logger.debug(filemanager_sandbox_exec)
        for key, value in desktop_file_dict.items():
            logger.debug("%s: %s" % (key, value))
        return desktop_file.name

    def get_local_desktop_file_directory(self):
        return os.path.join(
            self.real_home_dir, '.local', 'share', 'applications')

    def get_named_temporary_file(
            self, dir=None, mode='w+t', delete=False, suffix=''):
        # Discard files with underscores which look like an APP_ID to Unity
        # See https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1329141
        chars = tempfile._RandomNameSequence.characters.strip("_")
        tempfile._RandomNameSequence.characters = chars
        return tempfile.NamedTemporaryFile(
            dir=dir, mode=mode, delete=delete, suffix=suffix)

    def get_installed_version_and_directory(self):
        db = Click.DB()
        db.read()
        package_name = 'com.ubuntu.filemanager'
        registry = Click.User.for_user(db, name=os.environ.get('USER'))
        version = registry.get_version(package_name)
        directory = registry.get_path(package_name)
        return version, directory

    def patch_home(self):
        """ mock /home for testing purposes to preserve user data
        """

        # if running on non-phablet device,
        # run in temp folder to avoid mucking up home
        # bug 1316746
        # bug 1376423
        if self.test_type is not 'click':
            temp_dir_fixture = fixtures.TempDir()
            self.useFixture(temp_dir_fixture)
            temp_dir = temp_dir_fixture.path

            # before we set fixture, copy xauthority if needed
            self.useFixture(fixtures.EnvironmentVariable('HOME',
                                                         newvalue=temp_dir))

            logger.debug("Patched home to fake home directory %s" % temp_dir)

    def patch_environment(self, key, value):
        def patch_var(key, value):
            logging.info(
                "Setting environment variable '%s' to '%s'",
                key,
                value
            )

            os.environ[key] = value

        original = os.getenv(key)
        if original is None:
            original = ''
        patch_var(key, value)
        # on click, we should cleanup
        if self.test_type is 'click':
            self.addCleanup(patch_var, key, original)

    def _get_build_dir(self):
        """
        Returns the build dir after having parsed the CMake config file
        generated by Qt Creator. If it cannot find it or it cannot be parsed,
        an in-tree build is assumed and thus returned.
        """
        try:
            cmake_config = CMakePluginParser.CMakePluginParser(os.path.join(
                self.source_dir, 'CMakeLists.txt.user'))
            build_dir = cmake_config.active_build_dir
        except:
            build_dir = self.source_dir

        return build_dir


class FileManagerTestCase(BaseTestCaseWithPatchedHome):

    """Base test case that launches the filemanager-app."""

    def setUp(self):
        super(FileManagerTestCase, self).setUp()
        self.fakehome = os.getenv('HOME')
        self.original_file_count = \
            len([i for i in os.listdir(self.fakehome)
                 if not i.startswith('.')])
        logger.debug('Directory Listing for HOME\n%s' %
                     os.listdir(self.fakehome))
        logger.debug('File count in HOME is %s' % self.original_file_count)
        self.app = filemanager.Filemanager(self.launcher(), self.test_type)

    def make_file_in_home(self):
        return self.make_content_in_home('file')

    def make_directory_in_home(self):
        return self.make_content_in_home('directory')

    def make_content_in_home(self, type_):
        if type_ != 'file' and type_ != 'directory':
            raise ValueError('Unknown content type: "{0}"', type_)
        if type_ == 'file':
            temp_file = fm_fixtures.TemporaryFileInDirectory(self.fakehome)
            self.useFixture(temp_file)
            path = temp_file.path
        else:
            temp_dir = fm_fixtures.TemporaryDirectoryInDirectory(
                self.fakehome)
            self.useFixture(temp_dir)
            path = temp_dir.path

        # putting ignore_errors=True to avoid tests failing when cutting or
        # deleting file or directory has already canceled the file or directory
        self.addCleanup(shutil.rmtree, path, ignore_errors=True)

        logger.debug('Directory Listing for HOME\n%s' %
                     os.listdir(self.fakehome))
        self._assert_number_of_files(1)
        return path

    def _assert_number_of_files(self, expected_number_of_files, home=True):
        if home:
            expected_number_of_files += self.original_file_count
        folder_list_page = self.app.main_view.get_folder_list_page()
        self.assertThat(
            folder_list_page.get_number_of_files_from_list,
            Eventually(Equals(expected_number_of_files), timeout=60))
        self.assertThat(
            folder_list_page.get_number_of_files_from_header,
            Eventually(Equals(expected_number_of_files), timeout=60))
