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

enum class FILE_TYPE { MAIN, ATTACHMENT, ORIGINAL, JPM };

static const string MAIN_HTML_PREFIX = "a0";
static const string ORIGINAL_HTML_PREFIX = "c0";
static const string ATTACHMENT_HTML_PREFIX = "b0";
static const string JPM_HTML_PREFIX = "d";

static const string MAIN_BODYTEXT_PREFIX = "Main";
static const string ORIGINAL_BODYTEXT_PREFIX = "Org";
static const string ATTACHMENT_BODYTEXT_PREFIX = "Attach";
static const string JPM_BODYTEXT_PREFIX = "Jpm";

static const string MAIN = "main";
static const string ORIGINAL = "original";
static const string ATTACHMENT = "attachment";
static const string JPM = "jpm";

static const string MAIN_SEPERATOR_COLOR = "F0BEC0";
static const string ORIGINAL_SEPERATOR_COLOR = "004040";

static const string attachmentDirForLinkFromMain = R"(attachment\)";
static const string originalDirForLinkFromMain = R"(original\)";

static const string mainDirForLinkFromAttachment = R"(..\)";
static const string originalDirForLinkFromAttachment = R"(..\original\)";

string getHtmlFileNamePrefix(FILE_TYPE type);
string getSeparateLineColor(FILE_TYPE type);
FILE_TYPE getFileTypeFromString(const string &fileType);
