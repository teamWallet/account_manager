#include <string>
#include <vector>
using namespace std;

bool is_any(const char &s, const string &candidates) {
    for (const char &c : candidates) {
        if (s == c) {
            return true;
        }
    }
    return false;
}

string::size_type find_any(const string &s, const string &delimiters, const string::size_type start = 0) {
    for (std::string::size_type i = start; i < s.size(); ++i) {
        if (is_any(s[i], delimiters)) {
            return i;
        }
    }
    return string::npos;
}

std::string trim(const string &str) {
    const size_t first = str.find_first_not_of(' ');
    const size_t last  = str.find_last_not_of(' ');
    return (const std::string)str.substr(first, (last - first + 1));
}

void split(const string &s, const string &delimiters, vector<string> &v) {
    string::size_type i = 0;
    string::size_type j = find_any(s, delimiters);

    while (j != string::npos) {
        v.push_back(s.substr(i, j - i));
        i = ++j;
        j = find_any(s, delimiters, j);

        if (j == string::npos) {
            v.push_back(s.substr(i, s.length()));
        }
    }
}
