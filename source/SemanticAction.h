#ifndef SemanticAction_H
#define SemanticAction_H

#include <string>
#include <stack>
#include <list>

#include "JsonValue.h"
#include "JsonVariant.h"

class SemanticAction
{
public:
    enum JsonType {NullType, BoolType, LongType, DoubleType, StringType, ArrayType, ObjectType};

    SemanticAction();

    virtual ~SemanticAction();

    void actBeginObj();
    void actEndObj();
    void actBeginArray();
    void actEndArray();
    void actKey(std::string str);
    void actKey(const char* start, const char* end);
    void actNull();
    void actBool(std::string str);
    void actString(std::string str);
    void actLong(long val);
    void actDouble(double val);

    JsonValue * addValue(const JsonDataValue & data_);

    std::string currKey;
    unsigned bracketCount;
    unsigned levelCount;
    unsigned indentCount;
    JsonValue value;
    std::stack<JsonValue *> stackValues;
    std::stack<JsonType> stackTypes;
    JsonValue * pCurrVal;
};

#endif // SemanticAction_H
