#pragma once
#include <fstream>
#include <iostream>

#include <algorithm>
#include <map>
#include <memory>
#include <vector>

#include "Poco/File.h"
#include <chrono>

extern int debug;

static const std::string HTML_SRC_MAIN = R"(utf8HTML/src/)";
static const std::string HTML_SRC_ATTACHMENT = R"(utf8HTML/src/attachment/)";
static const std::string HTML_SRC_ORIGINAL = R"(utf8HTML/src/original/)";
static const std::string HTML_SRC_JPM = R"(utf8HTML/src/JPM/)";

static const std::string HTML_OUTPUT_MAIN = R"(utf8HTML/output/)";
static const std::string HTML_OUTPUT_ATTACHMENT =
    R"(utf8HTML/output/attachment/)";
static const std::string HTML_OUTPUT_ORIGINAL = R"(utf8HTML/output/original/)";
static const std::string HTML_OUTPUT_JPM = R"(utf8HTML/output/JPM/)";

static const std::string BODY_TEXT_OUTPUT = R"(bodyTexts/output/)";
static const std::string BODY_TEXT_FIX = R"(bodyTexts/afterFix/)";
static const std::string BODY_TEXT_SUFFIX = R"(.txt)";

static const std::string HTML_SUFFIX = R"(.htm)";
// last line of the html file
static const std::string htmlFileFinalLine = R"(</html>)";
static const std::string endOfHtmlHead = R"(/head)";
static const std::string htmlTitleStart = R"(<title>)";
static const std::string htmlTitleEnd = R"(</title>)";

static const int MAIN_MIN_CHAPTER_NUMBER = 1;
static const int MAIN_MAX_CHAPTER_NUMBER = 80;
static const int JPM_MIN_CHAPTER_NUMBER = 1;
static const int JPM_MAX_CHAPTER_NUMBER = 100;
static const int MIN_ATTACHMENT_NUMBER = 1;
static const int MAX_ATTACHMENT_NUMBER = 50;

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

std::string getAttachmentTitle(const std::string &filename);

enum class FILE_TYPE { MAIN, ATTACHMENT, ORIGINAL, JPM };

std::string getHtmlFileNamePrefix(FILE_TYPE type);
std::string getSeparateLineColor(FILE_TYPE type);
FILE_TYPE getFileTypeFromString(const std::string &fileType);
