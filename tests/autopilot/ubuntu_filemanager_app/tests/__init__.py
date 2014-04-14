# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
# Copyright 2013 Canonical
#
# This program is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 3, as published
# by the Free Software Foundation.

"""Filemanager app autopilot tests."""

import os.path
import shutil
import tempfile
import logging

import fixtures
import ubuntu_filemanager_app

from autopilot.input import Mouse, Touch, Pointer
from autopilot.platform import model
from autopilot.testcase import AutopilotTestCase
from autopilot import logging as autopilot_logging

from ubuntuuitoolkit import (
    base,
    emulators as toolkit_emulators
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

    local_location_qml = '../../ubuntu-filemanager-app.qml'
    local_location_binary = '../../src/app/filemanager'
    installed_location_qml = '/usr/share/ubuntu-filemanager-app/' \
                         'ubuntu-filemanager-app.qml'
    installed_location_binary = '/usr/bin/filemanager-app'

    def setUp(self):
        self._create_test_root()
        self.pointing_device = Pointer(self.input_device_class.create())
        super(FileManagerTestCase, self).setUp()

        if os.path.exists(self.local_location_binary):
            app_proxy = self.launch_test_local()
        elif os.path.exists(self.installed_location_binary):
            app_proxy = self.launch_test_installed()
        else:
            app_proxy = self.launch_test_click()

        self.app = ubuntu_filemanager_app.FileManagerApp(app_proxy)

        #turn off the OSK so it doesn't block screen elements
        if model() != 'Desktop':
            os.system('stop maliit-server')
            self.addCleanup(os.system, 'start maliit-server')

        self.original_file_count = \
            len([i for i in os.listdir(os.environ['TESTHOME'])
                 if not i.startswith('.')])
        logger.debug('Directory Listing for TESTHOME\n%s' %
                     os.listdir(os.environ['TESTHOME']))
        logger.debug('File count in TESTHOME is %s' % self.original_file_count)
        launch()

    def _create_test_root(self):
        #create a temporary directory for testing purposes
        #due to security lockdowns, make it under /home always
        temp_dir = tempfile.mkdtemp(dir=os.path.expanduser('~'))
        self.addCleanup(shutil.rmtree, temp_dir)
        logger.debug('Created root test directory ' + temp_dir)
        self.patch_environment('TESTHOME', temp_dir)
        logger.debug('Patched root test directory ' + temp_dir)
        return temp_dir

    @autopilot_logging.log_action(logger.info)
    def launch_test_local(self):
        self.useFixture(fixtures.EnvironmentVariable(
            'QML2_IMPORT_PATH', newvalue='../../src/plugin'))
        return self.launch_test_application(
            self.local_location_binary,
            app_type='qt',
            emulator_base=toolkit_emulators.UbuntuUIToolkitEmulatorBase)

    @autopilot_logging.log_action(logger.info)
    def launch_test_installed(self):
        self.app = self.launch_test_application(
            base.get_qmlscene_launch_command(),
            self.installed_location_qml,
            app_type='qt',
            emulator_base=toolkit_emulators.UbuntuUIToolkitEmulatorBase)

    @autopilot_logging.log_action(logger.info)
    def launch_test_click(self):
        self.app = self.launch_click_package(
            'com.ubuntu.filemanager',
            emulator_base=toolkit_emulators.UbuntuUIToolkitEmulatorBase)
