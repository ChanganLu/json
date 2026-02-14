#ifndef ELEMENT_TYPE_HPP
#define ELEMENT_TYPE_HPP

#include <variant>
#include <charconv>
#include <string>
#include <vector>

#include "./ordered_dict.hpp"
#include "./json_exception.hpp"
#include "./json_typing.hpp"
#include "./char_utils.hpp"
#include "./string_utils.hpp"


namespace json {

class JsonElement;

using Bool = bool;
using String = std::string;
using Object = utils::OrderedDict<std::string, JsonElement>;
using Array = std::vector<JsonElement>;


class JsonElement {

public:
    JsonElementTyping type;
    std::variant<NullType, Bool, Number, String, Object, Array> element;

    JsonElement(): type(JsonElementTyping::ERROR) {}
    explicit JsonElement(const NullType& null_element): type(JsonElementTyping::NULL_TYPE), element(null_element) {};
    explicit JsonElement(Bool bool_val): type(JsonElementTyping::BOOLEAN), element(bool_val) {}
    explicit JsonElement(const Number& number_val): type(JsonElementTyping::NUMBER), element(number_val) {}
    explicit JsonElement(const String& string_val): type(JsonElementTyping::STRING), element(string_val) {}
    explicit JsonElement(const Object& object_val): type(JsonElementTyping::OBJECT), element(object_val) {}
    explicit JsonElement(const Array& array_val): type(JsonElementTyping::ARRAY), element(array_val) {}

    JsonElement(const JsonElement& json_element): type(json_element.type), element(json_element.element) {}
    JsonElement& operator = (const JsonElement& json_element) { type = json_element.type; element = json_element.element; return *this; }
    
    JsonElement& operator = (const NullType&) { type = JsonElementTyping::NULL_TYPE; element = NullType(); return *this; }
    JsonElement& operator = (Bool bool_val) { type = JsonElementTyping::BOOLEAN; element = bool_val; return *this; }
    JsonElement& operator = (const Number& number_val) { type = JsonElementTyping::NUMBER; element = number_val; return *this; }
    JsonElement& operator = (const String& string_val) { type = JsonElementTyping::STRING; element = string_val; return *this; }
    JsonElement& operator = (const Object& object_val) { type = JsonElementTyping::OBJECT; element = object_val; return *this; }
    JsonElement& operator = (const Array& array_val) { type = JsonElementTyping::ARRAY; element = array_val; return *this; }

    const NullType& get_Null() const { return std::get<NullType>(element); }
    const Bool& get_Bool() const { return std::get<Bool>(element); }
    const Number& get_Number() const { return std::get<Number>(element); }
    const String& get_String() const { return std::get<String>(element); }
    const Object& get_Object() const { return std::get<Object>(element); }
    const Array& get_Array() const { return std::get<Array>(element); }

    const std::string to_string(std::string prefill = std::string(""), bool escape = false) const {
        // `prefill` is only used after `\n`
        if (type == JsonElementTyping::ERROR) throw json::JsonTypeError("Json type `ERROR` can not be convert to string.");
        else if (type == JsonElementTyping::NULL_TYPE) return get_Null().to_string();
        else if (type == JsonElementTyping::BOOLEAN) return get_Bool() ? std::string("true") : std::string("false");
        else if (type == JsonElementTyping::NUMBER) return get_Number().to_string();
        else if (type == JsonElementTyping::STRING) return escape ? utils::parse_string_escape(get_String()) : get_String();
        else if (type == JsonElementTyping::OBJECT) {
            std::string result = "{\n";
            const Object& ordered_dict = get_Object();
            size_t i = 0;
            size_t num_items = ordered_dict.size();
            std::string new_prefill = prefill + "    ";
            for (const auto& [key, value]: ordered_dict.items()) {
                result += new_prefill + (escape ? utils::parse_string_escape(key) : key) + ": " + value.to_string(new_prefill, escape);
                if (++i != num_items) result += ',';
                result += '\n';
            }
            result += prefill + '}';
            return result;
        }
        else if (type == JsonElementTyping::ARRAY) {
            std::string result = "[\n";
            const Array& list = get_Array();
            size_t i = 0;
            size_t num_items = list.size();
            std::string new_prefill = prefill + "    ";
            for (const auto& item: list) {
                result += new_prefill + item.to_string(new_prefill, escape);
                if (++i != num_items) result += ',';
                result += '\n';
            }
            result += prefill + ']';
            return result;
        }
        else throw json::JsonTypeError("Unknown type is encontered when convert a Json element to string.");
    }
    friend std::ostream& operator << (std::ostream& os, const JsonElement& json_element) { return os << json_element.to_string(); }

    friend std::istream& operator >> (std::istream& is, JsonElement& json_element) {

        /*
        判断 json 类型的依据:
        1. 读到 字母n/t/f , 分别为 null/true/false
        2. 读到 数字/负号(-) , 判断有没有 小数点/指数符合(e) : 如果有, 则是浮点数, 否则是整数
        3. 读到 双引号(") , 则为 字符串 , 读到第一个没有被 转义符号(\) 修饰的 双引号(") 结束
        4. 读到 左方括号([) , 则为 列表 , 读到匹配的 右方括号(]) 结束
        5. 读到 左花括号({) , 则为 字典 , 读到匹配的 右花括号(}) 结束
        */

        int c;
        while (utils::is_not_eof(c = is.get())) {
            // 读到 `n` , 继续读完 `ull` , 得到 null
            if (c == (int)'n') {
                int char_1 = is.get();
                int char_2 = is.get();
                int char_3 = is.get();
                if (char_1 != (int)'u' || char_2 != (int)'l' || char_3 != (int)'l') {
                    std::string err_string("Expected `n` `u` `l` `l` in istream but receive [n] ");
                    err_string = err_string + '`' + (char)char_1 + "` ";
                    err_string = err_string + '`' + (char)char_2 + "` ";
                    err_string = err_string + '`' + (char)char_3 + "`.";
                    throw json::JsonIstreamFailure(err_string);
                }
                json_element = NullType();
                break;
            }
            // 读到 `t` , 继续读完 `rue` , 得到 true
            else if (c == (int)'t') {
                int char_1 = is.get();
                int char_2 = is.get();
                int char_3 = is.get();
                if (char_1 != (int)'r' || char_2 != (int)'u' || char_3 != (int)'e') {
                    std::string err_string("Expected `t` `r` `u` `e` in istream but receive [t] ");
                    err_string = err_string + '`' + (char)char_1 + "` ";
                    err_string = err_string + '`' + (char)char_2 + "` ";
                    err_string = err_string + '`' + (char)char_3 + "`.";
                    throw json::JsonIstreamFailure(err_string);
                }
                json_element = true;
                break;
            }
            // 读到 `f` , 继续读完 `alse` , 得到 false
            else if (c == (int)'f') {
                int char_1 = is.get();
                int char_2 = is.get();
                int char_3 = is.get();
                int char_4 = is.get();
                if (char_1 != (int)'a' || char_2 != (int)'l' || char_3 != (int)'s' || char_4 != (int)'e') {
                    std::string err_string("Expected `f` `a` `l` `s` `e` in istream but receive [f] ");
                    err_string = err_string + '`' + (char)char_1 + "` ";
                    err_string = err_string + '`' + (char)char_2 + "` ";
                    err_string = err_string + '`' + (char)char_3 + "`.";
                    err_string = err_string + '`' + (char)char_4 + "`.";
                    throw json::JsonIstreamFailure(err_string);
                }
                json_element = false;
                break;
            }
            // 读到 数字或负号 , 继续读完整个数字, 期间: 1. 至多出现一个指数符号; 2. 另一个负号(如果有)只能紧跟指数符号; 3. 至多出现一个小数点且不能在指数符号的后面; 如果既没有指数符号也没有小数点则是整数, 否则为浮点数
            else if (c == (int)'-' || utils::is_digit(c)) {
                bool is_negative = false;
                bool has_floating = false;
                bool has_exponent = false;
                // Int64 integer = (c == (int)'-' ? 0 : utils::to_digit(c));
                // Float64 floating = 0.0; Float64 scale = 0.1;
                // Int64 exponent = 0;
                bool is_exp_negative = false; bool certain_sign = false;
                std::string integer_part;
                std::string floating_part;
                std::string exponent_part;
                if (c == (int)'-') is_negative = true;
                else integer_part += (char)c;
                // 读取整数部分, 整数部分仅允许数字
                while (true) {
                    c = is.peek();
                    // 如果下一个字符是数字, 加入整数部分
                    if (utils::is_digit(c)) {
                        is.get();
                        // integer = integer * 10 + utils::to_digit(c);
                        integer_part += (char)c;
                    }
                    // 如果下一个字符是小数点, 说明这个数字有小数部分
                    else if (c == (int)'.') {
                        is.get();
                        has_floating = true;
                        break;
                    }
                    // 如果下一个字符是指数符号(e/E), 说明这个数字有指数部分
                    else if (utils::lower(c) == (int)'e') {
                        is.get();
                        has_exponent = true;
                        break;
                    }
                    // 如果下一个字符是其他字符, 说明这个数字已经读完
                    else break;
                }
                // 如果有小数部分, 读取小数部分, 仅允许数字
                while (has_floating) {
                    c = is.peek();
                    // 如果下一个字符是整数, 则加入小数部分
                    if (utils::is_digit(c)) {
                        is.get();
                        // floating = floating + scale * (Float64)utils::to_digit(c);
                        // scale = scale * 0.1;
                        floating_part += (char)c;
                    }
                    // 如果下一个字符是指数符号, 说明这个数字有指数部分
                    else if (utils::lower(c) == (int)'e') {
                        is.get();
                        has_exponent = true;
                        break;
                    }
                    // 如果下一个字符是其他字符, 说明这个数字已经读完
                    else break;
                }
                // 如果有指数部分, 读取指数部分, 仅允许数字和至多一个先导的正号或负号
                while (has_exponent) {
                    c = is.peek();
                    // 如果下一个字符是数字, 则加入指数部分
                    if (utils::is_digit(c)) {
                        is.get();
                        // exponent = exponent * 10 + utils::to_digit(c);
                        certain_sign = true;
                        exponent_part += (char)c;
                    }
                    // 如果下一个字符是先导的负号, 则指数部分为负
                    else if (c == (int)'-' && !certain_sign) {
                        is.get();
                        is_exp_negative = true;
                        certain_sign = true;
                    }
                    // 如果下一个字符是先导的正号, 则指数部分为正
                    else if (c == (int)'+' && !certain_sign) {
                        is.get();
                        is_exp_negative = false;
                        certain_sign = true;
                    }
                    // 如果是其他字符, 说明这个数字已经读完
                    else break;
                }
                // 创建 Number 类型
                json_element = Number(integer_part, floating_part, exponent_part, is_negative, is_exp_negative, has_floating, has_exponent);
                break;
            }
            // 读到 `"` , 继续读到第一个不被转义符号修饰的 `"` , 得到字符串
            else if (c == (int)'"') {
                json_element = utils::get_string_from_istream(is);
                break;
            }
            // 读到 `[` , 继续读到匹配的 `]` , 得到列表
            else if (c == (int)'[') {
                Array list;
                bool last_is_comma = false;
                while (true) {
                    c = is.peek();
                    if (utils::is_eof(c)) throw json::JsonIstreamFailure("Unclosed square bracket is encountered when getting an Array.");
                    else if (c == (int)']') {
                        if (last_is_comma) throw json::JsonIstreamFailure("Array can only finish after an item or left bracket, not after a comma.");
                        else { is.get(); break; }
                    }
                    else if (c == (int)',') {
                        if (last_is_comma) throw json::JsonIstreamFailure("Comma can only appear after an item, not after a left bracket or another comma.");
                        else { is.get(); last_is_comma = true; continue; }
                    }
                    else if (utils::is_blank(c)) { is.get(); continue; }
                    else {
                        // 递归读取列表元素
                        JsonElement item;
                        is >> item;
                        list.push_back(item);
                        last_is_comma = false;
                    }
                }
                json_element = list;
                break;
            }
            // 读到 `{` , 继续读到匹配的 `}` , 得到字典
            else if (c == (int)'{') {
                Object dict;
                /*
                状态循环
                0: left-brace  ----+
                    ↓              |
                1: key  <--+       |
                2: colon   |       |
                3: value   |       |
                    |      |       ↓
                4:  +--> comma / right-brace
                */
                int status = 0;
                String key;
                JsonElement value;
                while (true) {
                    // 读取键-值对
                    c = is.peek();
                    if (utils::is_eof(c)) throw json::JsonIstreamFailure("Unclosed brace is encountered when getting an Object.");
                    else if (c == (int)'}') {
                        if (status == 0 || status == 3) { is.get(); break; }
                        else throw json::JsonIstreamFailure("Object can only finish after a key-value pair or left brace, but not after a comma, key or colon.");
                    }
                    else if (c == (int)',') {
                        if (status == 3) { status = 4; is.get(); continue; }
                        else throw json::JsonIstreamFailure("Comma can only appear after a key-value pair, but not after a left bracket, key, colon or another comma.");
                    }
                    else if (c == (int)':') {
                        if (status == 1) { status = 2; is.get(); continue; }
                        else throw json::JsonIstreamFailure("Colon can only appear after a key, but not after a left bracket, another colon, value or comma.");
                    }
                    else if (utils::is_blank(c)) { is.get(); continue; }
                    else if (c == (int)'"' && (status == 0 || status == 4)) {
                        // 读取键
                        status = 1; is.get();
                        key = utils::get_string_from_istream(is);
                        continue;
                    }
                    else if (status == 2) {
                        // 递归读取值
                        status = 3;
                        is >> value;
                        dict.insert(key, value);
                        key.clear(); value = JsonElement();
                        continue;
                    }
                    else throw json::JsonIstreamFailure("Fail to get an Object.");
                }
                json_element = dict;
                break;
            }
            // 读到空白字符, 跳过
            else if (utils::is_blank(c)) continue;
            // 读到其他字符, 抛出异常
            else throw json::JsonIstreamFailure(std::string("Unexpected character: `") + (char)c + std::string("`."));
        }

        return is;
    }

};


}

#endif