
#include <bwl/key_value_parser.h>

#include <bcl/beerocks_state_machine.h>
#include <bcl/beerocks_string_utils.h>
#include <bcl/network/network_utils.h>

#include <easylogging++.h>
#include <sstream>

namespace bwl {
constexpr char EVENT_KEYLESS_PARAM_OPCODE[] = "_opcode";
constexpr char EVENT_KEYLESS_PARAM_MAC[]    = "_mac";
constexpr char EVENT_KEYLESS_PARAM_IFACE[]  = "_iface";

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Functions ////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/**
 * @brief Finds first occurrence of two delimiters with string between them, on given string 'str':
 * <'delim_near'><any_characters except delimiters><'delim_far'>
 * 
 * @param[in] str String to find the first occurrence of two delimiters with string between them.  
 * @param[in] pos Postion of the first characters on 'str' to be considered in the search.
 * @param[in] delim_near Near delimiter.
 * @param[in] delim_far Near delimiter.
 * @return std::string::size_type Postion of the first character that matches. If no matches is 
 * found, returns string::npos.
 */
static std::string::size_type find_first_of_delimiter_pair(const std::string &str,
                                                           std::string::size_type pos,
                                                           char delim_near, char delim_far)
{
    // First search the end delimiter.
    auto idx = str.find_first_of(delim_far, pos);
    if (idx == std::string::npos) {
        return idx;
    }
    // Search back for the start delimiter from there.
    idx = str.rfind(delim_near, idx);
    return idx;
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void KeyValueParser::parse_line(std::istringstream &iss_in, std::list<char> delimiter_list,
                                parsed_line_t &parsed_line)
{
    if (delimiter_list.empty()) {
        return;
    }

    std::string str_storage, key;
    bool kv = true; // '1'=key, '0'=val
    while (std::getline(iss_in, str_storage, delimiter_list.front())) {
        // If reached to 1 delimiter resolution, the data can be parsed into key and value
        if (delimiter_list.size() == 1) {
            if (kv) {
                // Save key
                key = str_storage;
            } else {
                // Save Value
                parsed_line[key] = str_storage;
            }
            kv = !kv;

        } else {
            auto delimiter_list_out(delimiter_list);
            // Delete first delimiter
            delimiter_list_out.erase(delimiter_list_out.begin());
            std::istringstream iss_out(str_storage);
            parse_line(iss_out, delimiter_list_out, parsed_line);
        }
    }
}

void KeyValueParser::parse_multiline(std::istringstream &iss_in, std::list<char> delimiter_list,
                                     parsed_multiline_t &parsed_multiline)
{
    if (delimiter_list.empty()) {
        return;
    }

    std::string str_storage;
    while (std::getline(iss_in, str_storage, delimiter_list.front())) {
        auto delimiter_list_out(delimiter_list);

        // Delete first delimiter
        delimiter_list_out.erase(delimiter_list_out.begin());
        std::istringstream iss_out(str_storage);
        parsed_line_t parsed_line;
        parse_line(iss_out, delimiter_list_out, parsed_line);
        parsed_multiline.push_back(parsed_line);
    }
}

void KeyValueParser::parse_event(std::string event_str, parsed_line_t &parsed_line)
{
    // Eliminate event log level from the begining of the event string : "<3>".
    size_t idx_start = event_buffer_start_process_idx(event_str);

    // An event string might start with keyless values such as Even Name.
    // Find where the keyless parameters ends by finding where is the first parameter with key,
    // assuming it has a preceding " <SomeKeyString>='".
    auto idx = find_first_of_delimiter_pair(event_str, idx_start, ' ', '=');

    // Put null terminator at the end of our key=val, for string stream construction.
    if (idx != std::string::npos) {
        event_str[idx] = '\0';
    }

    // Parse keyless params part and insert to the 'parsed_line' map object.
    parse_event_keyless_params(event_str, idx_start, parsed_line);

    // Fill the map with the rest of event data.
    while (idx != std::string::npos) {

        idx_start = ++idx;

        // Find first '=' to skip on it
        idx = event_str.find_first_of('=', idx_start);

        // Find the next pair of delimiters index.
        idx = find_first_of_delimiter_pair(event_str, ++idx, ' ', '=');

        if (idx != std::string::npos) {
            // Put null terminator at the end of our key=val, for ss_in construction.
            event_str[idx] = '\0';
        }

        // Parse key=value
        std::istringstream iss_in(event_str.c_str() + idx_start);
        parse_line(iss_in, {'='}, parsed_line);
    }
}

size_t KeyValueParser::event_buffer_start_process_idx(const std::string &event_str)
{
    // Eliminate event log level from the begining of the event string : "<3>"
    auto idx_start = event_str.find_first_of(">");
    if (idx_start != std::string::npos) {
        idx_start++;
    } else {
        LOG(WARNING) << "Empty event! event_string: " << event_str;
    }
    return idx_start;
}

void KeyValueParser::parse_event_keyless_params(const std::string &event_str, size_t idx_start,
                                                parsed_line_t &parsed_line)
{
    // Insert to map known prams without key
    std::istringstream iss(event_str.c_str() + idx_start);
    std::string str_storage;
    bool opcode = true;
    while (std::getline(iss, str_storage, ' ')) {
        if (opcode) {
            // assume that the second param is data or event name
            parsed_line[EVENT_KEYLESS_PARAM_OPCODE] = str_storage;
            opcode                                  = false;
        } else if (beerocks::net::network_utils::is_valid_mac(str_storage)) {
            parsed_line[EVENT_KEYLESS_PARAM_MAC] = str_storage;
        } else if (!strncmp(str_storage.c_str(), "wlan", 4)) {
            parsed_line[EVENT_KEYLESS_PARAM_IFACE] = str_storage;
        }
    }
}

bool KeyValueParser::read_param(const std::string &key, parsed_line_t &obj, int64_t &value,
                                bool ignore_unknown)
{
    auto val_iter = obj.find(key);
    if (val_iter == obj.end()) {
        return false;
    }

    static const std::string unknown_string = "UNKNOWN";

    if (ignore_unknown && !unknown_string.compare(val_iter->second)) {
        value = 0;
        return true;
    }

    value = beerocks::string_utils::stoi(val_iter->second);

    return true;
}

bool KeyValueParser::read_param(const std::string &key, parsed_line_t &obj, const char **value)
{
    auto val_iter = obj.find(key);
    if (val_iter == obj.end()) {
        LOG(ERROR) << "param :" << key << " does not exist";
        return false;
    }

    *value = reinterpret_cast<const char *>((val_iter->second).c_str());
    return true;
}

void KeyValueParser::parsed_obj_debug(const parsed_line_t &obj)
{
    LOG(DEBUG) << std::endl << "parsed_obj_debug: ";
    for (const auto &kv_element : obj) {
        LOG(DEBUG) << "key: " << kv_element.first << ", value: " << kv_element.second;
    }
}

void KeyValueParser::parsed_obj_debug(const parsed_multiline_t &obj)
{
    LOG(DEBUG) << std::endl << "parsed_obj_debug: ";
    int element_num = 0;
    for (const auto &list_element : obj) {
        LOG(DEBUG) << "vector element: " << element_num;
        parsed_obj_debug(list_element);
        element_num++;
    }
}

} // namespace bwl
