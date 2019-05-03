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
static const std::string HTML_SRC_ORIGINAL = R"(utf8HTML/src/original/)";
static const std::string HTML_SRC_JPM = R"(utf8HTML/src/JPM/)";

static const std::string HTML_OUTPUT_MAIN = R"(utf8HTML/output/)";
static const std::string HTML_OUTPUT_ATTACHMENT =
    R"(utf8HTML/output/attachment/)";
static const std::string HTML_OUTPUT_ORIGINAL = R"(utf8HTML/output/original/)";
static const std::string HTML_OUTPUT_JPM = R"(utf8HTML/output/JPM/)";

static const std::string BODY_TEXT_OUTPUT = R"(bodyTexts/output/)";
static const std::string BODY_TEXT_FIX = R"(bodyTexts/afterFix/)";

// last line of the html file
static const std::string htmlFileFinalLine = R"(</html>)";

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

static const std::string HTML_OUTPUT_LINES_OF_MAIN =
    R"(utf8HTML/output/LinesOfMain.txt)";
static const std::string HTML_OUTPUT_LINES_OF_JPM =
    R"(utf8HTML/output/LinesOfJPM.txt)";
static const std::string HTML_OUTPUT_LINES_OF_ORIGINAL =
    R"(utf8HTML/output/LinesOfOriginal.txt)";
static const std::string HTML_OUTPUT_LINES_OF_ATTACHMENTS =
    R"(utf8HTML/output/LinesOfAttachments.txt)";

enum class FILE_TYPE { MAIN, ATTACHMENT, ORIGINAL, JPM };

std::string getHtmlFileNamePrefix(FILE_TYPE type);
std::string getSeparateLineColor(FILE_TYPE type);
FILE_TYPE getFileTypeFromString(const std::string &fileType);
std::string getFilePrefixFromFileType(FILE_TYPE type);
std::string getStatisticsOutputFilePathFromString(const std::string &fileType);
