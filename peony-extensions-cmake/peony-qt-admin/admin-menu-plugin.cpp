/*
 * Peony-Qt's Library
 *
 * Copyright (C) 2019, Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Yue Lan <lanyue@kylinos.cn>
 * Authors: Meihong <hemeihong@kylinos.cn>
 *
 */

#include "admin-menu-plugin.h"
#include "file-info.h"

#include <QProcess>
#include <QtConcurrent>
#include <QUrl>
#include <QTranslator>
#include <QApplication>
#include <QFile>

#include <QDebug>

using namespace Peony;

AdminMenuPlugin::AdminMenuPlugin(QObject *parent) : QObject(parent)
{
    QTranslator *t = new QTranslator(this);

    bool b_load = t->load(":/translations/peony-qt-admin-extension_"+QLocale::system().name());
    qDebug()<<"\n\n\n\n\n\n\n  AdminMenuPlugin translate:"<<b_load;
    QFile file(":/translations/peony-qt-admin-extension_"+QLocale::system().name()+".ts");
    qDebug()<<"file:"<<file.exists();
    QApplication::installTranslator(t);
}

QList<QAction *> AdminMenuPlugin::menuActions(Types types, const QString &uri, const QStringList &selectionUris)
{
    QList<QAction *> l;
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    if (selectionUris.isEmpty()) {
        auto directoryAction = new QAction(tr("Open Directory as Admin"));
        l<<directoryAction;
        directoryAction->connect(directoryAction, &QAction::triggered, [=](){
            QtConcurrent::run([=](){
                QProcess p;
                QUrl url = uri;
                p.setProgram("pkexec");
                p.setArguments(QStringList()<<"peony"<<url.toEncoded());
                p.start();
                p.waitForFinished();
            });
        });
    } else if (selectionUris.count() == 1) {
        auto info = FileInfo::fromUri(selectionUris.first());
        //special type mountable, return
        qDebug()<<"info isVirtual:"<<info->isVirtual()<<info->mimeType();
        if (selectionUris.first().startsWith("computer:///") || 
            selectionUris.first().startsWith("trash:///")|| 
            info->isVirtual())
            return l;
        if (info->isDir()) {
            auto directoryAction = new QAction(tr("Open Directory as Admin"));
            l<<directoryAction;
            directoryAction->connect(directoryAction, &QAction::triggered, [=](){
                QtConcurrent::run([=](){
                    QProcess p;
                    QUrl url = selectionUris.first();
                    p.setProgram("pkexec");
                    p.setArguments(QStringList()<<"peony"<<url.toEncoded());
                    p.start();
                    p.waitForFinished();
                });
            });
        }
        if (info->mimeType().startsWith("text")) {
            auto directoryAction = new QAction(tr("Open Text as Admin"));
            l<<directoryAction;
            directoryAction->connect(directoryAction, &QAction::triggered, [=](){
                QtConcurrent::run([=](){
                    QProcess p;
                    QUrl url = selectionUris.first();
                    p.setProgram("pkexec");
                    p.setArguments(QStringList()<<"pluma"<<url.toEncoded());
                    p.start();
                    p.waitForFinished();
                });
            });
        }
    }
#else
#endif
    return l;
}
