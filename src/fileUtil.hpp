#pragma once
#include <algorithm>
#include <iterator>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include <chrono>

#include "Poco/File.h"

extern int debug;

enum class FILE_TYPE { MAIN, ATTACHMENT, ORIGINAL, JPM };

static const std::string MAIN_HTML_PREFIX = R"(a0)";
static const std::string ORIGINAL_HTML_PREFIX = R"(c0)";
static const std::string ATTACHMENT_HTML_PREFIX = R"(b0)";
static const std::string JPM_HTML_PREFIX = R"(d)";

static const std::string MAIN_BODYTEXT_PREFIX = R"(Main)";
static const std::string ORIGINAL_BODYTEXT_PREFIX = R"(Org)";
static const std::string ATTACHMENT_BODYTEXT_PREFIX = R"(Attach)";
static const std::string JPM_BODYTEXT_PREFIX = R"(Jpm)";

static const std::string MAIN = R"(main)";
static const std::string ORIGINAL = R"(original)";
static const std::string ATTACHMENT = R"(attachment)";
static const std::string JPM = R"(jpm)";

static const std::string MAIN_SEPERATOR_COLOR = R"(F0BEC0)";
static const std::string ORIGINAL_SEPERATOR_COLOR = R"(004040)";

static const std::string attachmentDirForLinkFromMain = R"(attachment\)";
static const std::string originalDirForLinkFromMain = R"(original\)";
static const std::string jpmDirForLinkFromMain = R"(JPM\)";
static const std::string pictureDirForLinkFromMain = R"(pictures\)";

static const std::string mainDirForLinkFromAttachment = R"(..\)";
static const std::string originalDirForLinkFromAttachment = R"(..\original\)";
static const std::string jpmDirForLinkFromAttachment = R"(..\JPM\)";
static const std::string pictureDirForLinkFromAttachment = R"(..\pictures\)";

std::string getHtmlFileNamePrefix(FILE_TYPE type);
std::string getSeparateLineColor(FILE_TYPE type);
FILE_TYPE getFileTypeFromString(const std::string &fileType);
