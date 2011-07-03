#include <iostream>
#include <iomanip>

#include "SemanticAction.h"

SemanticAction::SemanticAction()
{
    bracketCount = 0;
    levelCount = 0;
    indentCount = 0;
    currKey = "";
}

SemanticAction::~SemanticAction()
{
}

void SemanticAction::actBeginObj()
{
    std::cout << std::string(indentCount, ' ') << "actBeginObj" << std::endl;
    bracketCount++;
    levelCount++;
    indentCount += 2;

    if(1 == bracketCount) {
        stackValues = std::stack<JsonValue *>();
        stackTypes = std::stack<JsonType>();

        value.data = JsonObject();
        currKey = "";

        this->pCurrVal = &value;
        stackValues.push(pCurrVal);
        stackTypes.push(SemanticAction::ObjectType);
    } else {
        JsonValue * pJsonVal = this->addValue(JsonObject());
        
        pCurrVal = pJsonVal;
        stackValues.push(pCurrVal);
        stackTypes.push(SemanticAction::ObjectType);
    }
}

void SemanticAction::actEndObj()
{
    bracketCount--;
    levelCount--;
    indentCount -= 2;

    std::cout << std::string(indentCount, ' ') << "actEndObj" << std::endl;
    std::cout << std::string(indentCount, ' ') << "The number of children the object has: " << boost::get<JsonObject>(pCurrVal->data).size() << std::endl;

    if(0 == bracketCount) {
        std::cout << std::string(indentCount, ' ') << "That's all folks!" << std::endl;
    } else {
        stackValues.pop();
        stackTypes.pop();

        pCurrVal = stackValues.top();
    }
}

void SemanticAction::actBeginArray()
{
    std::cout << std::string(indentCount, ' ') << "actBeginArray" << std::endl;
    levelCount++;
    indentCount += 2;

    JsonValue * pJsonVal = this->addValue(JsonArray());
        
    pCurrVal = pJsonVal;
    stackValues.push(pCurrVal);
    stackTypes.push(SemanticAction::ArrayType);
}

void SemanticAction::actEndArray()
{
    levelCount--;
    indentCount -= 2;

    std::cout << std::string(indentCount, ' ') << "actEndArray" << std::endl;
    std::cout << std::string(indentCount, ' ') << "The number of children the array has: " << boost::get<JsonArray>(pCurrVal->data).size() << std::endl;

    stackValues.pop();
    stackTypes.pop();

    pCurrVal = stackValues.top();
}

void SemanticAction::actKey(std::string str)
{
    currKey = str;
    std::cout << std::string(indentCount, ' ') << "key = " << str << std::endl;
}

void SemanticAction::actKey(const char* start, const char* end)
{
    std::string str(start, end);
    currKey = str;
    std::cout << std::string(indentCount, ' ') << "key = " << str << std::endl;
}

void SemanticAction::actNull()
{
    std::cout << std::string(indentCount, ' ') << "got null" << std::endl;

    this->addValue(JsonNull());
}

void SemanticAction::actBool(std::string str)
{
    bool val;

    if(str == "true")
        val = true;
    if(str == "false")
        val = false;

    std::cout << std::string(indentCount, ' ') << "got bool: " << val << std::endl;

    this->addValue(val);
}

void SemanticAction::actString(std::string val)
{
    std::cout << std::string(indentCount, ' ') << "got string: " << val << std::endl;

    this->addValue(val);
}

void SemanticAction::actLong(long val)
{
    std::cout << std::string(indentCount, ' ') << "got long: " << val << std::endl;

    this->addValue(val);
}

void SemanticAction::actDouble(double val)
{
    std::cout << std::string(indentCount, ' ') << "got double: " << val << std::endl;

    this->addValue(val);
}

JsonValue * SemanticAction::addValue(const JsonDataValue & data_)
{
    JsonValue * pJsonVal = new JsonValue();
    pJsonVal->data = data_;

    if(SemanticAction::ObjectType == stackTypes.top()) {
        boost::get<JsonObject>(pCurrVal->data).insert(
            std::pair<std::string, JsonValue *>(this->currKey, pJsonVal)
        );
    }

    if(SemanticAction::ArrayType == stackTypes.top()) {
        boost::get<JsonArray>(pCurrVal->data).push_back(pJsonVal);
    }

    return pJsonVal;
}
