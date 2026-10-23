#include "JSON.h"

JSONValue::JSONValue()
{
    //Nothing to do, will init with nothing
}

JSONObject::JSONObject()
{
    //Nothing to do, will init with nothing and type "NULL"
}

JSONObject::JSONObject(std::string jsonString)
{
    valid = parse(jsonString);
}

JSONObject::~JSONObject()
{
    for(size_t i = 0 ; i < array.size() ; i++ )
    {
        delete array[i];
    }
    
    for ( const auto &p : children )
    {
        delete p.second;
    }
}

JSONObject* JSONObject::getChildObject(std::string key)
{
    if(children.contains(key)) return children[key];
    else 
    {
        return nullptr;
    }
}

void JSONObject::setNumberValue(double newValue) 
{
    value.numberValue = newValue; 
    type = JSON_NUMBER;
}

void JSONObject::setStringValue(std::string newValue) 
{
    value.stringValue = newValue; 
    type = JSON_STRING;
}

void JSONObject::setBoolValue(bool newValue) 
{
    value.boolValue = newValue; 
    type = JSON_BOOL;
}

void JSONObject::setArray(std::vector<JSONObject*> newArray) 
{
    array.swap(newArray); 
    type = JSON_ARRAY;
}

void JSONObject::addToArray(JSONObject* newArrayPoint) 
{
    array.push_back(newArrayPoint); 
    type = JSON_ARRAY;
}

void JSONObject::addChild(std::string key, JSONObject* newChild) 
{
    children[key] = newChild; 
    type = JSON_OBJECT;
}

bool JSONObject::parse(std::string jsonString)
{
    if(!cleanUpString(jsonString)) //Also checks if string is empty
    {
        type = JSON_NULL;
        valid = false;
        return false;
    }

    char firstChar = jsonString.front();
    if (firstChar == '"') //String
    {
        
        if(jsonString == "\"\"")
        {         
            value.stringValue = "";
            value.boolValue = false;
            value.numberValue = 0;
            type = JSON_STRING;
            valid = true;
            return true;
        }
        //If too small, or does not end with ", or ends with an escaped "
        else if(jsonString.size() < 3 || jsonString.back() != '"' || (jsonString[jsonString.size()-2] == '\\' && jsonString[jsonString.size()-3] != '\\'))
        {
            type = JSON_NULL;
            value.stringValue = "";
            value.boolValue = false;
            value.numberValue = 0;
            valid = false;
            return false;
        }
        else //String starts and ends with not-escaped ", save what's inside.
        { 
            //Save string
            value.stringValue = jsonString.substr(1,jsonString.size()-2);
            unescapeCharacters(value.stringValue);
            //set type
            type = JSON_STRING;
            valid = true;

            //Check if it could be a number instead or bool
            if(jsonString == "true")
            {
                value.boolValue = true;
                type = JSON_BOOL;
                return true;
            }
            else if(jsonString == "false")
            {
                value.boolValue = false;
                type = JSON_BOOL;
                return true;
            }
            else if(jsonString == "null")
            {
                type = JSON_NULL;
                return true;
            }
            else
            {
                try
                {
                    size_t pos;
                    value.numberValue = std::stod(value.stringValue, &pos);
                    type = JSON_NUMBER;
                }
                catch(...)
                {
                    value.numberValue = 0;
                }
                
                return true;
            }
        }
    }
    else if (firstChar == '[') //Array
    {
        if(jsonString.back() != ']')
        {
            type = JSON_NULL;
            valid = false;
            return false;
        }
        else if(jsonString.size() == 2)
        {            
            array.clear();
            type = JSON_ARRAY;
            valid = true;
            return true;
        }

        //separate by , then put into array
        std::vector<std::string> items = separateJSONElements(jsonString);

        for(int index = 0 ; index < items.size() ; index++)
        {
            JSONObject* newJSONObject = new JSONObject(items[index]);
            if(newJSONObject && newJSONObject->isValid())
            {
                array.push_back(newJSONObject);
            }
        }

        type = JSON_ARRAY;
        valid = true;
        return true;
    }
    else if (firstChar == '{') //Object
    {
        if(jsonString.back() != '}')
        {
            type = JSON_NULL;
            valid = false;
            return false;
        }
        else if(jsonString.size() == 2)
        {
            type = JSON_OBJECT;
            valid = true;
            return true;
        }

        //separate by , then put into array
        std::vector<std::string> items = separateJSONElements(jsonString);

        valid = true;
        for(int index = 0 ; index < items.size() ; index++)
        {
            std::pair<std::string, std::string> keyValuePair = splitKeyValuePair(items[index]);
            JSONObject* newJSONObject = new JSONObject(keyValuePair.second);
            if(newJSONObject && newJSONObject->isValid())
            {
                addChild(keyValuePair.first, newJSONObject) ;
            }
            else
            {
                valid = false;
                return false;
            }
        }

        type = JSON_OBJECT;
        return true;
    }
    else //double, bool or null
    {
        if(jsonString == "true")
        {
            value.boolValue = true;
            type = JSON_BOOL;
            valid = true;
            return true;
        }
        else if(jsonString == "false")
        {
            value.boolValue = false;
            type = JSON_BOOL;
            valid = true;
            return true;
        }
        else if(jsonString == "null")
        {
            type = JSON_NULL;
            valid = true;
            return true;
        }
        else
        {
            try
            {
                size_t pos;
                value.numberValue = std::stod(jsonString, &pos);
                type = JSON_NUMBER;
                valid = true;
                return true;
            }
            catch(...)
            {
                type = JSON_NULL;
                valid = false;
                return false;
            }
        }
    }
}

bool JSONObject::cleanUpString(std::string &stringToClean)
{
    if(stringToClean.empty()) return false;

    //Remove Beginning whitespaces
    while(stringToClean.front() == ' ' || stringToClean.front() == '\n' || stringToClean.front() == '\r' || stringToClean.front() == '\t')
    {
        stringToClean.erase(0,1);

        if(stringToClean.empty())
        {
            return false;
        }
    }

    //Remove ending whitespaces
    while(stringToClean.back() == ' ' || stringToClean.back() == '\n' || stringToClean.back() == '\r' || stringToClean.back() == '\t')
    {
        stringToClean.pop_back();
        
        if(stringToClean.empty())
        {
            return false;
        }
    }
    return true;
}

//separates every string by ',' puts the resulting strings without the ',' in a vector.
std::vector<std::string> JSONObject::separateJSONElements(std::string inputString)
{
    std::vector<std::string> output;
    char lookingFor;
    if(inputString.empty()) return output;
    if(inputString[0] == '{') lookingFor = '}';
    else if(inputString[0] == '[') lookingFor = ']';
    else return output;

    size_t cursor = 1;
    while(cursor < inputString.size())
    {
        std::pair<size_t,bool> nextSeparatorCursor = findNextSeparator(inputString, lookingFor, cursor);
        if(nextSeparatorCursor.first == 0)
        {
            //error occured. Return what was parsed if any
            return output;
        }
        output.push_back(inputString.substr(cursor, nextSeparatorCursor.first - cursor)); //save string bewteen cursor and separator.

        cursor = nextSeparatorCursor.first + 1; //Save next cursor position

        if (nextSeparatorCursor.second)
        { 
            //end of structure is reached. Return.
            return output;
        }
    }

    return output;
}

// startIndex needs to point to the opoening  '"' or a string
size_t JSONObject::findEndOfString(std::string inputString, size_t startIndex)
{   
    if(startIndex > inputString.size() - 2 || inputString[startIndex] != '"') //If size is too small, or if no string starts where specified.
    {
        return 0;
    }
    //If too small, or does not end with ", or ends with an escaped "
    size_t index = startIndex + 1;
    while(index < inputString.size())
    {
        size_t strMark = inputString.find_first_of('"',index);
        if(strMark == inputString.npos)
        {
            //Ending not found, error
            return 0;
        }
        if(inputString[strMark-1] != '\\' || inputString[strMark-2] == '\\') //ending char was not escaped
        {
            return strMark;
        }
        //start again with updated search index
        index = strMark+1;
    }
    return 0;
}

//startIndex should point to the opening bracket or "
size_t JSONObject::getOut(std::string& inputString, size_t startIndex)
{
    if(inputString[startIndex] == '"') return findEndOfString(inputString, startIndex);
    char lookingFor;
    if(inputString[startIndex] == '{') lookingFor = '}';
    else if(inputString[startIndex] == '[') lookingFor = ']';
    else return 0; //Nothing to get out of

    bool out = false;
    size_t index = startIndex + 1;
    while(index < inputString.size())
    {
        if((inputString[index] == '"' || inputString[index] == '{' || inputString[index] == '[') && index < inputString.size()-1 )
        {
            index = getOut(inputString, index); //sets the index to the character right after the end of the structure
            if(index == 0) return 0; // check it here because the while can't do it since we start at index 0;
        }
        else if (inputString[index] == lookingFor)
        {
            return index;
        }

        index++; //Go to next character
    }
    return 0;
}

std::pair<size_t,bool> JSONObject::findNextSeparator(std::string& inputString, char endChar, size_t startIndex)
{
    size_t index = startIndex;
    while(index < inputString.size())
    {
        if((inputString[index] == '"' || inputString[index] == '{' || inputString[index] == '[') && index < inputString.size()-1 )
        {
            index = getOut(inputString, index); //sets the index to the character right after the end of the structure
            if(index == 0) return std::pair<size_t,bool>(0,false); // check it here because the while can't do it since we start at index 0;
        }
        else if(inputString[index] == endChar || inputString[index] == ',')
        {
            return std::pair<size_t,bool>(index,inputString[index] == endChar);
        }
        index++; //Regular character or whitespace, ignore and go until you find a structure to jump, or a separator, or the end character
    }
    return std::pair<size_t,bool>(0,false);
}

std::pair<std::string,std::string> JSONObject::splitKeyValuePair(std::string& inputString)
{
    std::pair<std::string,std::string> outputPair;
    size_t separator = inputString.find_first_of(':');
    if(separator == inputString.npos) return outputPair;

    size_t keyIndex = inputString.find_first_of('"');
    size_t endOfKeyIndex = findEndOfString(inputString, keyIndex);
    if(keyIndex == inputString.npos || endOfKeyIndex == 0 || (endOfKeyIndex-keyIndex-1) < 1 ) return outputPair;

    outputPair.first = inputString.substr(keyIndex+1, endOfKeyIndex-keyIndex-1);
    outputPair.second = inputString.substr(separator+1);
    return outputPair;
}


std::string JSONObject::print(bool prettyPrint, int tabSize, int tabLevel)
{
    std::string outputString;
    std::string stringValue;
    if(prettyPrint)
    {
        outputString.append(tabSize*(tabLevel+1), ' ');
    }
    switch(type)
    {
        case JSON_NULL:
            outputString = "null"; // Might remove
        break;
        case JSON_NUMBER:
            outputString = std::to_string(value.numberValue);
        break;
        case JSON_STRING:
            stringValue = value.stringValue;
            escapeCharacters(stringValue);
            outputString = '"' + stringValue + '"';
        break;
        case JSON_BOOL:
            if (value.boolValue) outputString = "true";
            else outputString = "false";
        break;
        case JSON_ARRAY:
            //Open bracket, 
            outputString = "[";
            //then loop through every item in array and call print, add a comma at the end of each except the last, then close bracket
            for(size_t i = 0 ; i < array.size() ; i++)
            {
                if(prettyPrint) //Then add newline and the tabs
                {
                    outputString += "\n";
                    outputString.append(tabSize*(tabLevel+1), ' ');
                }
                outputString += array[i]->print(prettyPrint, tabSize, tabLevel+1);
                
                if(i != array.size() -1 )
                {
                    outputString += ",";
                }
            }
            if(prettyPrint) 
            {
                outputString += "\n";
                outputString.append(tabSize*tabLevel, ' ');
            }
            outputString += "]";
        break;
        case JSON_OBJECT:
            //Open curly bracket, then loop through every item in children and print "key": then call print on object, add a comma at the end of each except the last, then close curly bracket
                    //Open bracket, 
            outputString = "{";
            //then loop through every item in array and call print, add a comma at the end of each except the last, then close bracket
            std::map<std::string,JSONObject*>::iterator child;
            for (child = children.begin(); child != children.end(); child++)
            {
                if(prettyPrint) //Then add newline and the tabs
                {
                    outputString += "\n";
                    outputString.append(tabSize*(tabLevel+1), ' ');
                }

                outputString += '"' + child->first + '"';
                if(prettyPrint) outputString += " : ";
                else outputString += ":";
                outputString += child->second->print(prettyPrint, tabSize, tabLevel+1);
                
                if(child != std::prev(children.end()))
                {
                    outputString += ",";
                }
            }
            if(prettyPrint) 
            {
                outputString += "\n";
                outputString.append(tabSize*tabLevel, ' ');
            }
            outputString += "}";
        break;
    }

    return outputString;
}

void JSONObject::replaceAll(std::string& source, const std::string from, const std::string to)
{   //Should allow multiple pairs of from/to to be replaced in one call, but for now this is fine.
    std::string newString;
    newString.reserve(source.length());  // avoids a few memory allocations

    std::string::size_type lastPos = 0;
    std::string::size_type findPos;

    while(std::string::npos != (findPos = source.find(from, lastPos)))
    {
        newString.append(source, lastPos, findPos - lastPos);
        newString += to;
        lastPos = findPos + from.length();
    }

    // Care for the rest after last occurrence
    newString += source.substr(lastPos);

    source.swap(newString);
}

void JSONObject::escapeCharacters(std::string& stringToEscape)
{
    replaceAll(stringToEscape, "\\", "\\\\");
    replaceAll(stringToEscape, "\"", "\\\"");
    replaceAll(stringToEscape, "/", "\\/");
    replaceAll(stringToEscape, "\b", "\\b");
    replaceAll(stringToEscape, "\f", "\\f");
    replaceAll(stringToEscape, "\n", "\\n");
    replaceAll(stringToEscape, "\r", "\\r");
    replaceAll(stringToEscape, "\t", "\\t");
}

void JSONObject::unescapeCharacters(std::string& stringToUnescape)
{
    replaceAll(stringToUnescape, "\\\\", "\\");
    replaceAll(stringToUnescape, "\\\"", "\"");
    replaceAll(stringToUnescape, "\\/", "/");
    replaceAll(stringToUnescape, "\\b", "\b");
    replaceAll(stringToUnescape, "\\f", "\f");
    replaceAll(stringToUnescape, "\\n", "\n");
    replaceAll(stringToUnescape, "\\r", "\r");
    replaceAll(stringToUnescape, "\\t", "\t");
}