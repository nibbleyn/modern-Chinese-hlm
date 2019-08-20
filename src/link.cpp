#include "link.hpp"
extern int debug;

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
  if (linkString.find(downArrow) != string::npos) {
    m_type = LINK_TYPE::REVERSE;
    if (debug >= LOG_INFO)
      METHOD_OUTPUT << "reverse link type seen from downArrow: " << downArrow
                    << endl;
    return;
  }
  // special link to image, use title to check
  if (linkString.find(titleStartChars + imageTypeChars + titleEndChars) !=
      string::npos) {
    m_type = LINK_TYPE::IMAGE;
    if (debug >= LOG_INFO)
      METHOD_OUTPUT << "image type seen from special title: "
                    << titleStartChars + imageTypeChars + titleEndChars << endl;
    return;
  }
  // special link to sub Para, use title to check
  if (linkString.find(titleStartChars + subParaTitle) != string::npos) {
    m_type = LINK_TYPE::SUBPARA;
    if (debug >= LOG_INFO)
      METHOD_OUTPUT << "sub para type seen from special title: "
                    << titleStartChars + subParaTitle << endl;
    return;
  }

  m_type = LINK_TYPE::SAMEPAGE;
  if (linkString.find(TARGET_FILE_EXT) == string::npos) {
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
    htmStart = referParaEndChar;
  }
  m_annotation =
      getIncludedStringBetweenTags(linkString, htmStart, linkEndChars);
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "annotation: " << m_annotation << endl;
  return true;
}

void Link::needToChange() {
  if (not m_needChange) {
    if (debug >= LOG_INFO)
      METHOD_OUTPUT << "need to change" << endl;
    m_needChange = true;
  }
}

string Link::getFormatedFullString() { return m_fullString; }

/**
 * must ensure this is not a lineNumber string, which is a normal link also
 * before calling this method
 */
size_t Link::loadFirstFromContainedLine(const string &containedLine,
                                        size_t after) {
  auto pos = getFullStringAndBodyTextFromContainedLine(containedLine, after);
  // skip lineNumber link
  if (m_fullString.find(LineNumberIdentity) != string::npos)
    pos = getFullStringAndBodyTextFromContainedLine(containedLine, pos + 1);
  if (pos != string::npos) {
    readTypeAndAnnotation(m_fullString);
    m_displayText = m_annotation;
  }
  return pos;
}
