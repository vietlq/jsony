#include <map>
#include <string>
#include <vector>
#include <iostream>

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_container.hpp>
#include <boost/spirit/include/phoenix_function.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/fusion/include/adapt_assoc_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/variant.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

#include "JsonVariant.h"
#include "SemanticAction.h"

template <typename Iterator>
struct json_grammar : qi::grammar<Iterator, ascii::space_type>
{
    json_grammar(SemanticAction & semActions_) : json_grammar::base_type(start), semActions(semActions_)
    {
        using ascii::alpha;
        using ascii::alnum;
        using qi::long_long;
        using qi::long_double;
        using qi::lit;
        using qi::char_;
        using qi::double_;
        using qi::long_;
        using qi::lexeme;

        // 
        start =
            char_('{')              [boost::bind(&SemanticAction::actBeginObj, &semActions)]
            >> -(js_member % ',')
            >> char_('}')           [boost::bind(&SemanticAction::actEndObj, &semActions)]
        ;
        // 
        js_member =
            js_key                  [phoenix::bind(&SemanticAction::actKey, &semActions, qi::_1)]
            >> ':' >> js_value
        ;
        // 
        js_array = char_('[')       [boost::bind(&SemanticAction::actBeginArray, &semActions)]
            >> -(js_value % ',')
            >> char_(']')           [boost::bind(&SemanticAction::actEndArray, &semActions)]
        ;
        // 
        js_bool = lit("true")       //[boost::bind(&SemanticAction::actTrue, &semActions, _1)]
            | lit("false")          //[boost::bind(&SemanticAction::actFalse, &semActions, _1)]
        ;
        // 
        js_null = lit("null");
        // 
        js_value = js_string        [boost::bind(&SemanticAction::actString, &semActions, _1)]
            | js_num                //[]
            | js_array              //[]
            | start                 //[]
            | js_bool               [boost::bind(&SemanticAction::actBool, &semActions, _1)]
            | js_null               [boost::bind(&SemanticAction::actNull, &semActions)]
        ;
        // 
        js_num = js_real            [boost::bind(&SemanticAction::actDouble, &semActions, _1)]
            | long_                 [boost::bind(&SemanticAction::actLong, &semActions, _1)]
        ;
        // 
        js_key = (alpha >> *alnum) | js_string;
        // 
        js_single_quoted_str = lexeme["'" >> *(~char_("\\'") | ("\\" >> char_("\\'"))) >> "'"];
        //js_single_quoted_str = lexeme["'" >> *(~char_("\\'") | ("\\" >> char_)) >> "'"];
        // 
        js_double_quoted_str = lexeme['"' >> *(~char_("\\\"") | ("\\" >> char_("\\\""))) >> '"'];
        //js_double_quoted_str = lexeme['"' >> *(~char_("\\\"") | ("\\" >> char_)) >> '"'];
        // 
        js_string = js_single_quoted_str | js_double_quoted_str;
    }

    qi::rule<Iterator, ascii::space_type> start;
    qi::rule<Iterator, ascii::space_type> js_member;
    qi::rule<Iterator, ascii::space_type> js_value;
    qi::rule<Iterator, ascii::space_type> js_array;
    qi::rule<Iterator, ascii::space_type> js_num;
    qi::rule<Iterator, ascii::space_type> js_null;
    qi::real_parser<double, qi::strict_real_policies<double> > js_real;
    qi::rule<Iterator, std::string(), ascii::space_type> js_single_quoted_str;
    qi::rule<Iterator, std::string(), ascii::space_type> js_double_quoted_str;
    qi::rule<Iterator, std::string(), ascii::space_type> js_key;
    qi::rule<Iterator, std::string(), ascii::space_type> js_string;
    qi::rule<Iterator, std::string(), ascii::space_type> js_bool;

    SemanticAction & semActions;
};

int main()
{
    std::string inputStr;
    boost::variant<long, double, bool, std::string>;
    SemanticAction semActions;
    json_grammar<std::string::const_iterator> jsonParser(semActions);
    bool parseOK = false;

    while(std::getline(std::cin, inputStr)) {
        if(inputStr.empty() || inputStr[0] == 'q' || inputStr[0] == 'Q')
            break;

        std::string::const_iterator iter = inputStr.begin();
        std::string::const_iterator iterEnd = inputStr.end();

        parseOK = qi::phrase_parse(iter, iterEnd, jsonParser, ascii::space);

        if(parseOK && iter == iterEnd) {
            std::cout << "Successfully parsed the input as JSON!" << std::endl;
        } else {
            std::cout << "Cannot parse the input as JSON!" << std::endl;
        }
    }

    return 0;
}
