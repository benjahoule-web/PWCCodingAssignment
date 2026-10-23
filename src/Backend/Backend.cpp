#include "Backend.h"
#include <iostream>
#include <fstream>
#include <stdexcept>

Backend::Backend()
{
    setupPostHandler();
    if (!server.bind_to_port("localhost", 8080))
    {
        throw std::runtime_error("Failed to bind server to localhost:8080");
    }
    serverListeningThread = std::thread([this]() {
        if (!server.listen_after_bind() && !stopListening.load())
        {
            std::cerr << "HTTP server stopped unexpectedly" << std::endl;
        }
    });
}

Backend::~Backend()
{
    stopListening.store(true); //Make the thread stop
    server.stop();
    if (serverListeningThread.joinable()) // Wait for thread to stop
    {
        serverListeningThread.join();
    }
}

std::deque<NetMessage> Backend::getNetMessages()
{
    std::lock_guard<std::mutex> lock(messagesMutex);
    return netMessages;
}

void Backend::handlePost(const httplib::Request& request, httplib::Response& response)
{
    logDebugMessage("Received POST request with body: " + request.body);

    JSONObject json(request.body);
    if(handleNewJSON(json)) //Could move the processing of the JSON to a separate thread to allow more requests to be processed while the current one is being handled but should be OK
    {
        response.status = 200;
        response.set_content(request.body, "text/plain");
    }
    else
    {
        response.status = 400;
        response.set_content("Invalid JSON", "text/plain");
        logDebugMessage("Failed to parse JSON.");
    }
}

void Backend::setupPostHandler()
{
    server.set_pre_routing_handler([this](const httplib::Request& request, httplib::Response& response) {
        if (request.method != "POST" ||
            (!request.has_header("Content-Length") &&
            !request.has_header("Transfer-Encoding")))
        {
            logDebugMessage("Rejected a bad request. Method: " + request.method);
            response.status = 400;
            response.set_content("Server only accepts POST requests with a body", "text/plain");
            return httplib::Server::HandlerResponse::Handled;
        }

        return httplib::Server::HandlerResponse::Unhandled;
    });

    server.Post("/", [this](const httplib::Request& request, httplib::Response& response) {
        handlePost(request, response);
    });
}

bool Backend::handleNewJSON(JSONObject& json)
{
    if(json.isValid())
    {
        if(json.isObject())
        {
            int validData = 0;
            uint64_t timestamp = 0;
            if(json.getChildObject("timestamp"))
            {
                timestamp = json.getChildObject("timestamp")->getNumberValue();
                validData++;
            }
            std::string app;
            if(json.getChildObject("app"))
            {
                app = json.getChildObject("app")->getStringValue();
                validData++;
            }
            std::string message;
            if(json.getChildObject("message"))
            {
                message = json.getChildObject("message")->getStringValue();
                validData++;
            }

            if(validData != 3)
            {
                logDebugMessage("Parsed a valid but incomplete JSON.");
                return false;
            }

            NetMessage newMessage(timestamp, app, message);
            {
                std::lock_guard<std::mutex> lock(messagesMutex);
                netMessages.push_front(newMessage);
            }
            logMessage(newMessage);
            logDebugMessage("Parsed JSON successfully! Timestamp: " + std::to_string(json.getChildObject("timestamp")->getNumberValue()) + 
                                                     ", App: " + json.getChildObject("app")->getStringValue() + 
                                                 ", Message: " + json.getChildObject("message")->getStringValue());
            //notify dashboard that a new message has been received
            if(updateUI)
            {
                updateUI();
            }
            return true;
        }
    }

    return false;
}

void Backend::flushRecentMessages()
{
    std::lock_guard<std::mutex> lock(messagesMutex);
    netMessages.clear();
    logDebugMessage("Flushed recent messages.");
}

void Backend::logMessage(const NetMessage& message)
{
    std::ofstream logFile;
    logFile.open("MessageLog.csv", std::ios_base::app);
    logFile << message.getTimestamp() << "," << message.getHumanReadableTimestamp() << "," << message.getApp() << "," << message.getMessage() << std::endl;
}

void Backend::logDebugMessage(std::string message)
{
    std::ofstream logFile;
    logFile.open("DebugLog.txt", std::ios_base::app);
    logFile << message << std::endl;

    return;
}
