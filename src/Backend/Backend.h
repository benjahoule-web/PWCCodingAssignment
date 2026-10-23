#pragma once

#include <atomic>
#include <deque>
#include <mutex>
#include <thread>
#include <httplib.h>
#include "JSON.h"
#include "NetMessage.h"

class Backend 
{
    public:
        Backend(); //Sets up the net object and the message parsing
        ~Backend();
        std::deque<NetMessage> getNetMessages();
        void setUpdateUICallback(std::function<void()> callback) { updateUI = callback; }
        void flushRecentMessages();

    private:
        void setupPostHandler();
        void handlePost(const httplib::Request& request, httplib::Response& response);
        bool handleNewJSON(JSONObject& json);
        void logMessage(const NetMessage& message);
        void logDebugMessage(std::string message);

        httplib::Server server;
        std::deque<JSONObject> jsonObjects;
        std::deque<NetMessage> netMessages;
        std::mutex messagesMutex;
        std::thread serverListeningThread;
        std::atomic<bool> stopListening = false;

        std::function<void()> updateUI;
};