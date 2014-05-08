# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

"""Filemanager app autopilot tests."""

import os
import shutil
import logging

import fixtures
from filemanager import emulators

from autopilot.input import Mouse, Touch, Pointer
from autopilot.platform import model
from autopilot.testcase import AutopilotTestCase
from autopilot import logging as autopilot_logging

from ubuntuuitoolkit import (
    emulators as toolkit_emulators,
    fixture_setup as toolkit_fixtures
)

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
    local_location_qml = local_location + "/filemanager.qml"
    installed_location_qml = "/usr/share/filemanager/qml/filemanager.qml"
    local_location_binary = os.path.join(local_location, 'src/app/filemanager')
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

    @autopilot_logging.log_action(logger.info)
    def launch_test_local(self):
        self.useFixture(fixtures.EnvironmentVariable(
            'QML2_IMPORT_PATH', newvalue=os.path.join(self.build_dir,
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
        """ Copy .Xauthority file to directory, if it exists in /home
        """
        xauth = os.path.expanduser(os.path.join('~', '.Xauthority'))
        if os.path.isfile(xauth):
            logger.debug("Copying .Xauthority to " + directory)
            shutil.copyfile(
                os.path.expanduser(os.path.join('~', '.Xauthority')),
                os.path.join(directory, '.Xauthority'))

    def _patch_home(self):
        """ mock /home for testing purposes to preserve user data
        """
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

        logger.debug("Patched home to fake home directory " + temp_dir)

        return temp_dir

    @property
    def main_view(self):
        return self.app.wait_select_single(emulators.MainView)
