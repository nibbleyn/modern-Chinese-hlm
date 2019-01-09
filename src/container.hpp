#pragma once
#include "fileUtil.hpp"

class Container {
public:
  Container() = default;

protected:
  string htmlInputFilePath{""};
  string htmlOutputFilePath{""};
  string bodyTextInputFilePath{""};
  string bodyTextOutputFilePath{""};
};

/**
 * used for features like removePersonalViewpoints, numbering and linkfixing
 * etc.
 */
class CoupledContainer : public Container {

public:
  CoupledContainer() {
    htmlInputFilePath = HTML_SRC_MAIN;
    htmlOutputFilePath = HTML_OUTPUT_MAIN;
    bodyTextInputFilePath = BODY_TEXT_OUTPUT;
    bodyTextOutputFilePath = BODY_TEXT_FIX;
  }
  CoupledContainer(FILE_TYPE fileType) : fileType(fileType) {
    if (fileType == FILE_TYPE::ATTACHMENT) {
      htmlInputFilePath = HTML_SRC_ATTACHMENT;
      htmlOutputFilePath = HTML_OUTPUT_ATTACHMENT;
    } else if (fileType == FILE_TYPE::ORIGINAL) {
      htmlInputFilePath = HTML_SRC_ORIGINAL;
      htmlOutputFilePath = HTML_OUTPUT_ORIGINAL;
    } else {
      htmlInputFilePath = HTML_SRC_MAIN;
      htmlOutputFilePath = HTML_OUTPUT_MAIN;
    }
    bodyTextInputFilePath = BODY_TEXT_OUTPUT;
    bodyTextOutputFilePath = BODY_TEXT_FIX;
  }
  void dissembleFromHTM(const string &file, int attachNo = 1);
  void assembleBackToHTM(const string &file, int attachNo = 1,
                         const string &title = "",
                         const string &displayTitle = "");

private:
  FILE_TYPE fileType{FILE_TYPE::MAIN};
};

class GenericContainer : public Container {
public:
  GenericContainer() {
    htmlInputFilePath = HTML_CONTAINER;
    htmlOutputFilePath = HTML_CONTAINER;
    bodyTextInputFilePath = BODY_TEXT_CONTAINER;
    bodyTextOutputFilePath = BODY_TEXT_CONTAINER;
  }
  virtual ~GenericContainer(){};
  virtual string getInputFileName() const = 0;
  void assembleBackToHTM(const string &title = "",
                         const string &displayTitle = "");

protected:
  string outputFilename{"output"};
};

/**
 * used for features like reConstructStory findFirstInNoAttachmentFiles etc.
 */
class ListContainer : public GenericContainer {
public:
  ListContainer() = default;
  ListContainer(string filename) { outputFilename = filename; }

private:
  string getInputFileName() const override { return "1"; }
};

/**
 * used for features like updateIndexTable etc.
 */
class TableContainer : public GenericContainer {
public:
  TableContainer() = default;

private:
  string getInputFileName() const override { return "2"; }
};

void dissembleAttachments(int minTarget, int maxTarget, int minAttachNo,
                          int maxAttachNo);
void assembleAttachments(int minTarget, int maxTarget, int minAttachNo,
                         int maxAttachNo);
