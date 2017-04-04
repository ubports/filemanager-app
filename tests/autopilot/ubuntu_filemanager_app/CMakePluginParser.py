# -*- Mode: Python; coding: utf-8; indent-tabs-mode: nil; tab-width: 4 -*-
#
# Copyright (C) 2014 Canonical Ltd.
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
#
# Author:
#   David Planella <david.planella@ubuntu.com>

"""
This module parses a configuration file from the Qt Creator's CMake plugin and
enables programmatical read-only access to several of its configuration options
"""

import sys
from lxml import etree


class CMakePluginParseError(Exception):
    """
    Custom exception for errors during the parsing of a
    CMakeLists.txt.user file
    """
    def __init__(self, message):
        Exception.__init__(self, message)


class CMakePluginParser(object):
    """
    Parses a CMake plugin's config file and provides R/O access to its
    configuration options """

    def __init__(self, cmakelists_usr_file='CMakeLists.txt.user'):
        self.usr_file = cmakelists_usr_file

        try:
            self.info = etree.parse(self.usr_file)
        except:
            sys.stderr.write("Could not open the given " +
                             "CMakeLists.txt.user file: " + self.info)
            raise

    def _get_active_build_target(self):
        """
        Return the active build target from the current project in Qt Creator
        """

        try:
            active_build_target_nr = self.info.xpath(
                "./data/variable" +
                "[text()='ProjectExplorer.Project.ActiveTarget']" +
                "/../value")[0].text
        except:
            raise CMakePluginParseError("Could not find the active build " +
                                        "target in the CMake plugin's config")

        active_build_target = "ProjectExplorer.Project.Target." + \
            active_build_target_nr

        return active_build_target

    def _get_active_build_config(self, active_build_target):
        """Return the active build config from the active build targed"""

        try:
            active_build_config_nr = self.info.xpath(
                "./data/variable[text()='{0}']".format(active_build_target) +
                "/..//value[@key="
                "'ProjectExplorer.Target.ActiveBuildConfiguration']")[0].text
        except:
            raise CMakePluginParseError("Could not find the active build " +
                                        "target's active build config " +
                                        "in the CMake plugin's config")

        active_build_config = "ProjectExplorer.Target.BuildConfiguration." + \
            active_build_config_nr

        return active_build_config

    def _get_active_build_config_path(self):
        """Return the active build config's absolute path"""

        active_build_target = self._get_active_build_target()
        active_build_config = \
            self._get_active_build_config(active_build_target)

        try:
            active_build_config_node = self.info.xpath(
                "./data/variable[text()='{0}']".format(active_build_target) +
                "/..//valuemap[@key='{0}']".format(active_build_config))[0]
        except:
            raise CMakePluginParseError("Could not find the active " +
                                        "build config's node " +
                                        "in the CMake plugin's config")

        try:
            active_build_config_path = active_build_config_node.xpath(
                "./value[@key=" +
                "'ProjectExplorer.BuildConfiguration.BuildDirectory']")[0].text
        except:
            raise CMakePluginParseError("Could not find the active build " +
                                        "directory in the CMake plugin's " +
                                        "config")

        return active_build_config_path

    @property
    def active_build_dir(self):
        """Return the active build config's directory as an absolute path"""
        return self._get_active_build_config_path()
