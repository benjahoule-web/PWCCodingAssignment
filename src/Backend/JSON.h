#pragma once

#include <stdio.h>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>


class JSONValue
{
public:
    JSONValue();
    double numberValue = 0;
    std::string stringValue = "";
    bool boolValue = false;
};


class JSONObject
{
public:
    JSONObject();
    JSONObject(std::string jsonString);
    ~JSONObject();

    enum DataType
    {
        JSON_NULL,
        JSON_NUMBER,
        JSON_STRING,
        JSON_BOOL,
        JSON_ARRAY,
        JSON_OBJECT
    };
    bool isNull() {return type == JSON_NULL;}
    bool isValid() {return valid;}
    bool isNumber() {return type == JSON_NUMBER;}
    bool isString() {return type == JSON_STRING;}
    bool isBool() {return type == JSON_BOOL;}
    bool isArray() {return type == JSON_ARRAY;}
    bool isObject() {return type == JSON_OBJECT;}
    DataType getType() {return type;}

    double getNumberValue() {return value.numberValue;}
    std::string getStringValue() {return value.stringValue;}
    bool getBoolValue() {return value.boolValue;}
    JSONObject* getArrayValue(int index) {return array[index];}
    JSONObject* getChildObject(std::string key);

    void setNumberValue(double newValue);
    void setStringValue(std::string newValue);
    void setBoolValue(bool newValue);
    void setArray(std::vector<JSONObject*> newArray);
    void addToArray(JSONObject* newArrayPoint);
    void addChild(std::string key, JSONObject* newChild);
    
    bool parse(std::string jsonString);
    std::string print(bool prettyPrint = false, int tabSize = 2, int tabLevel = 0);

private:
    bool valid = false;
    DataType type = JSON_NULL;
    std::map<std::string, JSONObject*> children;
    std::vector<JSONObject*> array;
    JSONValue value;

    bool cleanUpString(std::string &stringToClean); //ret false if string ends up empty, true otherwise
    size_t getOut(std::string &inputString, size_t startIndex);
    size_t findEndOfString(std::string inputString, size_t startIndex);
    std::pair<size_t,bool> findNextSeparator(std::string &inputString, char endChar, size_t startIndex);
    std::vector<std::string> separateJSONElements(std::string inputString);
    std::pair<std::string,std::string> splitKeyValuePair(std::string& inputString);
    void replaceAll(std::string& source, const std::string from, const std::string to);
    void escapeCharacters(std::string& stringToEscape);
    void unescapeCharacters(std::string& stringToUnescape);

    
    void logMessage(std::string message)
    {
        std::ofstream logFile;
        logFile.open("JSONLog.csv", std::ios_base::app);
        logFile << message << std::endl;
    }

};

