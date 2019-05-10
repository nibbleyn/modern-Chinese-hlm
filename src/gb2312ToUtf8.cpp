#include "gb2312ToUtf8.hpp"
#include "Poco/File.h"
#include "Poco/PipeStream.h"
#include "Poco/Process.h"
#include "Poco/StreamCopier.h"
using Poco::Process;
using Poco::ProcessHandle;

void convertFromGB2312ToUtf8(string inputFile, string outputFile) {
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << inputFile << endl;
  string cmd("iconv");
  vector<string> args;
  args.push_back("-c");
  args.push_back(inputFile);
  args.push_back("-f");
  args.push_back("gb2312");
  args.push_back("-t");
  args.push_back("utf8");
  Poco::Pipe outPipe;
  ProcessHandle ph = Process::launch(cmd, args, 0, &outPipe, 0);
  Poco::PipeInputStream istr(outPipe);
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << outputFile << endl;
  ofstream ostr(outputFile);
  Poco::StreamCopier::copyStream(istr, ostr);
}

/**
 * all files are converted from GB2312_HTML_SRC whatever type is
 */
void convertFromGB2312ToUtf8(string referFile, string format, FILE_TYPE type,
                             int attachNo = MIN_ATTACHMENT_NUMBER) {
  string inputFile{""}, outputFile{""};
  if (format == "htm") {
    string attachmentPart{""};
    if (type == FILE_TYPE::ATTACHMENT)
      attachmentPart = attachmentFileMiddleChar + TurnToString(attachNo);
    inputFile = GB2312_HTML_SRC + getHtmlFileNamePrefix(type) + referFile +
                attachmentPart + HTML_SUFFIX;
    if (debug >= LOG_INFO)
      FUNCTION_OUTPUT << inputFile << endl;
    ifstream infile(inputFile);
    if (!infile) {
      FUNCTION_OUTPUT << ERROR_FILE_NOT_EXIST << inputFile << endl;
      return;
    }
    outputFile = HTML_OUTPUT_MAIN;
    if (type == FILE_TYPE::ATTACHMENT)
      outputFile = HTML_OUTPUT_ATTACHMENT;
    if (type == FILE_TYPE::ORIGINAL)
      outputFile = HTML_OUTPUT_ORIGINAL;
    if (type == FILE_TYPE::JPM)
      outputFile = HTML_OUTPUT_JPM;
    outputFile +=
        getHtmlFileNamePrefix(type) + referFile + attachmentPart + HTML_SUFFIX;
    if (debug >= LOG_INFO)
      FUNCTION_OUTPUT << outputFile << endl;
  }
  convertFromGB2312ToUtf8(inputFile, outputFile);
}

void convertNonPrefixedAttachmentFilesFromGB2312ToUtf8() {
  string inputFile{""}, outputFile{""};
  inputFile = GB2312_HTML_SRC + "b00001_1.htm";
  outputFile = HTML_OUTPUT_ATTACHMENT + "b00001_1.htm";
  convertFromGB2312ToUtf8(inputFile, outputFile);
}

void convertNonPrefixedMainFilesFromGB2312ToUtf8() {
  string inputFile{""}, outputFile{""};
  inputFile = GB2312_HTML_SRC + "a00001.htm";
  outputFile = HTML_OUTPUT_MAIN + "a00001.htm";
  convertFromGB2312ToUtf8(inputFile, outputFile);
  inputFile = GB2312_HTML_SRC + "a0001.htm";
  outputFile = HTML_OUTPUT_MAIN + "a0001.htm";
  convertFromGB2312ToUtf8(inputFile, outputFile);
}

void convertNonPrefixedOriginalFilesFromGB2312ToUtf8() {
  string inputFile{""}, outputFile{""};
  inputFile = GB2312_HTML_SRC + "c00001.htm";
  outputFile = HTML_OUTPUT_ORIGINAL + "c00001.htm";
  convertFromGB2312ToUtf8(inputFile, outputFile);
  inputFile = GB2312_HTML_SRC + "c0001.htm";
  outputFile = HTML_OUTPUT_ORIGINAL + "c0001.htm";
  convertFromGB2312ToUtf8(inputFile, outputFile);
}

void convertMainMenuFromGB2312ToUtf8() {
  string inputFile{""}, outputFile{""};
  inputFile = GB2312_HTML_SRC + "aindex.htm";
  outputFile = HTML_OUTPUT_MAIN + "aindex.htm";
  convertFromGB2312ToUtf8(inputFile, outputFile);
}

void convertAttachmentMainMenuFromGB2312ToUtf8() {
  string inputFile{""}, outputFile{""};
  inputFile = GB2312_HTML_SRC + "bttindex0.htm";
  outputFile = HTML_OUTPUT_ATTACHMENT + "bttindex0.htm";
  convertFromGB2312ToUtf8(inputFile, outputFile);
  inputFile = GB2312_HTML_SRC + "bttindex1.htm";
  outputFile = HTML_OUTPUT_ATTACHMENT + "bttindex1.htm";
  convertFromGB2312ToUtf8(inputFile, outputFile);
}

void gb2312FixMain(int minTarget, int maxTarget) {
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    convertFromGB2312ToUtf8(file, "htm", FILE_TYPE::MAIN);
  }
}

void gb2312FixOriginal(int minTarget, int maxTarget) {
  for (const auto &file : buildFileSet(minTarget, maxTarget, ORIGINAL)) {
    convertFromGB2312ToUtf8(file, "htm", FILE_TYPE::ORIGINAL);
  }
}

void gb2312FixJPM(int minTarget, int maxTarget) {
  for (int i = minTarget; i <= maxTarget; i++) {
    string inputFile = GB2312_HTML_SRC + "JPM/" + TurnToString(i) + HTML_SUFFIX;
    string outputFile = HTML_OUTPUT_JPM + TurnToString(i) + HTML_SUFFIX;
    convertFromGB2312ToUtf8(inputFile, outputFile);
  }
}

void gb2312FixAttachment(int minTarget, int maxTarget) {
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    CoupledBodyTextContainer container;
    container.setFileType(FILE_TYPE::ATTACHMENT);
    container.setFileAndAttachmentNumber(file);
    auto attList =
        getAttachmentFileListForChapter(GB2312_HTML_SRC, TurnToInt(file));
    for (const auto &attNo : attList)
      convertFromGB2312ToUtf8(file, "htm", FILE_TYPE::ATTACHMENT, attNo);
  }
}

void ConvertPrefixedGb2312FilesToUtf8() {
  gb2312FixMain(MAIN_MIN_CHAPTER_NUMBER, MAIN_MAX_CHAPTER_NUMBER);
  gb2312FixAttachment(MAIN_MIN_CHAPTER_NUMBER, MAIN_MAX_CHAPTER_NUMBER);
  gb2312FixJPM(JPM_MIN_CHAPTER_NUMBER, JPM_MAX_CHAPTER_NUMBER);
  gb2312FixOriginal(MAIN_MIN_CHAPTER_NUMBER, MAIN_MAX_CHAPTER_NUMBER);
}

void ConvertNonPrefixedGb2312FilesToUtf8() {
  convertNonPrefixedAttachmentFilesFromGB2312ToUtf8();
  convertNonPrefixedMainFilesFromGB2312ToUtf8();
  convertNonPrefixedOriginalFilesFromGB2312ToUtf8();
}
