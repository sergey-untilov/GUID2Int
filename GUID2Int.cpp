#include <iostream>
#include <fstream>
#include <string>
#include <filesystem> // C++17
#include <regex>
#include <map>

namespace fs = std::filesystem;
using namespace std;

const string FILE_EXT = ".csv";

const std::regex GUID_PATTERN("\\b[0-9a-f]{8}\\b-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-\\b[0-9a-f]{12}\\b", std::regex::ECMAScript);

static unsigned int counter = 0;
static map<string, string> guidMap;
string getID(const string guid);

int main()
{
    const string sourcePath = "./";
    const string destinationPath = "./";
    for (const auto& source : fs::directory_iterator(sourcePath))
    {
        const string fileExt = source.path().extension().string();
        if (!fileExt.compare(FILE_EXT))
        {
            bool isChanged = false;
            const string sourceFileName = source.path().string();
            ifstream sourceFile(sourceFileName);
            if (sourceFile.is_open())
            {
                cout << sourceFileName << endl;
                const string tmpFileName = destinationPath + source.path().filename().string() + ".tmp";
                ofstream tmpFile(tmpFileName);
                if (tmpFile.is_open()) {
                    string line;
                    while (getline(sourceFile, line))
                    {
                        std::smatch found;
                        while (regex_search(line, found, GUID_PATTERN, std::regex_constants::format_first_only))
                        {
                            const string guid = found[0];
                            const string ID = getID(guid);
                            line = regex_replace(line, GUID_PATTERN, ID, std::regex_constants::format_first_only);
                            isChanged = true;
                        }
                        tmpFile << line << endl;
                    }
                    tmpFile.close();
                }
                sourceFile.close();
                if (isChanged) {
                    const string newFileName = destinationPath + source.path().filename().string();
                    if (!newFileName.compare(sourceFileName)) {
                        remove(sourceFileName.c_str());
                    }
                    if (rename(tmpFileName.c_str(), newFileName.c_str())) {
                        cout << "Couldn't rename file: " << tmpFileName << endl;
                    }
                }
                else {
                    remove(tmpFileName.c_str());
                }
            }
            else
                cout << "Unable to open file: " << sourceFileName << endl;
        }
    }
    return 0;
}

string getID(const string guid)
{
    auto found = guidMap.find(guid);
    if (found != guidMap.end()) {
        return found->second;
    }
    const string ID = to_string(++counter);
    guidMap.insert(pair{ guid, ID });
    return ID;
}