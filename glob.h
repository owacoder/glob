#include <string>
#include <vector>

/** @brief Glob to see if string matches pattern.
 *
 * This function operates on single bytes, and does not support UTF-8.
 *
 * Pattern can contain the following:
 *
 *     '?' - Matches any single character. The character must be present, even if the '?' is at the end of the pattern.
 *     '*' - Matches any sequence of zero or more characters. If pattern starts and ends with '*', a substring is searched for.
 *     '[' - NOT IMPLEMENTED: Begins a character set to search for. Sets can be ranges '[0-9a-z]' or distinct sets '[CBV]', and can be negated if the first character is '^'
 *           You can search for a literal ']' by including that as the first character in a set (i.e. to search for ']', use '[]]'; to search for '[', use '[[]'; and to search for either '[' or ']', use '[][]')
 *           This is because searching for the empty set is not allowed.
 *     xxx - Any other character matches itself. It must be present.
 *
 * @param str The string to check.
 * @param pattern The glob pattern to match @p str against.
 * @return 0 if `str` matches `pattern`, -1 if no match, and -2 if the pattern has improper syntax.
 */
int glob(std::string_view test, std::string_view glob) {
    struct position {
        position(std::string_view testpos, std::string_view globpos)
            : testpos(testpos)
            , globpos(globpos)
        {}

        std::string_view testpos; // Remainder of string to test
        std::string_view globpos;
    };

    std::vector<position> positions;

    positions.push_back({test, glob});

    while (true) {
        auto &current = positions.back();
        bool match = true;

        for (; current.testpos.size() && current.globpos.size(); current.globpos.remove_prefix(1)) {
            const auto testchar = current.testpos[0];
            const auto globchar = current.globpos[0];

            if (globchar == '?') {
                current.testpos.remove_prefix(1);
            } else if (globchar == '*') {
                while (current.globpos.size() && current.globpos[0] == '*')
                    current.globpos.remove_prefix(1);

                // Automatic match if wildcard at end of string (matches everything left to test)
                if (current.globpos.empty())
                    return 0;

                const position p = current;
                positions.push_back(p);
                if (positions.size() > 2)
                    positions.erase(positions.begin());

                goto try_new_glob;
            } else {
                if (globchar != testchar) {
                    match = false;
                    break;
                }

                current.testpos.remove_prefix(1);
            }
        }

        if (current.testpos.empty()) {
            while (current.globpos.size() && current.globpos[0] == '*')
                current.globpos.remove_prefix(1);

            return current.globpos.empty() ? 0 : -1;
        } else if (positions.size() == 1) {
            return match ? 0 : -1;
        } else {
            auto &previous = positions[positions.size() - 2];

            previous.testpos.remove_prefix(1);
            current = previous;
        }

try_new_glob:;
    }

    return -1;
}
