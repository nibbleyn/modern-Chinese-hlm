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

static constexpr const char *HTML_SRC_MAIN = R"(utf8HTML/src/)";
static constexpr const char *HTML_SRC_ORIGINAL = R"(utf8HTML/src/original/)";
static constexpr const char *HTML_SRC_JPM = R"(utf8HTML/src/JPM/)";

static constexpr const char *HTML_OUTPUT_MAIN = R"(utf8HTML/output/)";
static constexpr const char *HTML_OUTPUT_ATTACHMENT =
    R"(utf8HTML/output/attachment/)";
static constexpr const char *HTML_OUTPUT_ORIGINAL =
    R"(utf8HTML/output/original/)";
static constexpr const char *HTML_OUTPUT_JPM = R"(utf8HTML/output/JPM/)";

static constexpr const char *BODY_TEXT_OUTPUT = R"(bodyTexts/output/)";
static constexpr const char *BODY_TEXT_FIX = R"(bodyTexts/afterFix/)";

// last line of the html file
static constexpr const char *htmlFileFinalLine = R"(</html>)";

static constexpr const char *MAIN_HTML_PREFIX = R"(a0)";
static constexpr const char *ORIGINAL_HTML_PREFIX = R"(c0)";
static constexpr const char *ATTACHMENT_HTML_PREFIX = R"(b0)";
static constexpr const char *JPM_HTML_PREFIX = R"(d)";

static constexpr const char *MAIN_BODYTEXT_PREFIX = R"(Main)";
static constexpr const char *ORIGINAL_BODYTEXT_PREFIX = R"(Org)";
static constexpr const char *ATTACHMENT_BODYTEXT_PREFIX = R"(Attach)";
static constexpr const char *JPM_BODYTEXT_PREFIX = R"(Jpm)";

static constexpr const char *MAIN = R"(main)";
static constexpr const char *ORIGINAL = R"(original)";
static constexpr const char *ATTACHMENT = R"(attachment)";
static constexpr const char *JPM = R"(jpm)";
static constexpr const char *GENERATED = R"(_generated)";
static constexpr const char *NUMBERED = R"(_numbered)";

static constexpr const char *MAIN_SEPERATOR_COLOR = R"(F0BEC0)";
static constexpr const char *ORIGINAL_SEPERATOR_COLOR = R"(004040)";

static constexpr const char *HTML_OUTPUT_LINES_OF_MAIN =
    R"(utf8HTML/output/LinesOfMain.txt)";
static constexpr const char *HTML_OUTPUT_LINES_OF_JPM =
    R"(utf8HTML/output/LinesOfJPM.txt)";
static constexpr const char *HTML_OUTPUT_LINES_OF_ORIGINAL =
    R"(utf8HTML/output/LinesOfOriginal.txt)";
static constexpr const char *HTML_OUTPUT_LINES_OF_ATTACHMENTS =
    R"(utf8HTML/output/LinesOfAttachments.txt)";

enum class FILE_TYPE { MAIN, ATTACHMENT, ORIGINAL, JPM };

std::string getHtmlFileNamePrefix(FILE_TYPE type);
std::string getSeparateLineColor(FILE_TYPE type);
FILE_TYPE getFileTypeFromString(const std::string &fileType);
std::string getFilePrefixFromFileType(FILE_TYPE type);
std::string getStatisticsOutputFilePathFromString(const std::string &fileType);
