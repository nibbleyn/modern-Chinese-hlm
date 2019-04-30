#include "link.hpp"

extern int debug;

/**
 * there is only one template for all display types supported
 * so this function is to change it into right hidden or unhidden type
 * @param linkString the link with <a unhidden" to replace
 * @param type the display type of the link, visible or not
 * @return the replaced link accordingly
 */
void replaceDisplayLink(string &linkString, LINK_DISPLAY_TYPE type) {
  if (type == LINK_DISPLAY_TYPE::UNHIDDEN)
    return;

  string displayTag = linkStartChars + displaySpace + unhiddenDisplayProperty;
  auto displayTagBegin = linkString.find(displayTag);
  linkString.replace(displayTagBegin, displayTag.length(),
                     (type == LINK_DISPLAY_TYPE::DIRECT)
                         ? linkStartChars
                         : linkStartChars + displaySpace +
                               hiddenDisplayProperty);
}

static const string linkToSameFile =
    R"(<a unhidden title="QQ" href="#YY"><sub hidden>WW</sub>ZZ</a>)";

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
string fixLinkFromSameFileTemplate(LINK_DISPLAY_TYPE type, const string &key,
                                   const string &citation,
                                   const string &annotation,
                                   const string &referPara) {
  string link = linkToSameFile;
  replaceDisplayLink(link, type);
  if (referPara.empty()) {
    replacePart(link, R"(#YY)", emptyString);
  } else
    replacePart(link, "YY", referPara);
  // in case use top/bottom as reference name
  if (key.empty()) {
    replacePart(link, R"(<sub hidden>WW</sub>)", emptyString);
    replacePart(link, R"( title="QQ")", emptyString);
  } else {
    replacePart(link, "QQ", key);
  }
  replacePart(link, "WW", citation);
  replacePart(link, "ZZ", annotation);
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << link << endl;
  return link;
}

static const string linkToMainFile =
    R"(<a unhidden title="QQ" href="PPa0XX.htm#YY"><sub hidden>WW</sub>ZZ</a>)";

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
                               LINK_DISPLAY_TYPE type, const string &key,
                               const string &citation, const string &annotation,
                               const string &referPara) {
  string link = linkToMainFile;
  replaceDisplayLink(link, type);
  replacePart(link, "PP", path);
  replacePart(link, "XX", filename);
  if (referPara.empty()) {
    replacePart(link, R"(#YY)", emptyString);
  } else
    replacePart(link, "YY", referPara);
  // in case use top/bottom as reference name
  if (key.empty()) {
    replacePart(link, R"(<sub hidden>WW</sub>)", emptyString);
    replacePart(link, R"( title="QQ")", emptyString);
  } else {
    replacePart(link, "QQ", key);
  }
  replacePart(link, "WW", citation);
  replacePart(link, "ZZ", annotation);
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << link << endl;
  return link;
}

// now only support reverse link from main back to main
static const string reverseLinkToMainFile =
    R"(<a unhidden href="a0XX.htm#YY"><sub hidden>WW</sub>被引用</a>)";

string fixLinkFromReverseLinkTemplate(const string &filename,
                                      LINK_DISPLAY_TYPE type,
                                      const string &referPara,
                                      const string &citation,
                                      const string &annotation) {
  string link = reverseLinkToMainFile;
  replaceDisplayLink(link, type);
  replacePart(link, "XX", filename);
  if (referPara.empty()) {
    replacePart(link, R"(#YY)", emptyString);
  } else
    replacePart(link, "YY", referPara);
  replacePart(link, "WW", citation);
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << link << endl;
  return link;
}

static const string linkToOriginalFile =
    R"(<a unhidden title="QQ" href="PPc0XX.htm#YY"><sub hidden>WW</sub>ZZ</a>)";

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
                                   const string &referPara,
                                   const string &annotation) {
  auto link = linkToOriginalFile;
  replacePart(link, "PP", path);
  replacePart(link, "XX", filename);
  if (referPara.empty()) {
    replacePart(link, R"(#YY)", emptyString);
  } else
    replacePart(link, "YY", referPara);
  // in case use top/bottom as reference name
  if (key.empty()) {
    replacePart(link, R"(<sub hidden>WW</sub>)", emptyString);
    replacePart(link, R"( title="QQ")", emptyString);
  } else {
    replacePart(link, "QQ", key);
  }
  replacePart(link, "WW", citation);
  replacePart(link, "ZZ", annotation);
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << link << endl;
  return link;
}

static const string linkToJPMFile =
    R"(<a unhidden title="QQ" href="PPdXXX.htm#YY"><sub hidden>WW</sub>ZZ</a>)";

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
                              const string &referPara,
                              const string &annotation) {
  auto link = linkToJPMFile;
  replacePart(link, "PP", path);
  replacePart(link, "XXX", filename);
  if (referPara.empty()) {
    replacePart(link, R"(#YY)", emptyString);
  } else
    replacePart(link, "YY", referPara);
  // in case use top/bottom as reference name
  if (key.empty()) {
    replacePart(link, R"(<sub hidden>WW</sub>)", emptyString);
    replacePart(link, R"( title="QQ")", emptyString);
  } else {
    replacePart(link, "QQ", key);
  }
  replacePart(link, "WW", citation);
  replacePart(link, "ZZ", annotation);
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << link << endl;
  return link;
}

static const string linkToAttachmentFile =
    R"(<a unhidden href="PPb0XX_YY.htm">ZZ</a>)";

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
                                     const string &annotation) {
  auto link = linkToAttachmentFile;
  replacePart(link, "PP", path);
  replacePart(link, "XX", filename);
  replacePart(link, "YY", attachNo);
  replacePart(link, "ZZ", annotation);
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << link << endl;
  return link;
}

static const string linkToImageFile =
    R"(<a unhidden title="IMAGE" href="XX#YY">（图示：ZZ）</a>)";

/**
 * Note: do not fix path now
 */
string fixLinkFromImageTemplate(const string &fullReferFilenameWithPathExt,
                                const string &picFilename,
                                const string &fullAnnotation,
                                const string &displayProperty) {
  auto link = linkToImageFile;
  if (displayProperty != unhiddenDisplayProperty) {
    if (displayProperty == hiddenDisplayProperty)
      replacePart(link, unhiddenDisplayProperty, hiddenDisplayProperty);
    else
      // direct
      replacePart(link, unhiddenDisplayProperty + displaySpace, emptyString);
  }
  replacePart(link, "XX", fullReferFilenameWithPathExt);
  replacePart(link, "YY", picFilename);
  replacePart(link, "（图示：ZZ）", fullAnnotation);
  return link;
}

/**
 * generate the string of a link from its info like type, key, etc.
 * NOTE: should be consistent with template
 * @return the string of the link
 */

/**
 * get display type HIDDEN, UNHIDDEN, etc. from the link begin tag attribute.
 * and assign m_displayType member field correspondingly.
 * NOTE: key should not be these keywords
 * @param linkString the link to check
 */
void Link::readDisplayType(const string &linkString) {
  string containedPart = getIncludedStringBetweenTags(
      linkString, linkStartChars, referFileMiddleChar);
  if (debug >= LOG_INFO)
    FUNCTION_OUTPUT << containedPart << endl;
  if (containedPart.find(unhiddenDisplayProperty) != string::npos) {
    m_displayType = LINK_DISPLAY_TYPE::UNHIDDEN;
  } else if (containedPart.find(hiddenDisplayProperty) != string::npos) {
    m_displayType = LINK_DISPLAY_TYPE::HIDDEN;
  } else {
    m_displayType = LINK_DISPLAY_TYPE::DIRECT;
  }
  if (debug >= LOG_INFO) {
    if (m_displayType == LINK_DISPLAY_TYPE::DIRECT)
      METHOD_OUTPUT << "display Type: "
                    << "direct." << endl;
    else
      METHOD_OUTPUT << "display Type: " << displayPropertyAsString() << endl;
  }
}

/**
 * a link type is got from the filename it's refer to
 * if no filename appears, it is a link to the same page
 * NOTE: never called for an image type link
 * @param refereFileName the refer file name part of the link
 * @return link type
 */
LINK_TYPE getLinKTypeFromReferFileName(const string &refereFileName) {
  LINK_TYPE type = LINK_TYPE::SAMEPAGE;

  if (refereFileName.find(contentTableFilename) != string::npos or
      refereFileName.find(MAIN_TYPE_HTML_TARGET) != string::npos) {
    type = LINK_TYPE::MAIN;
  } else if (refereFileName.find(ATTACHMENT_TYPE_HTML_TARGET) != string::npos) {
    type = LINK_TYPE::ATTACHMENT;
  } else if (refereFileName.find(ORIGINAL_TYPE_HTML_TARGET) != string::npos) {
    type = LINK_TYPE::ORIGINAL;
  } else if (refereFileName.find(JPM_TYPE_HTML_TARGET) != string::npos) {
    type = LINK_TYPE::JPM;
  }
  return type;
}
/**
 * change target type thru specifying a type prefix
 * @param prefix type prefix
 */
void Link::setTypeThruFileNamePrefix(const string &prefix) {
  if (prefix == MAIN_TYPE_HTML_TARGET)
    m_type = LINK_TYPE::MAIN;
}

/**
 * judge a link is referring to main text, attachment or original text.
 * and assign type member field correspondingly.
 * if .htm file extension is found, check prefix
 * else check title to "IMAGE"
 * else ignore file extension and viewed as link within samepage
 * @param linkString the link to check
 */
void Link::readType(const string &linkString) {
  // special link to image, use title to check
  if (linkString.find(titleStartChars + imageTypeChars + titleEndChars) !=
      string::npos) {
    m_type = LINK_TYPE::IMAGE;
    return;
  }

  m_type = LINK_TYPE::SAMEPAGE;
  if (linkString.find(TARGET_FILE_EXT) == string::npos) {
    if (debug >= LOG_EXCEPTION)
      METHOD_OUTPUT << "no .htm file extension found for " << linkString
                    << endl;
    return;
  }

  string refereFileName = getIncludedStringBetweenTags(
      linkString, referFileMiddleChar, TARGET_FILE_EXT);
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "type seen from prefix: " << refereFileName << endl;
  m_type = getLinKTypeFromReferFileName(refereFileName);
}

/**
 * get the referred para like P4L2 in the target file
 * and assign referPara member field correspondingly.
 * @param linkString the link to check
 */
void Link::readReferPara(const string &linkString) {
  if (m_type == LINK_TYPE::IMAGE) {
    m_referPara = getIncludedStringBetweenTags(linkString, referParaMiddleChar,
                                               referParaEndChar);
    if (debug >= LOG_INFO) {
      METHOD_OUTPUT << "m_referPara: " << m_referPara << endl;
    }
    return;
  }
  string htmStart = TARGET_FILE_EXT + referParaMiddleChar;
  if (m_type == LINK_TYPE::SAMEPAGE)
    htmStart = referParaMiddleChar;
  // no file to refer
  if (linkString.find(htmStart) == string::npos) {
    if (debug >= LOG_EXCEPTION and m_type != LINK_TYPE::ATTACHMENT)
      METHOD_OUTPUT << "no # found to read referPara from link: " << linkString
                    << endl;
    return;
  }
  m_referPara =
      getIncludedStringBetweenTags(linkString, htmStart, titleEndChars);
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "referPara: " << m_referPara << endl;
}

/**
 * annotation is the last part of a link normally, represents the meaning of the
 * link. get this value and assign annotation member field correspondingly.
 * @param linkString the link for analysis
 * @return true if there is target specified
 */
bool Link::readAnnotation(const string &linkString) {
  string htmStart = citationEndChars;
  if (linkString.find(htmStart) == string::npos) {
    // deprecated key end tag
    htmStart = keyEndChars;
    if (linkString.find(htmStart) == string::npos) {
      htmStart = referParaEndChar;
    }
  }
  m_annotation =
      getIncludedStringBetweenTags(linkString, htmStart, linkEndChars);
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "annotation: " << m_annotation << endl;
  return true;
}
