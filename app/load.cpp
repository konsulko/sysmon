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

#include "load.h"
#include "message.h"
#include "messageengine.h"

Load::Load (QUrl &url, QObject * parent) :
    QObject(parent),
    m_mloop(nullptr),
    m_loadAverage(0)
{
    m_mloop = new MessageEngine(url);

    QObject::connect(m_mloop, &MessageEngine::connected, this, &Load::onConnected);
    QObject::connect(m_mloop, &MessageEngine::disconnected, this, &Load::onDisconnected);
    QObject::connect(m_mloop, &MessageEngine::messageReceived, this, &Load::onMessageReceived);
}

Load::~Load()
{
    delete m_mloop;
}

void Load::onMessageReceived(MessageType type, Message *msg)
{
    if (msg->isEvent() && (msg->eventName() == "load")) {
        // If a load event, parse value and update property
        QJsonObject loadEventData = msg->eventData();
        m_loadAverage = loadEventData.value("value").toDouble();
        emit loadAverageChanged(m_loadAverage);
    } else if (msg->isReply()) {
        // If a load reply, parse value and update property
        m_loadAverage = msg->replyData().value("value").toDouble();
        emit loadAverageChanged(m_loadAverage);
    }
    msg->deleteLater();
}

void Load::onConnected()
{
    QStringListIterator eventIterator(events);
    Message *msg;

    // Immediately get current load value
    msg = new Message();
    QJsonObject parameter;
    msg->createRequest("load", "load", parameter);
    m_mloop->sendMessage(msg);
    msg->deleteLater();

    // Subscribe to load events
    while (eventIterator.hasNext()) {
        msg = new Message();
        QJsonObject parameter;
        parameter.insert("value", eventIterator.next());
        msg->createRequest("load", "subscribe", parameter);
        m_mloop->sendMessage(msg);
        msg->deleteLater();
    }
}

void Load::onDisconnected()
{
    QStringListIterator eventIterator(events);
    Message *msg;

    // Unsubscribe from load events
    while (eventIterator.hasNext()) {
        msg = new Message();
        QJsonObject parameter;
        parameter.insert("value", eventIterator.next());
        msg->createRequest("load", "unsubscribe", parameter);
        m_mloop->sendMessage(msg);
        msg->deleteLater();
    }
}
