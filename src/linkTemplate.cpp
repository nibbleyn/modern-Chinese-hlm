#include "link.hpp"
extern int debug;

/**
 * there is only one template for all display types supported
 * so this function is to change it into right hidden or unhidden type
 * @param linkString the link with <a unhidden" to replace
 * @param type the display type of the link, visible or not
 * @return the replaced link accordingly
 */
void replaceDisplayLink(string &linkString, DISPLAY_TYPE type) {
  if (type == DISPLAY_TYPE::UNHIDDEN)
    return;

  string displayTag = linkStartChars + displaySpace + unhiddenDisplayProperty;
  auto displayTagBegin = linkString.find(displayTag);
  linkString.replace(displayTagBegin, displayTag.length(),
                     (type == DISPLAY_TYPE::DIRECT)
                         ? linkStartChars
                         : linkStartChars + displaySpace +
                               hiddenDisplayProperty);
}

static constexpr const char *linkToImageFile =
    R"(<a unhidden title="IMAGE" href="XX#YY">↑（图示：ZZ）</a>)";
static constexpr const char *linkToAttachmentFile =
    R"(<a unhidden href="PPb0XX_SS.htm#YY">ZZ</a>)";
static constexpr const char *linkToSameFile =
    R"(<a unhidden title="QQ" href="#YY"><sub hidden>WW</sub>ZZ</a>)";
static constexpr const char *linkToMainFile =
    R"(<a unhidden title="QQ" href="PPa0XX.htm#YY">↑<sub hidden>WW</sub>ZZ</a>)";
static constexpr const char *linkToOriginalFile =
    R"(<a unhidden title="QQ" href="PPc0XX.htm#YY">↑<sub hidden>WW</sub>ZZ</a>)";
static constexpr const char *linkToJPMFile =
    R"(<a unhidden title="QQ" href="PPdXXX.htm#YY">↑<sub hidden>WW</sub>ZZ</a>)";
// now only support reverse link from main back to main, no key required
static constexpr const char *reverseLinkToMainFile =
    R"(<a unhidden href="a0XX.htm#YY">↓<sub>WW</sub>引用</a>)";

static constexpr const char *defaultPath = R"(PP)";
static constexpr const char *defaultMainFilename = R"(XX)";
static constexpr const char *defaultAttachmentNumber = R"(SS)";
static constexpr const char *defaultJpmFilename = R"(XXX)";
static constexpr const char *defaultKey = R"(QQ)";
static constexpr const char *defaultWholeKey = R"( title="QQ")";
static constexpr const char *defaultReferPara = R"(YY)";
static constexpr const char *defaultWholeReferPara = R"(#YY)";
static constexpr const char *defaultCitation = R"(WW)";
static constexpr const char *defaultWholeCitation = R"(↑<sub hidden>WW</sub>)";
static constexpr const char *defaultAnnotation = R"(ZZ)";
static constexpr const char *defaultImageAnnotation = R"(（图示：ZZ）)";

/**
 * generate real correct link within same file
 * by filling right "refer para" based on key searching
 * NOTE: avoid literal strings of YY QQ WW ZZ in the original link
 * @param type display type
 * @param key the key to use in search within this same file
 * @param citation the target place identified by section number
 * @param annotation the original annotation
 * @param referPara the target place identified by line number
 * @return link after fixed
 */
string fixLinkFromSameFileTemplate(DISPLAY_TYPE type, const string &key,
                                   const string &citation,
                                   const string &annotation,
                                   const string &referPara) {
  string link = linkToSameFile;
  replaceDisplayLink(link, type);
  if (referPara.empty()) {
    replacePart(link, defaultWholeReferPara, emptyString);
  } else
    replacePart(link, defaultReferPara, referPara);
  // in case use top/bottom as reference name
  if (key.empty()) {
    replacePart(link, defaultWholeCitation, emptyString);
    replacePart(link, defaultWholeKey, emptyString);
  } else {
    replacePart(link, defaultKey, key);
  }
  replacePart(link, defaultCitation, citation);
  replacePart(link, defaultAnnotation, annotation);
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << link << endl;
  return link;
}

/**
 * generate real correct link to other main file
 * by filling right "refer para" based on key searching
 * NOTE: avoid literal strings of QQ PP XX YY WW ZZ
 * in the original link to get a unexpected replacement
 * @param path relative path based on the link type
 * refer to getPathOfReferenceFile()
 * @param filename the target main file's chapter name, e.g. 07
 * @param type display type
 * @param key the key to use in search over target main file
 * @param citation the target place identified by section number
 * @param annotation the original annotation
 * @param referPara the target place identified by line number
 * @return link after fixed
 */
string fixLinkFromMainTemplate(const string &path, const string &filename,
                               DISPLAY_TYPE type, const string &key,
                               const string &citation, const string &annotation,
                               const string &referPara) {
  string link = linkToMainFile;
  replaceDisplayLink(link, type);
  replacePart(link, defaultPath, path);
  replacePart(link, defaultMainFilename, filename);
  if (referPara.empty()) {
    replacePart(link, defaultWholeReferPara, emptyString);
  } else
    replacePart(link, defaultReferPara, referPara);
  // in case use top/bottom as reference name
  if (key.empty()) {
    replacePart(link, defaultWholeCitation, emptyString);
    replacePart(link, defaultWholeKey, emptyString);
  } else {
    replacePart(link, defaultKey, key);
  }
  replacePart(link, defaultCitation, citation);
  replacePart(link, defaultAnnotation, annotation);
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << link << endl;
  return link;
}

string fixLinkFromReverseLinkTemplate(const string &filename, DISPLAY_TYPE type,
                                      const string &citation,
                                      const string &annotation,
                                      const string &referPara) {
  string link = reverseLinkToMainFile;
  replaceDisplayLink(link, type);
  replacePart(link, defaultMainFilename, filename);
  if (referPara.empty()) {
    replacePart(link, defaultWholeReferPara, emptyString);
  } else
    replacePart(link, defaultReferPara, referPara);
  replacePart(link, defaultCitation, citation);
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << link << endl;
  return link;
}

/**
 * generate real correct link to original file
 * by filling right "refer para" based on key searching
 * NOTE: avoid literal strings of QQ PP XX YY WW
 * in the original link to get a unexpected replacement
 * @param path relative path based on the link type
 * refer to getPathOfReferenceFile()
 * @param filename the target origin file's chapter name, e.g. 07
 * @param key the key to use in search over target original file
 * @param citation the target place identified by section number
 * @param referPara the target place identified by line number
 * @param annotation the original annotation
 * @return link after fixed
 */
string fixLinkFromOriginalTemplate(const string &path, const string &filename,
                                   const string &key, const string &citation,
                                   const string &annotation,
                                   const string &referPara) {
  string link = linkToOriginalFile;
  replacePart(link, defaultPath, path);
  replacePart(link, defaultMainFilename, filename);
  if (referPara.empty()) {
    replacePart(link, defaultWholeReferPara, emptyString);
  } else
    replacePart(link, defaultReferPara, referPara);
  // in case use top/bottom as reference name
  if (key.empty()) {
    replacePart(link, defaultWholeCitation, emptyString);
    replacePart(link, defaultWholeKey, emptyString);
  } else {
    replacePart(link, defaultKey, key);
  }
  replacePart(link, defaultCitation, citation);
  replacePart(link, defaultAnnotation, annotation);
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << link << endl;
  return link;
}

/**
 * generate real correct link to jpm file
 * by filling right "refer para" based on key searching
 * NOTE: avoid literal strings of QQ PP XXX YY WW ZZ
 * in the original link to get a unexpected replacement
 * @param path relative path based on the link type
 * refer to getPathOfReferenceFile()
 * @param filename the target origin file's chapter name, e.g. 07
 * @param key the key to use in search over target original file
 * @param citation the target place identified by section number
 * @param referPara the target place identified by line number
 * @param annotation the original annotation
 * @return link after fixed
 */
string fixLinkFromJPMTemplate(const string &path, const string &filename,
                              const string &key, const string &citation,
                              const string &annotation,
                              const string &referPara) {
  string link = linkToJPMFile;
  replacePart(link, defaultPath, path);
  replacePart(link, defaultJpmFilename, filename);
  if (referPara.empty()) {
    replacePart(link, defaultWholeReferPara, emptyString);
  } else
    replacePart(link, defaultReferPara, referPara);
  // in case use top/bottom as reference name
  if (key.empty()) {
    replacePart(link, defaultWholeCitation, emptyString);
    replacePart(link, defaultWholeKey, emptyString);
  } else {
    replacePart(link, defaultKey, key);
  }
  if (citation.empty()) {
    replacePart(link, defaultWholeCitation, emptyString);
  } else
    replacePart(link, defaultCitation, citation);
  replacePart(link, defaultAnnotation, annotation);
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << link << endl;
  return link;
}

/**
 * generate real correct link to attachment file
 * NOTE: avoid literal strings of PP XX YY ZZ
 * in the original link to get a unexpected replacement
 * @param path relative path based on the link type
 * refer to getPathOfReferenceFile()
 * @param filename the target attachment file's chapter name,
 * e.g. 07 of b007_3
 * @param attachNo the target attachment file's attachment index,
 * e.g. 3 of b007_3
 * @param annotation the original annotation
 * @return link after fixed
 */
string fixLinkFromAttachmentTemplate(const string &path, const string &filename,
                                     const string &attachNo,
                                     const string &annotation,
                                     const string &referPara) {
  string link = linkToAttachmentFile;
  replacePart(link, defaultPath, path);
  replacePart(link, defaultMainFilename, filename);
  replacePart(link, defaultAttachmentNumber, attachNo);
  if (referPara.empty()) {
    replacePart(link, defaultWholeReferPara, emptyString);
  } else
    replacePart(link, defaultReferPara, referPara);
  replacePart(link, defaultAnnotation, annotation);
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << link << endl;
  return link;
}

/**
 * Note: do not fix path now
 */
string fixLinkFromImageTemplate(const string &fullReferFilenameWithPathExt,
                                const string &picFilename,
                                const string &fullAnnotation,
                                const string &displayProperty) {
  string link = linkToImageFile;
  if (displayProperty != unhiddenDisplayProperty) {
    if (displayProperty == hiddenDisplayProperty)
      replacePart(link, unhiddenDisplayProperty, hiddenDisplayProperty);
    else
      // direct
      replacePart(link, unhiddenDisplayProperty + displaySpace, emptyString);
  }
  replacePart(link, defaultMainFilename, fullReferFilenameWithPathExt);
  replacePart(link, defaultReferPara, picFilename);
  replacePart(link, defaultImageAnnotation, fullAnnotation);
  return link;
}
