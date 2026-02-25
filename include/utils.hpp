#pragma once

#include <charconv>
#include <stdexcept>
#include <string>
#include <string_view>

inline int ToInt(std::string_view value) {
    int result{};
    auto [parse_end_ptr, error_code] = std::from_chars(value.begin(), value.end(), result);
    if (error_code != std::errc{}) {
        throw std::invalid_argument(
            "Cannot convert '" + std::string(value) +
            "' to integral: invalid format (error_code: " + std::to_string(static_cast<int>(error_code)) + ")");
    }

    if (parse_end_ptr != value.data() + value.size()) {
        throw std::invalid_argument("Cannot convert '" + std::string(value) +
                                    "' to integral: trailing characters after '" +
                                    std::string(parse_end_ptr, value.data() + value.size()) + "'");
    }

    return result;
}
