#include <bits/stdc++.h>
using namespace std;

// Convert from base (2..36) string to decimal string
string decodeBaseString(const string &s, int base) {
    string digits = "0123456789abcdefghijklmnopqrstuvwxyz";
    string result = "0";

    for (char ch : s) {
        int val = digits.find(tolower(ch));
        if (val < 0 || val >= base) return "-1"; // invalid digit

        // Multiply result by base
        string tmp = "0";
        int carry = 0;
        for (int i = result.size()-1; i>=0; --i) {
            int digit = result[i]-'0';
            int prod = digit*base + carry;
            result[i] = (prod % 10) + '0';
            carry = prod / 10;
        }
        while (carry) {
            result = char(carry%10+'0') + result;
            carry/=10;
        }

        // Add val
        carry = val;
        for (int i=result.size()-1;i>=0;i--) {
            int sum = (result[i]-'0') + carry;
            result[i] = (sum % 10) + '0';
            carry = sum / 10;
        }
        while(carry){
            result = char(carry%10+'0') + result;
            carry /=10;
        }
    }
    // Remove leading zeros
    size_t pos = result.find_first_not_of('0');
    if (pos != string::npos) return result.substr(pos);
    return "0";
}

// Load JSON-like data into map<string, pair<int,string>>
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

int main(int argc, char *argv[]) {
    double a=2, b=-5, c=3; // default
    if(argc>=4){
        a = atof(argv[1]);
        b = atof(argv[2]);
        c = atof(argv[3]);
    }

    // Solve quadratic
    double D = b*b - 4*a*c;
    vector<string> roots;
    if(a==0){
        if(b!=0) roots.push_back(to_string(-c/b));
    } else if(D>0){
        roots = {to_string((-b+sqrt(D))/(2*a)), to_string((-b-sqrt(D))/(2*a))};
    } else if(D==0){
        roots.push_back(to_string(-b/(2*a)));
    } else {
        double real = -b/(2*a);
        double imag = sqrt(-D)/(2*a);
        roots = {to_string(real)+"+"+to_string(imag)+"i", to_string(real)+"-"+to_string(imag)+"i"};
    }

    // Load test cases
    map<string, pair<int,string>> data;
    auto d1 = loadJson("first.json");
    auto d2 = loadJson("second.json");
    data.insert(d1.begin(), d1.end());
    data.insert(d2.begin(), d2.end());

    // Process test cases
    vector<map<string,string>> testCases;
    for(auto &kv : data){
        string key = kv.first;
        int base = kv.second.first;
        string rawVal = kv.second.second;
        string y_decimal = decodeBaseString(rawVal, base);

        // Evaluate polynomial f(x) = ax^2 + bx + c
        double fx = a*stod(key)*stod(key) + b*stod(key) + c;

        map<string,string> entry;
        entry["x"] = key;
        entry["base"] = to_string(base);
        entry["value_raw"] = rawVal;
        entry["y_decimal"] = y_decimal;
        entry["f_x"] = to_string(fx);

        testCases.push_back(entry);
    }

    // Print JSON
    cout << "{\n";
    cout << "  \"polynomial\": \"f(x)="<<a<<"x^2+"<<b<<"x+"<<c<<"\",\n";
    cout << "  \"coefficients\": {\"a\": "<<a<<", \"b\": "<<b<<", \"c\": "<<c<<"},\n";
    cout << "  \"roots\": [";
    for(size_t i=0;i<roots.size();i++){
        cout<<"\""<<roots[i]<<"\"";
        if(i+1<roots.size()) cout<<",";
    }
    cout << "],\n";

    cout << "  \"test_cases\": [\n";
    for(size_t i=0;i<testCases.size();i++){
        auto &t = testCases[i];
        cout<<"    {\"x\":\""<<t["x"]<<"\",\"base\":"
            <<t["base"]<<",\"value_raw\":\""<<t["value_raw"]
            <<"\",\"y_decimal\":\""<<t["y_decimal"]
            <<"\",\"f_x\":\""<<t["f_x"]<<"\"}";
        if(i+1<testCases.size()) cout<<",";
        cout<<"\n";
    }
    cout << "  ]\n";
    cout << "}\n";

    return 0;
}
