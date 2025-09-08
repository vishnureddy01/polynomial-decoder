#include <bits/stdc++.h>
using namespace std;

// Simple JSON parser for flat objects { "key": { "base": "...", "value": "..." } }
map<string, pair<int,string>> loadJson(const string &filename) {
    ifstream in(filename);
    if (!in) {
        cerr << "Could not open " << filename << endl;
        return {};
    }
    string content((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());

    map<string, pair<int,string>> result;
    regex entryRegex(R"raw("(\w+)"\s*:\s*\{\s*"base"\s*:\s*"(\d+)"\s*,\s*"value"\s*:\s*"([0-9a-zA-Z]+)"\s*\})raw");
    smatch m;
    string::const_iterator searchStart(content.cbegin());
    while (regex_search(searchStart, content.cend(), m, entryRegex)) {
        string key = m[1];
        int base = stoi(m[2]);
        string value = m[3];
        result[key] = {base, value};
        searchStart = m.suffix().first;
    }
    return result;
}

// Convert from base (2..36) to long long
long long decodeBase(const string &s, int base) {
    string digits = "0123456789abcdefghijklmnopqrstuvwxyz";
    long long result = 0;
    for (char ch : s) {
        int val = digits.find(tolower(ch));
        if (val < 0 || val >= base) {
            cerr << "Invalid digit '" << ch << "' for base " << base << endl;
            return -1;
        }
        result = result * base + val;
    }
    return result;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        cerr << "Usage: ./solve a b c\n";
        return 1;
    }

    double a = atof(argv[1]);
    double b = atof(argv[2]);
    double c = atof(argv[3]);

    cout << "f(x) = " << a << "x^2 + " << b << "x + " << c << endl;
    cout << "c = f(0) = " << c << endl;

    // Solve quadratic
    double D = b*b - 4*a*c;
    vector<string> roots;
    if (a == 0) {
        if (b != 0) {
            roots.push_back(to_string(-c / b));
        }
    } else if (D > 0) {
        double r1 = (-b + sqrt(D)) / (2*a);
        double r2 = (-b - sqrt(D)) / (2*a);
        roots = {to_string(r1), to_string(r2)};
    } else if (D == 0) {
        double r = -b / (2*a);
        roots = {to_string(r)};
    } else {
        double real = -b / (2*a);
        double imag = sqrt(-D) / (2*a);
        roots = {to_string(real) + "+" + to_string(imag) + "i",
                 to_string(real) + "-" + to_string(imag) + "i"};
    }

    // Load test cases from both files
    map<string, pair<int,string>> data;
    auto d1 = loadJson("first.json");
    auto d2 = loadJson("second.json");
    data.insert(d1.begin(), d1.end());
    data.insert(d2.begin(), d2.end());

    // Decode values
    vector<map<string,string>> testCases;
    for (auto &kv : data) {
        string key = kv.first;
        int base = kv.second.first;
        string rawVal = kv.second.second;
        long long dec = decodeBase(rawVal, base);
        map<string,string> entry;
        entry["x"] = key;
        entry["base"] = to_string(base);
        entry["value_raw"] = rawVal;
        entry["y_decimal"] = to_string(dec);
        testCases.push_back(entry);
    }

    // Write result.json
    ofstream out("result.json");
    out << "{\n";
    out << "  \"polynomial\": \"f(x)=" << a << "x^2+" << b << "x+" << c << "\",\n";
    out << "  \"coefficients\": {\"a\": " << a << ", \"b\": " << b << ", \"c\": " << c << "},\n";
    out << "  \"roots\": [";
    for (size_t i=0;i<roots.size();i++) {
        out << "\"" << roots[i] << "\"";
        if (i+1<roots.size()) out << ",";
    }
    out << "],\n";
    out << "  \"test_cases\": [\n";
    for (size_t i=0;i<testCases.size();i++) {
        auto &t = testCases[i];
        out << "    {\"x\": \""<<t["x"]<<"\", \"base\": "<<t["base"]<<", \"value_raw\": \""<<t["value_raw"]<<"\", \"y_decimal\": \""<<t["y_decimal"]<<"\"}";
        if (i+1<testCases.size()) out << ",";
        out << "\n";
    }
    out << "  ]\n";
    out << "}\n";
    out.close();

    cout << "Wrote result.json with roots and decoded test cases.\n";
    return 0;
}
