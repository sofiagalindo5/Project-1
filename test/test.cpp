// Name: Sofia Galindo
// ufid: 97453336 // <-- replace with your real ufid

#include <catch2/catch_test_macros.hpp>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <algorithm>

#include "CommandRunner.h" 
// You create this. It should declare:
// std::string runCommands(const std::string& input);

using namespace std;

// Helper: make 8-digit ufid strings like 00000042
static string ufid(int x) {
    ostringstream oss;
    oss << setw(8) << setfill('0') << x;
    return oss.str();
}

// Helper: build expected inorder output line with commas like "N00000000, N00000001\n"
static string line(const vector<string>& names) {
    string out;
    for (size_t i = 0; i < names.size(); i++) {
        out += names[i];
        if (i + 1 < names.size()) out += ", ";
    }
    out += "\n";
    return out;
}



TEST_CASE("Five+ command executions print unsuccessful") {
    
    ostringstream in;
    in
      << 6 << "\n"
      << "insert \"Ana\" 12345678\n"       
      << "insert \"A11y\" 45679999\n"           // invalid name 
      << "insert \"Lily\" 123\n"                // wrong id length
      << "insert \"Sofia\" 12345678\n"          // duplicate ufid 
      << "remove 22222222\n"                    // missing ufid 
      << "remove A2222222\n"                    // invalid ufid 
		
    

    string out = runCommands(in.str());

    // Counts "unsuccessful"
    size_t pos = 0;
    int count = 0;
    while (true) {
        pos = out.find("unsuccessful", pos);
        if (pos == string::npos) break;
        count++;
        pos += string("unsuccessful").size();
    }
    REQUIRE(count >= 5);
}

TEST_CASE("Insert triggers all four rotation cases (LL, RR, LR, RL)") {
    
    // if print the names in preorder

    SECTION("LL rotation: 30,20,10 -> root should become 20") {
        ostringstream in;
        in << 4 << "\n"
           << "insert \"n30\" 00000030\n"
           << "insert \"n20\" 00000020\n"
           << "insert \"n10\" 00000010\n"
           << "printPreorder\n";
        string out = runCommands(in.str());
        REQUIRE(out.find("n20, n10, n30") != string::npos);
    }

    SECTION("RR rotation: 10,20,30 -> root should become 20") {
        ostringstream in;
        in << 4 << "\n"
           << "insert \"n10\" 00000010\n"
           << "insert \"n20\" 00000020\n"
           << "insert \"n30\" 00000030\n"
           << "printPreorder\n";
        string out = runCommands(in.str());
        REQUIRE(out.find("n20, n10, n30") != string::npos);
    }

    SECTION("LR rotation: 30,10,20 -> root should become 20") {
        ostringstream in;
        in << 4 << "\n"
           << "insert \"n30\" 00000030\n"
           << "insert \"n10\" 00000010\n"
           << "insert \"n20\" 00000020\n"
           << "printPreorder\n";
        string out = runCommands(in.str());
        REQUIRE(out.find("n20, n10, n30") != string::npos);
    }

    SECTION("RL rotation: 10,30,20 -> root should become 20") {
        ostringstream in;
        in << 4 << "\n"
           << "insert \"n10\" 00000010\n"
           << "insert \"n30\" 00000030\n"
           << "insert \"n20\" 00000020\n"
           << "printPreorder\n";
        string out = runCommands(in.str());
        REQUIRE(out.find("n20, n10, n30") != string::npos);
    }
}

TEST_CASE("Insert 100 nodes, remove 10, verify inorder has remaining 90 in order", "[bulk][inorder]") {
    vector<int> removed = {7, 13, 22, 35, 41, 56, 63, 78, 84, 99};
    vector<bool> isRemoved(100, false);
    for (int x : removed) isRemoved[x] = true;

    
    // 100 inserts , 10 removes , 1 printInorder 
    ostringstream in;
    in << 111 << "\n";

    for (int i = 0; i < 100; i++) {
        string id = ufid(i);
        string name = "N" + id;
        in << "insert \"" << name << "\" " << id << "\n";
    }
    for (int x : removed) {
        in << "remove " << ufid(x) << "\n";
    }
    in << "printInorder\n";

    string out = runCommands(in.str());


    vector<string> expectedNames;
    expectedNames.reserve(90); //build expected line sorted w/o skipp
    for (int i = 0; i < 100; i++) {
        if (!isRemoved[i]) {
            string id = ufid(i);
            expectedNames.push_back("N" + id);
        }
    }
    string expectedLine = line(expectedNames);

    REQUIRE(out.find(expectedLine) != string::npos);
}
