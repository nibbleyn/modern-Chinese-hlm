#include "hlmGb2312.hpp"
#include "Poco/File.h"
#include "Poco/PipeStream.h"
#include "Poco/Process.h"
#include "Poco/StreamCopier.h"
using Poco::Process;
using Poco::ProcessHandle;

/**
 * convert gb2312 format FILE_TYPE::MAIN files to utf8 format
 * @param referFile
 * @param format
 * @param type
 * @param attachNo
 */
void convertFromGB2312ToUtf8(string referFile, string format, FILE_TYPE type,
                             int attachNo) {
  string inputFile{""}, outputFile{""};
  if (format == "htm") {
    string attachmentPart{""};
    if (type == FILE_TYPE::ATTACHMENT)
      attachmentPart = "_" + TurnToString(attachNo);
    inputFile = GB2312_HTML_SRC + getFileNamePrefix(type) + referFile +
                attachmentPart + ".htm";
    cout << inputFile << endl;
    ifstream infile(inputFile);
    if (!infile) // doesn't exist
    {
      cout << "file doesn't exist:" << inputFile << endl;
      return;
    }
    outputFile = HTML_OUTPUT;
    if (type == FILE_TYPE::ATTACHMENT)
      outputFile = HTML_OUTPUT_ATTACHMENT;
    outputFile += getFileNamePrefix(type) + referFile + attachmentPart + ".htm";
    cout << outputFile << endl;
  }
  convertFromGB2312ToUtf8(inputFile, outputFile);
}

/**
 *
 * @param inputFile
 * @param outputFile
 */
void convertFromGB2312ToUtf8(string inputFile, string outputFile) {
  cout << inputFile << endl;
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
  cout << outputFile << endl;
  ofstream ostr(outputFile);
  Poco::StreamCopier::copyStream(istr, ostr);
}

/**
 *
 */
void convertMainMenuFromGB2312ToUtf8() {
  string inputFile{""}, outputFile{""};
  inputFile = GB2312_HTML_SRC + "aindex.htm";
  outputFile = HTML_OUTPUT + "aindex.htm";
  convertFromGB2312ToUtf8(inputFile, outputFile);
}

/**
 *
 */
void convertAttachmentMainMenuFromGB2312ToUtf8() {
  string inputFile{""}, outputFile{""};
  inputFile = GB2312_HTML_SRC + "bttindex0.htm";
  outputFile = HTML_OUTPUT_ATTACHMENT + "bttindex0.htm";
  convertFromGB2312ToUtf8(inputFile, outputFile);
  inputFile = GB2312_HTML_SRC + "bttindex1.htm";
  outputFile = HTML_OUTPUT_ATTACHMENT + "bttindex1.htm";
  convertFromGB2312ToUtf8(inputFile, outputFile);
}

/**
 *
 * @param minTarget
 * @param maxTarget
 */
void gb2312FixMain(int minTarget, int maxTarget) {
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    convertFromGB2312ToUtf8(file, "htm", FILE_TYPE::MAIN);
  }
}

/**
 *
 * @param minTarget
 * @param maxTarget
 */
void gb2312FixOriginal(int minTarget, int maxTarget) {
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    string inputFile = GB2312_HTML_SRC + "original/" +
                       getFileNamePrefix(FILE_TYPE::MAIN) + file + ".htm";
    string outputFile = "utf8HTML/original/" +
                        getFileNamePrefix(FILE_TYPE::MAIN) + file + ".htm";
    convertFromGB2312ToUtf8(inputFile, outputFile);
  }
}

/**
 *
 */
void gb2312FixJPM() {
  for (int i = 1; i <= 100; i++) {
    string inputFile = GB2312_HTML_SRC + "JPM/" + TurnToString(i) + ".html";
    string outputFile = "utf8HTML/JPM/" + TurnToString(i) + ".html";
    convertFromGB2312ToUtf8(inputFile, outputFile);
  }
}

/**
 *
 * @param minTarget
 * @param maxTarget
 */
void gb2312FixAttachment(int minTarget, int maxTarget) {
  for (const auto &file : buildFileSet(minTarget, maxTarget)) {
    auto attList = getAttachmentFileListForChapter(file, GB2312_HTML_SRC);
    for (const auto &attNo : attList)
      convertFromGB2312ToUtf8(file, "htm", FILE_TYPE::ATTACHMENT, attNo);
  }
}

/**
 *
 */
void convertAllFromGb2312ToUtf8() {
  gb2312FixMain(1, 80);
  gb2312FixAttachment(1, 80);
  convertAttachmentMainMenuFromGB2312ToUtf8();
  convertMainMenuFromGB2312ToUtf8();
}