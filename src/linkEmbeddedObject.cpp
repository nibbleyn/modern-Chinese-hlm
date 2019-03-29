#include "linkEmbeddedObject.hpp"

string scanForSubLinks(const string &original, const string &fromFile) {
  string result;
  using SubStringOffsetTable =
      std::map<size_t, size_t>; // start offset -> end offset
  SubStringOffsetTable subStrings;
  string startTag = linkStartChars;
  string endTag = linkEndChars;
  auto offset = original.find(startTag);
  do {
    if (offset == string::npos)
      break;
    subStrings[offset] = original.find(endTag, offset);
    offset = original.find(startTag, offset + 1);
  } while (true);
  auto endOfSubStringOffset = 0;
  for (const auto &link : subStrings) {
    result += original.substr(endOfSubStringOffset,
                              link.first - endOfSubStringOffset);
    auto current = std::make_unique<LinkFromMain>(fromFile);

    current->loadFirstFromContainedLine(original, endOfSubStringOffset);
    result += current->getDisplayString();
    endOfSubStringOffset = link.second + endTag.length();
  }
  result += original.substr(endOfSubStringOffset);
  return result;
}

static const string personalCommentTemplate =
    R"(<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">XX</u>)";

string fixPersonalCommentFromTemplate(const string &comment) {
  string result = personalCommentTemplate;
  replacePart(result, "XX", comment);
  return result;
}

string PersonalComment::getWholeString() {
  return fixPersonalCommentFromTemplate(m_bodyText);
}
string PersonalComment::getDisplayString() { return m_displayText; }

size_t PersonalComment::displaySize() { return getDisplayString().length(); }

size_t PersonalComment::loadFirstFromContainedLine(const string &containedLine,
                                                   size_t after) {
  m_fullString = getWholeStringBetweenTags(
      containedLine, personalCommentStartChars, personalCommentEndChars, after);
  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << "m_fullString: " << endl;
    METHOD_OUTPUT << m_fullString << endl;
  }
  m_bodyText = getIncludedStringBetweenTags(
      m_fullString, endOfPersonalCommentBeginTag, personalCommentEndChars);
  m_displayText = scanForSubLinks(m_bodyText, m_fromFile);
  return containedLine.find(personalCommentStartChars, after);
}

static const string poemTranslationTemplate =
    R"(<samp unhidden font style="font-size: 13.5pt; font-family:楷体; color:#ff00ff">XX</samp>)";

string fixPoemTranslationFromTemplate(const string &translation) {
  string result = poemTranslationTemplate;
  replacePart(result, "XX", translation);
  return result;
}

string PoemTranslation::getWholeString() {
  return fixPoemTranslationFromTemplate(m_bodyText);
}
string PoemTranslation::getDisplayString() { return m_displayText; }

size_t PoemTranslation::displaySize() { return getDisplayString().length(); }

size_t PoemTranslation::loadFirstFromContainedLine(const string &containedLine,
                                                   size_t after) {
  m_fullString = getWholeStringBetweenTags(
      containedLine, poemTranslationBeginChars, poemTranslationEndChars, after);
  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << "m_fullString: " << endl;
    METHOD_OUTPUT << m_fullString << endl;
  }
  m_bodyText = getIncludedStringBetweenTags(
      m_fullString, endOfPoemTranslationBeginTag, poemTranslationEndChars);
  m_displayText = scanForSubLinks(m_bodyText, m_fromFile);
  return containedLine.find(poemTranslationBeginChars, after);
}

static const string commentTemplate =
    R"(<cite unhidden>XX</cite>)";

string fixCommentFromTemplate(const string &comment) {
  string result = commentTemplate;
  replacePart(result, "XX", comment);
  return result;
}

string Comment::getWholeString() { return fixCommentFromTemplate(m_bodyText); }
string Comment::getDisplayString() { return m_displayText; }

size_t Comment::displaySize() { return getDisplayString().length(); }

size_t Comment::loadFirstFromContainedLine(const string &containedLine,
                                           size_t after) {
  m_fullString = getWholeStringBetweenTags(containedLine, commentBeginChars,
                                           commentEndChars, after);
  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << "m_fullString: " << endl;
    METHOD_OUTPUT << m_fullString << endl;
  }
  m_bodyText = getIncludedStringBetweenTags(m_fullString, endOfCommentBeginTag,
                                            commentEndChars);
  m_displayText = scanForSubLinks(m_bodyText, m_fromFile);
  return containedLine.find(commentBeginChars, after);
}

/**
 * this function needs numbering first to get a correct target refer line number
 * or copy .txt files under testData to bodyTexts\output
 */
void testLinkFromMain(string fromFile, string linkString,
                      bool needToGenerateOrgLink) {
  FUNCTION_OUTPUT << "original link: " << endl;
  FUNCTION_OUTPUT << linkString << endl;
  LinkFromMain lfm(fromFile, linkString);
  lfm.readReferFileName(linkString); // second step of construction
  lfm.fixFromString(linkString);
  if (needToGenerateOrgLink)
    lfm.generateLinkToOrigin();
  auto fixed = lfm.asString();
  FUNCTION_OUTPUT << "need Update: " << lfm.needUpdate() << endl;
  FUNCTION_OUTPUT << "after fixed: " << endl;
  FUNCTION_OUTPUT << fixed << endl;
  FUNCTION_OUTPUT << "display as:" << lfm.getDisplayString() << "||" << endl;
}

void testLinkFromAttachment(string fromFile, string linkString,
                            bool needToGenerateOrgLink) {
  FUNCTION_OUTPUT << "original link: " << endl;
  FUNCTION_OUTPUT << linkString << endl;
  LinkFromAttachment lfm(fromFile, linkString);
  lfm.readReferFileName(linkString); // second step of construction
  lfm.fixFromString(linkString);
  FUNCTION_OUTPUT << lfm.getAnnotation() << endl;
  if (needToGenerateOrgLink)
    lfm.generateLinkToOrigin();
  auto fixed = lfm.asString();
  FUNCTION_OUTPUT << "need Update: " << lfm.needUpdate() << endl;
  FUNCTION_OUTPUT << "after fixed: " << endl;
  FUNCTION_OUTPUT << fixed << endl;
  FUNCTION_OUTPUT << "display as:" << lfm.getDisplayString() << "||" << endl;
}

void testLink(Link &lfm, string linkString, bool needToGenerateOrgLink) {
  FUNCTION_OUTPUT << "original link: " << endl;
  FUNCTION_OUTPUT << linkString << endl;

  //
  lfm.readReferFileName(linkString); // second step of construction
  lfm.fixFromString(linkString);
  if (needToGenerateOrgLink)
    lfm.generateLinkToOrigin();
  auto fixed = lfm.asString();
  FUNCTION_OUTPUT << "need Update: " << lfm.needUpdate() << endl;
  FUNCTION_OUTPUT << "after fixed: " << endl;
  FUNCTION_OUTPUT << fixed << endl;
  FUNCTION_OUTPUT << "display as:" << lfm.getDisplayString() << "||" << endl;
}

void testLinkOperation() {
  SEPERATE("testLinkOperation", " starts ");

  auto str = getWholeStringBetweenTags("<111>22>", "<111>", ">");
  FUNCTION_OUTPUT << str << endl;

  //clang-format off
  testLinkFromAttachment(
      "07",
      R"(<a title="IMAGE" href="#nwbt.jpg">（图示：女娲补天）</a>)", false);
  SEPERATE("direct image link", " finished ");

  testLinkFromAttachment(
      "07",
      R"(<a hidden title="IMAGE" href="#nwbt.jpg">（图示：女娲补天）</a>)",
      false);
  SEPERATE("hidden image link", " finished ");

  string linkString =
      R"(<a unhidden href="a080.htm#top">原是)" + commentStart +
      unhiddenDisplayProperty + endOfBeginTag +
      R"(薛姨妈1)" + commentEnd +
      R"(老奶奶)" + commentStart + unhiddenDisplayProperty + endOfBeginTag +
      R"(薛姨妈1)" + commentEnd +
      R"(使唤的</a>)";
  FUNCTION_OUTPUT << "original link: " << endl;
  FUNCTION_OUTPUT << linkString << endl;
  LinkFromMain lfm("75", linkString);
  lfm.readReferFileName(linkString); // second step of construction
  FUNCTION_OUTPUT << "change to refer to file 57. " << endl;
  lfm.fixReferFile(57);
  lfm.fixFromString(linkString);
  auto fixed = lfm.asString();
  FUNCTION_OUTPUT << "need Update: " << lfm.needUpdate() << endl;
  FUNCTION_OUTPUT << "after fixed: " << endl;
  FUNCTION_OUTPUT << fixed << endl;
  FUNCTION_OUTPUT << "display as:" << lfm.getDisplayString() << "||" << endl;
  SEPERATE("fixReferFile", " finished ");

  linkString = fixLinkFromJPMTemplate(jpmDirForLinkFromMain, "017", R"(床帐)",
                                      "", "P1L1", R"(雪梅相妒，无复桂月争辉)");
  FUNCTION_OUTPUT << linkString << endl;
  LinkFromMain link1("05", linkString);
  testLink(link1, linkString, false);

  SEPERATE("JPM link", " finished ");

  linkString =
      R"(<a unhidden href="a005.htm#P94" title="海棠">海棠春睡</a>)";
  FUNCTION_OUTPUT << linkString << endl;
  LinkFromMain link2("70", linkString);
  testLink(link2, linkString, false);

  SEPERATE("title fix", " finished ");

  linkString =
      R"(<a hidden href="a080.htm#top">原是)" + commentStart +
      unhiddenDisplayProperty + endOfBeginTag +
      R"(薛姨妈1)" + commentEnd +
      R"(老奶奶)" + commentStart + unhiddenDisplayProperty + endOfBeginTag +
      R"(薛姨妈1)" + commentEnd +
      R"(使唤的</a>)";
  testLinkFromMain("07", linkString, false);

  SEPERATE("#top", " finished ");

  testLinkFromMain(
      "07",
      R"(<a hidden href="attachment\b003_9.htm#P2L3">原是老奶奶（薛姨妈）使唤的</a>)",
      false);
  SEPERATE("WARNING:", " SUCH LINK'S REFERPARA WON'T BE FIXED AUTOMATICALLY.");

  SEPERATE("attachment with referPara", " finished ");

  testLinkFromMain("80",
                   fixLinkFromSameFileTemplate(
                       LINK_DISPLAY_TYPE::UNHIDDEN, "菱角菱花",
                       "第80章1.1节:", "原是老奶奶（薛姨妈）使唤的", "94"),
                   false);
  SEPERATE("fixLinkFromSameFileTemplate", " finished ");

  linkString = fixLinkFromMainTemplate(
      "", "80", LINK_DISPLAY_TYPE::UNHIDDEN, "菱角菱花",
      "第80章1.1节:", "原是老奶奶（薛姨妈）使唤的", "94");
  LinkFromMain link("07", linkString);
  testLink(link, linkString, false);

  SEPERATE("fixLinkFromMainTemplate", " finished ");

  testLinkFromMain("03",
                   fixLinkFromMainTemplate(
                       "", "80", LINK_DISPLAY_TYPE::UNHIDDEN, "菱角菱花",
                       "第80章1.1节:", "原是老奶奶（薛姨妈）使唤的", "94"),
                   true);
  SEPERATE("generate original link afterwards", " finished ");

  testLinkFromMain("07",
                   fixLinkFromOriginalTemplate(originalDirForLinkFromMain, "18",
                                               "happy",
                                               "第80章1.1节:", "90101"),
                   false);
  SEPERATE("fixLinkFromOriginalTemplate", " finished ");

  testLinkFromMain("07",
                   fixLinkFromAttachmentTemplate(attachmentDirForLinkFromMain,
                                                 "18", "7", "happy"),
                   false);
  SEPERATE("fixLinkFromOriginalTemplate", " finished ");

  SEPERATE("testLinkFromMain", " finished ");

  string linkString2 = fixLinkFromAttachmentTemplate("", "18", "7", "happy");
  FUNCTION_OUTPUT << "original link: " << endl;
  FUNCTION_OUTPUT << linkString2 << endl;
  LinkFromAttachment lfm1("03_9", linkString2);
  lfm1.readReferFileName(linkString2); // second step of construction
  FUNCTION_OUTPUT << "change to refer to file 55_3. " << endl;
  lfm1.fixReferFile(55, 3);
  lfm1.fixFromString(linkString2);
  auto fixed2 = lfm1.asString();
  FUNCTION_OUTPUT << "need Update: " << lfm1.needUpdate() << endl;
  FUNCTION_OUTPUT << "after fixed: " << endl;
  FUNCTION_OUTPUT << fixed2 << endl;
  FUNCTION_OUTPUT << "display as:" << lfm1.getDisplayString() << "||" << endl;
  SEPERATE("fixReferFile", " finished ");

  testLinkFromAttachment("1_0",
                         R"(<a unhidden href="..\aindex.htm">回目录</a>)",
                         false);
  SEPERATE("回目录", " finished ");

  testLinkFromAttachment(
      "03_9",
      fixLinkFromSameFileTemplate(LINK_DISPLAY_TYPE::UNHIDDEN, "西北",
                                  "第80章1.1节:", "原是老奶奶（薛姨妈）使唤的",
                                  "94"),
      false);
  SEPERATE("fixLinkFromSameFileTemplate", " finished ");

  testLinkFromAttachment(
      "03_9",
      fixLinkFromMainTemplate(
          R"(..\)", "80", LINK_DISPLAY_TYPE::UNHIDDEN, "菱角菱花",
          "第80章1.1节:", "原是老奶奶（薛姨妈）使唤的", "94"),
      true);
  SEPERATE("fixLinkFromMainTemplate", " finished ");

  testLinkFromAttachment("03_9",
                         fixLinkFromOriginalTemplate(R"(..\original\)", "80",
                                                     "菱角菱花",
                                                     "第80章1.1节:", "94"),
                         false);
  SEPERATE("fixLinkFromOriginalTemplate", " finished ");

  testLinkFromAttachment(
      "03_9", fixLinkFromAttachmentTemplate("", "18", "7", "happy"), false);
  SEPERATE("fixLinkFromAttachmentTemplate", " finished ");
  //clang-format on
  SEPERATE("testLinkFromAttachment", " finished ");
}
