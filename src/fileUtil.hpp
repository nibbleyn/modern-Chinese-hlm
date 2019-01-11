#pragma once
#include <algorithm>
#include <iterator>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include <chrono>

#include "Poco/File.h"

using namespace std;
extern int debug;

enum class FILE_TYPE { MAIN, ATTACHMENT, ORIGINAL };

string getFileNamePrefixFromFileType(FILE_TYPE type);
string getBodyTextFilePrefixFromFileType(FILE_TYPE type);
string getSeparateLineColor(FILE_TYPE type);
FILE_TYPE getFileTypeFromString(const string &fileType);
