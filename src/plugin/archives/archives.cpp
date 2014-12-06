/*
 * Copyright (C) 2014 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author : Niklas Wenzel <nikwen.developer@gmail.com>
 */

#include "archives.h"
#include "QDebug"

void Archives::extractZip(const QString path, const QString destination)
{
    if (_process != nullptr && _process->state() == QProcess::ProcessState::Running) {
        return; // Do not allow two extractions running in parallel. Due to the way this is used in QML parallelization is not needed.
    }

    QString program = "unzip"; // This programm is available in the images as it is one of the dependencies of the ubuntu-download-manager package.
    QStringList arguments;
    arguments << path << "-d" << destination;

    _process = new QProcess(this);

    // Connect to internal slots in order to have one unified onFinished slot handling both events for QML.
    connect(_process,
            static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>
            (&QProcess::finished), this, &Archives::_onFinished);
    connect(_process,
            static_cast<void(QProcess::*)(QProcess::ProcessError)>
            (&QProcess::error), this, &Archives::_onError);

    _process->start(program, arguments);
}

void Archives::_onError(QProcess::ProcessError error)
{
    emit finished(false, error);
}

void Archives::_onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if ((exitStatus == QProcess::NormalExit || exitCode == 0) && _process->readAllStandardError().isEmpty()) {
        emit finished(true, -1);
    } else {
        emit finished(false, -1);
    }
}
