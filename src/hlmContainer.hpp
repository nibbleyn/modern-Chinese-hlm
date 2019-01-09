#include "hlmFileType.hpp"

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
 * used for features like removePersonalViewpoints, numbering and linkfixing  etc.
 */
class CoupledContainer : public Container {

public:
  CoupledContainer() {
    htmlInputFilePath = HTML_SRC_MAIN;
    htmlOutputFilePath = HTML_OUTPUT_MAIN;
    bodyTextInputFilePath = BODY_TEXT_OUTPUT;
    bodyTextOutputFilePath = BODY_TEXT_FIX;
  }
  CoupledContainer(const string &fileType) : fileType(fileType) {
    if (fileType == "attachment") {
      htmlInputFilePath = HTML_SRC_ATTACHMENT;
      htmlOutputFilePath = HTML_OUTPUT_ATTACHMENT;
    } else if (fileType == "original") {
      htmlInputFilePath = HTML_SRC_ORIGINAL;
      htmlOutputFilePath = HTML_OUTPUT_ORIGINAL;

    } else {
      htmlInputFilePath = HTML_SRC_MAIN;
      htmlOutputFilePath = HTML_OUTPUT_MAIN;
    }
    bodyTextInputFilePath = BODY_TEXT_OUTPUT;
    bodyTextOutputFilePath = BODY_TEXT_FIX;
  }

private:
  string fileType{""};
};

class GenericContainer : public Container {
public:
  GenericContainer() {
    htmlInputFilePath = HTML_CONTAINER;
    htmlOutputFilePath = HTML_CONTAINER;
    bodyTextInputFilePath = BODY_TEXT_CONTAINER;
    bodyTextOutputFilePath = BODY_TEXT_CONTAINER;
  }
};

/**
 * used for features like updateIndexTable etc.
 */
class TableContainer : public GenericContainer {
public:
  TableContainer() = default;
};

/**
 * used for features like reConstructStory findFirstInNoAttachmentFiles etc.
 */
class ListContainer : public GenericContainer {
public:
  ListContainer() = default;
};
