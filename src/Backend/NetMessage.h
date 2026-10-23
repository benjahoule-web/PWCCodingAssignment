#pragma once

#include <chrono>
#include <format>

class NetMessage {

    private:
        uint64_t timestamp;
        std::string humanReadableTimestamp;
        std::string app;
        std::string message;

    public:
        NetMessage(uint64_t timestamp, std::string app, std::string message) {
            this->timestamp = timestamp;
            this->humanReadableTimestamp = std::format("{:%Y-%m-%d %H:%M:%S}", std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::from_time_t(timestamp))
            );
            this->app = app;
            this->message = message;
        }

        uint64_t getTimestamp() const { return timestamp; }
        std::string getHumanReadableTimestamp() const { return humanReadableTimestamp; }
        std::string getApp() const { return app; }
        std::string getMessage() const { return message; }
};