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

import logging
import os
import shutil
import tempfile

import autopilot.logging
import fixtures


logger = logging.getLogger(__name__)


class TemporaryFileInHome(fixtures.Fixture):
    """Create a temporafy file in the home directory."""

    def setUp(self):
        super(TemporaryFileInHome, self).setUp()
        _, self.path = tempfile.mkstemp(
            prefix='tmpfm', dir=os.environ.get('HOME'))
        logger.debug('Created temporary file in HOME: {}'.format(self.path))
        self.addCleanup(self.delete_file, self.path)

    @autopilot.logging.log_action(logger.info)
    def delete_file(self, path):
        """Delete a file, if it exists."""
        if os.path.exists(path):
            logger.debug('Deleting file.')
            os.remove(path)
        else:
            logger.debug('File does not exist.')


class TemporaryDirectoryInHome(fixtures.Fixture):
    """Create a temporary directory in the home directory."""

    def setUp(self):
        super(TemporaryDirectoryInHome, self).setUp()
        self.path = tempfile.mkdtemp(
            prefix='tmpfm', dir=os.environ.get('HOME'))
        logger.debug(
            'Created temporary directory in HOME: {}'.format(self.path))
        self.addCleanup(self.delete_directory, self.path)

    @autopilot.logging.log_action(logger.info)
    def delete_directory(self, path):
        """Delete a directory, if it exists."""
        if os.path.exists(path):
            logger.debug('Deleting directory.')
            shutil.rmtree(path)
        else:
            logger.debug('Directory does not exist.')
