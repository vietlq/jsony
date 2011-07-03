#ifndef JsonVariant_H
#define JsonVariant_H

#include <map>
#include <vector>
#include <string>
#include <boost/variant.hpp>
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>

struct JsonNull {};
struct JsonValue;

typedef std::map<std::string, JsonValue *> JsonObject;
typedef std::vector<JsonValue *> JsonArray;
typedef boost::variant<
    JsonNull, bool, long, double, std::string,
    JsonObject, JsonArray,
    boost::recursive_wrapper<JsonValue>
> JsonDataValue;

struct JsonValue
{
    JsonDataValue data;
};

#endif // JsonVariant_H
