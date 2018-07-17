/*
 * Copyright (C) 2018 Konsulko Group
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LOAD_H
#define LOAD_H

#include <QDebug>
#include <QObject>

#include "messageengine.h"

class Load : public QObject
{
    Q_OBJECT

    public:
        explicit Load(QUrl &url, QObject * parent = Q_NULLPTR);
        virtual ~Load();

        Q_PROPERTY(double loadAverage READ loadAverage NOTIFY loadAverageChanged)

        double loadAverage() const { return m_loadAverage; }

    signals:
        void loadAverageChanged(double value);

    private:
        MessageEngine *m_mloop;
        double m_loadAverage;

        // slots
        void onConnected();
        void onDisconnected();
        void onMessageReceived(MessageType, Message*);

        const QStringList events {
            "load",
        };
};

#endif // LOAD_H
