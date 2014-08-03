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
 * Author : Arto Jalkanen <ajalkane@gmail.com>
 */

#include "pamauthentication.h"

#include <QDebug>
#include <QtDBus/QtDBus>

#include <security/pam_appl.h>

#define UNITYGREETER_SERVICE "com.canonical.UnityGreeter"
#define UNITYGREETER_PATH "/list"
#define UNITYGREETER_INTERFACE "com.canonical.UnityGreeter"
#define UNITYGREETER_METHOD_ENTRY_IS_LOCKED "org.freedesktop.DBus.Properties.Get"
#define UNITYGREETER_METHOD_ENTRY_IS_LOCKED_ARG1 "com.canonical.UnityGreeter.List"
#define UNITYGREETER_METHOD_ENTRY_IS_LOCKED_ARG2 "EntryIsLocked"


PamAuthentication::PamAuthentication(QObject *parent) :
    QObject(parent)
{
    m_userLogin = qgetenv("USER");
}

PamAuthentication::~PamAuthentication() {

}

void
PamAuthentication::setServiceName(const QString &serviceName) {
    if (serviceName != m_serviceName) {
        m_serviceName = serviceName;
        emit serviceNameChanged();
    }
}

bool
PamAuthentication::requireAuthentication() {
    QDBusInterface dbus_iface(UNITYGREETER_SERVICE, UNITYGREETER_PATH,
                              UNITYGREETER_INTERFACE);

    qDebug() << Q_FUNC_INFO << "Querying if authentication required";
    QDBusReply<bool> reply = dbus_iface.call(UNITYGREETER_METHOD_ENTRY_IS_LOCKED,
                                             UNITYGREETER_METHOD_ENTRY_IS_LOCKED_ARG1,
                                             UNITYGREETER_METHOD_ENTRY_IS_LOCKED_ARG2);
    if (reply.isValid()) {
        bool replyValue = reply.value();
        qDebug() << Q_FUNC_INFO << "Return value" << replyValue;
        return replyValue;
    } else {
        qDebug() << Q_FUNC_INFO << "Failed getting value";
    }
    // By default be cautious and require authentication
    return true;
}

bool
PamAuthentication::validatePasswordToken(const QString &token) {
    pam_handle *pamHandle = 0;
    if (!initPam(&pamHandle)) {
        qDebug() << Q_FUNC_INFO << "Pam init failed";
        return false;
    }

    m_passwordToken = token;

    int status = pam_authenticate(pamHandle, 0);
    qDebug() << Q_FUNC_INFO << "Pam authenticate status" << status;

    pam_end(pamHandle, status);

    m_passwordToken.clear();

    return status == PAM_SUCCESS;
}

bool
PamAuthentication::initPam(pam_handle **pamHandle)
{
    pam_conv conversation;
    conversation.conv = ConversationFunction;
    conversation.appdata_ptr = static_cast<void *>(this);

    return pam_start(m_serviceName.toLatin1().data(), m_userLogin.toLatin1().data(),
                     &conversation, pamHandle) == PAM_SUCCESS;
}

int PamAuthentication::ConversationFunction(int num_msg,
                                            const pam_message **msg,
                                            pam_response **resp,
                                            void* appdata_ptr)
{
    if (num_msg <= 0) {
        return PAM_CONV_ERR;
    }

    *resp = static_cast<pam_response*>(calloc(num_msg, sizeof(pam_response)));

    PamAuthentication *self = static_cast<PamAuthentication*>(appdata_ptr);

    for (int count = 0; count < num_msg; ++count) {
        switch (msg[count]->msg_style) {
        case PAM_PROMPT_ECHO_ON:
        {
            qDebug() << Q_FUNC_INFO << "PAM_PROMPT_ECHO_ON received";
            resp[count]->resp = strdup(self->m_passwordToken.toLatin1().data());
            resp[count]->resp_retcode = 0;
            break;
        }
        case PAM_PROMPT_ECHO_OFF:
        {
            qDebug() << Q_FUNC_INFO << "PAM_PROMPT_ECHO_OFF received";
            resp[count]->resp = strdup(self->m_passwordToken.toLatin1().data());
            resp[count]->resp_retcode = 0;
            break;
        }
        case PAM_TEXT_INFO:
        {
            QString message(msg[count]->msg);
            qDebug() << Q_FUNC_INFO << "PAM_TEXT_INFO received" << message;
            break;
        }
        case PAM_AUTHTOK:
        {
            qDebug() << Q_FUNC_INFO << "PAM_AUTHTOK received";
            break;
        }
        default:
        {
            qDebug() << Q_FUNC_INFO << "Other PAM msg received: " << msg[count]->msg_style;
        }
        }
    }

    return PAM_SUCCESS;
}
