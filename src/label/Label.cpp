//
// Created by ephir on 07.12.2025.
//

#include "Label.h"

std::vector<std::string> generate_labels(const size_t number) {
    constexpr size_t alphabet_size = 26;

    const size_t length = std::max(static_cast<size_t>(1),
                                   static_cast<size_t>(std::ceil(std::log(number) / std::log(alphabet_size))));

    std::vector<std::string> result;
    result.reserve(number);

    for (size_t i = 0; i < number; ++i) {
        size_t n = i;
        std::string s(length, 'a');

        for (size_t pos = length; pos > 0; --pos) {
            s[pos - 1] = 'a' + (n % alphabet_size);
            n /= alphabet_size;
        }

        result.push_back(std::move(s));
    }

    return result;
}
