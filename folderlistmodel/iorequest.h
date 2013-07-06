/*
 * Copyright (C) 2012 Robin Burchell <robin+nemo@viroteck.net>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * "Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Nemo Mobile nor the names of its contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 */

#ifndef IOREQUEST_H
#define IOREQUEST_H

#include <QHash>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QVector>

class IORequest : public QObject
{
    Q_OBJECT
public:   
    explicit IORequest();
    
public:
    enum RequestType
    {
        DirList,
        DirListExternalFSChanges
    };
    virtual void run() = 0;
    RequestType  type() const;
    
private:
    // hide this because IORequest should *NOT* be parented directly
    using QObject::setParent;

protected:
    RequestType  m_type;
};



class DirListWorker : public IORequest
{
    Q_OBJECT
public:
    explicit DirListWorker(const QString &pathName, QDir::Filter filter, const bool isRecursive);
    void run();
signals:
    void fetchingContents(QDateTime);
    void itemsAdded(const QVector<QFileInfo> &files);
    void workerFinished();

protected:
    QVector<QFileInfo>     getContents();

private:
    QVector<QFileInfo> add(const QString &pathName, QDir::Filter filter,
                           const bool isRecursive, QVector<QFileInfo> directoryContents);
private:
    QString       mPathName;
    QDir::Filter  mFilter;
    bool          mIsRecursive;
};



class  ExternalFileSystemChangesWorker : public DirListWorker
{
    Q_OBJECT
public:
    explicit ExternalFileSystemChangesWorker(const QVector<QFileInfo>& content,
                                      const QString &pathName,
                                      QDir::Filter filter,
                                      const bool isRecursive);
    void     run();
signals:
    void     removed(const QFileInfo&);
    void     changed(const QFileInfo&);
    void     added(const QFileInfo& );
    void     finished();
private:
    QHash<QString, QFileInfo>    m_curContent;   //!< using hash because the vector can be in any order
};



#endif // IOREQUEST_H
