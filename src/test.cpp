#include "gb2312ToUtf8.hpp"
#include "linkFix.hpp"
#include "numbering.hpp"
#include "personal.hpp"
#include "search.hpp"
#include "story.hpp"

int debug = LOG_INFO;
// int debug = LOG_EXCEPTION;

void testSearchTextIsOnlyPartOfOtherKeys() {
  string line =
      R"(秋水眼又对秋水鸳鸯剑，埋下<a unhidden href="#P94"><i hidden>春色</i>“倒底是不标致的好”</a>的悲剧结局)";
  line =
      R"(满脸春色，比白日更增了颜色（<i unhidden>美丽</i>）。贾琏搂她笑道：“人人都说我们那<a href="a044.htm#P94"><i hidden>夜叉星</i>夜叉婆</a>（<i unhidden>凤姐</i>）齐整（<i unhidden>标致</i>），如今我看来，（<i unhidden>俯就你</i>）给你拾鞋也不要。”尤二姐道：“我虽标致，却无<a href="a066.htm#P94"><i hidden>品行</i>品行</a>。看来倒底是不标致的好。)";
  string key = R"(春色)";
  cout << isOnlyPartOfOtherKeys(line, key);
}

void testLineHeader(string lnStr) {
  cout << "original: " << endl << lnStr << endl;
  LineNumber ln(lnStr);
  cout << ln.getParaNumber() << " " << ln.getlineNumber() << " "
       << ln.asString() << endl
       << "is paragraph header? " << ln.isParagraphHeader() << endl;
  if (not ln.isParagraphHeader()) {
    if (not ln.isPureTextOnly())
      cout << "generate line prefix: " << endl
           << ln.generateLinePrefix() << endl;
    else
      cout << "pure text" << endl;
  }
}

void testLineHeaderFromContainedLine(string containedLine) {
  cout << "original: " << endl << containedLine << endl;
  LineNumber ln;
  ln.loadFromContainedLine(containedLine);
  cout << ln.getParaNumber() << " " << ln.getlineNumber() << " "
       << ln.asString() << endl
       << "is paragraph header? " << ln.isParagraphHeader() << endl;
  if (not ln.isParagraphHeader()) {
    if (not ln.isPureTextOnly())
      cout << "generate line prefix: " << endl
           << ln.generateLinePrefix() << endl;
    else
      cout << "pure text" << endl;
  }
}

void testParagraphHeader(string lnStr) {
  cout << "original: " << endl << lnStr << endl;
  LineNumber ln(lnStr);
  cout << ln.getParaNumber() << " " << ln.getlineNumber() << " "
       << ln.asString() << endl
       << "is paragraph header? " << ln.isParagraphHeader() << endl;
  if (not ln.isParagraphHeader()) {
    if (not ln.isPureTextOnly())
      cout << "generate line prefix: " << endl
           << ln.generateLinePrefix() << endl;
    else
      cout << "pure text" << endl;
  }
}

void testParagraphHeaderFromContainedLine(string containedLine) {
  cout << "original: " << endl << containedLine << endl;
  LineNumber ln;
  ln.loadFromContainedLine(containedLine);
  cout << ln.getParaNumber() << " " << ln.getlineNumber() << " "
       << ln.asString() << endl
       << "is paragraph header? " << ln.isParagraphHeader() << endl;
  if (not ln.isParagraphHeader()) {
    if (not ln.isPureTextOnly())
      cout << "generate line prefix: " << endl
           << ln.generateLinePrefix() << endl;
    else
      cout << "pure text" << endl;
  }
}

void testLineNumber() {
  LineNumber::setStartNumber(START_PARA_NUMBER);
  testLineHeader("P1L4");
  SEPERATE("ln1", " finished ");
  testLineHeader("P13L4");
  SEPERATE("ln2", " finished ");
  testLineHeaderFromContainedLine(
      R"(<a unhidden name="P12L8">12.8</a>　　原来 这一个名唤 贾蔷，也系 宁国府 正派玄孙，父母早亡，从小 跟贾珍过活，如今 长了十六岁，比 贾蓉 生得还 风流俊俏。贾蓉、贾蔷兄弟二人最相亲厚，常相共处。<br>)");
  SEPERATE("ln3", " finished ");

  testParagraphHeaderFromContainedLine(fixFirstParaHeaderFromTemplate(
      LineNumber::getStartNumber(), getSeparateLineColor(FILE_TYPE::MAIN)));
  SEPERATE("ln4", " finished ");
  testParagraphHeaderFromContainedLine(fixMiddleParaHeaderFromTemplate(
      LineNumber::getStartNumber(), 1, getSeparateLineColor(FILE_TYPE::MAIN),
      false));
  SEPERATE("ln5", " finished ");
  testParagraphHeaderFromContainedLine(fixLastParaHeaderFromTemplate(
      LineNumber::getStartNumber(), 12, getSeparateLineColor(FILE_TYPE::MAIN)));
  SEPERATE("ln6", " finished ");
  testLineHeaderFromContainedLine(R"(<br>)");
  SEPERATE("ln7", " finished ");
  testLineHeaderFromContainedLine(R"(anything)");
  SEPERATE("ln8", " finished ");
}

/**
 * this function needs numbering first to get a correct target refer line number
 * or copy .txt files under testData to bodyTexts\output
 */
void testLinkFromMain(string fromFile, string linkString,
                      bool needToGenerateOrgLink) {
  cout << "original link: " << endl << linkString << endl;
  LinkFromMain lfm(fromFile, linkString);
  lfm.readReferFileName(linkString); // second step of construction
  lfm.fixFromString(linkString);
  if (needToGenerateOrgLink)
    lfm.generateLinkToOrigin();
  auto fixed = lfm.asString();
  cout << "need Update: " << lfm.needUpdate() << endl;
  cout << "after fixed: " << endl << fixed << endl;
}

void testLinkFromAttachment(string fromFile, string linkString,
                            bool needToGenerateOrgLink) {
  cout << "original link: " << endl << linkString << endl;
  LinkFromAttachment lfm(fromFile, linkString);
  lfm.readReferFileName(linkString); // second step of construction
  lfm.fixFromString(linkString);
  cout << lfm.getAnnotation() << endl;
  if (needToGenerateOrgLink)
    lfm.generateLinkToOrigin();
  auto fixed = lfm.asString();
  cout << "need Update: " << lfm.needUpdate() << endl;
  cout << "after fixed: " << endl << fixed << endl;
}

void testLink(Link &lfm, string linkString, bool needToGenerateOrgLink) {
  cout << "original link: " << endl << linkString << endl;

  //
  lfm.readReferFileName(linkString); // second step of construction
  lfm.fixFromString(linkString);
  if (needToGenerateOrgLink)
    lfm.generateLinkToOrigin();
  auto fixed = lfm.asString();
  cout << "need Update: " << lfm.needUpdate() << endl;
  cout << "after fixed: " << endl << fixed << endl;
}

void testLinkOperation() {

  string linkString =
      R"(<a hidden href="a080.htm#top">原是（<i unhidden>薛姨妈1</i>）老奶奶（<i unhidden>薛姨妈2</i>）使唤的</a>)";
  cout << "original link: " << endl << linkString << endl;
  LinkFromMain lfm("75", linkString);
  lfm.readReferFileName(linkString); // second step of construction
  cout << "change to refer to file 57. " << endl;
  lfm.fixReferFile(57);
  lfm.fixFromString(linkString);
  auto fixed = lfm.asString();
  cout << "need Update: " << lfm.needUpdate() << endl;
  cout << "after fixed: " << endl << fixed << endl;
  SEPERATE("fixReferFile", " finished ");

  testLinkFromMain("07", R"(<a hidden
        href="a080.htm#top">原是（<i unhidden>薛姨妈1</i>）老奶奶（<i unhidden>薛姨妈2</i>）使唤的</a>)",
                   false);
  SEPERATE("#top", " finished ");

  testLinkFromMain(
      "07",
      R"(<a hidden href="attachment\b003_9.htm#P2L3">原是老奶奶（薛姨妈）使唤的</a>)",
      false);
  SEPERATE("WARNING:", " SUCH LINK'S REFERPARA WON'T BE FIXED AUTOMATICALLY. ");
  SEPERATE("attachment with referPara", " finished ");

  testLinkFromMain(
      "80",
      fixLinkFromSameFileTemplate(LINK_DISPLAY_TYPE::HIDDEN, "菱角菱花",
                                  "原是老奶奶（薛姨妈）使唤的", "94"),
      false);
  SEPERATE("fixLinkFromSameFileTemplate", " finished ");

  linkString = fixLinkFromMainTemplate(
      "", "80", LINK_DISPLAY_TYPE::HIDDEN, "菱角菱花",
      "第80章1.1节:", "原是老奶奶（薛姨妈）使唤的", "94");
  LinkFromMain link("07", linkString);
  testLink(link, linkString, false);

  SEPERATE("fixLinkFromMainTemplate", " finished ");

  testLinkFromMain("03",
                   fixLinkFromMainTemplate(
                       "", "80", LINK_DISPLAY_TYPE::HIDDEN, "菱角菱花",
                       "第80章1.1节:", "原是老奶奶（薛姨妈）使唤的", "94"),
                   true);
  SEPERATE("generate original link afterwards", " finished ");

  testLinkFromMain(
      "07", fixLinkFromOriginalTemplate(R"(original\)", "18", "happy", "90101"),
      false);
  SEPERATE("fixLinkFromOriginalTemplate", " finished ");

  testLinkFromMain(
      "07", fixLinkFromAttachmentTemplate(R"(attachment\)", "18", "7", "happy"),
      false);
  SEPERATE("fixLinkFromOriginalTemplate", " finished ");

  SEPERATE("testLinkFromMain", " finished ");

  string linkString1 = fixLinkFromMainTemplate(
      R"(..\)", "80", LINK_DISPLAY_TYPE::HIDDEN, "菱角菱花",
      "第80章1.1节:", "原是老奶奶（薛姨妈）使唤的", "94");
  string linkString2 = fixLinkFromAttachmentTemplate("", "18", "7", "happy");
  cout << "original link: " << endl << linkString2 << endl;
  LinkFromAttachment lfm1("03_9", linkString2);
  lfm1.readReferFileName(linkString2); // second step of construction
  cout << "change to refer to file 55_3. " << endl;
  lfm1.fixReferFile(55, 3);
  lfm1.fixFromString(linkString2);
  auto fixed2 = lfm1.asString();
  cout << "need Update: " << lfm1.needUpdate() << endl;
  cout << "after fixed: " << endl << fixed2 << endl;
  SEPERATE("fixReferFile", " finished ");
  testLinkFromAttachment("1_0",
                         R"(<a unhidden href="..\aindex.htm">回目录</a>)",
                         false);
  SEPERATE("回目录", " finished ");

  testLinkFromAttachment(
      "03_9",
      fixLinkFromSameFileTemplate(LINK_DISPLAY_TYPE::HIDDEN, "西北",
                                  "原是老奶奶（薛姨妈）使唤的", "94"),
      false);
  SEPERATE("fixLinkFromSameFileTemplate", " finished ");

  testLinkFromAttachment(
      "03_9",
      fixLinkFromMainTemplate(
          R"(..\)", "80", LINK_DISPLAY_TYPE::HIDDEN, "菱角菱花",
          "第80章1.1节:", "原是老奶奶（薛姨妈）使唤的", "94"),
      true);
  SEPERATE("fixLinkFromMainTemplate", " finished ");

  testLinkFromAttachment(
      "03_9",
      fixLinkFromOriginalTemplate(R"(..\original\)", "80", "菱角菱花", "94"),
      false);
  SEPERATE("fixLinkFromOriginalTemplate", " finished ");

  testLinkFromAttachment(
      "03_9", fixLinkFromAttachmentTemplate("", "18", "7", "happy"), false);
  SEPERATE("fixLinkFromAttachmentTemplate", " finished ");
  SEPERATE("testLinkFromAttachment", " finished ");
}

void testAttachmentOperations() {
  auto attList = getAttachmentFileListForChapter("28", HTML_SRC_ATTACHMENT);
  sort(attList.begin(), attList.end());
  for (const auto &attNo : attList) {
    cout << attNo << ", ";
  }
  cout << endl;
  attList = getAttachmentFileListForChapter("22", HTML_SRC_ATTACHMENT);
  sort(attList.begin(), attList.end());
  for (const auto &attNo : attList) {
    cout << attNo << ", ";
  }
  cout << endl;
}

void testContainer() {
  ListContainer container("1_gen");
  auto link = fixLinkFromMainTemplate(
      "", "80", LINK_DISPLAY_TYPE::UNHIDDEN, "菱角菱花",
      "第80章1.1节:", "原是老奶奶（薛姨妈）使唤的", "94");

  // to test link to original file
  link = fixLinkFromOriginalTemplate(R"(original\)", "80", "菱角菱花", "94");

  LinkFromMain lfm("", link);
  lfm.readReferFileName(link); // second step of construction
  lfm.fixFromString(link);
  link = lfm.asString();

  container.clearBodyTextFile();
  container.appendParagraphInBodyText(link);
  container.appendParagraphInBodyText("18 links are found.");
  container.assembleBackToHTM("test", "test container");
  cout << "result is in file " << container.getOutputFilePath() << endl;
}

void testConstructSubStory() {
  BodyText bodyText(getBodyTextFilePrefixFromFileType(FILE_TYPE::MAIN));

  BodyText::ParaStruct res = bodyText.getNumberOfPara("06");
  cout << GetTupleElement(res, 0) << " " << GetTupleElement(res, 1) << " "
       << GetTupleElement(res, 2) << endl;
}

void testFindFirstInFiles() {
  searchKeywordInNoAttachmentFiles(R"(聚赌嫖娼)", "main", "xxx3");
  searchKeywordInNoAttachmentFiles(R"(头一社)", "original", "xxx4");
}

void testRemovePersonalViewpoints() {
  int minTarget = 54, maxTarget = 54;
  removePersonalViewpoints(minTarget, maxTarget, FILE_TYPE::MAIN);
}

void testConvertToUtf8() {
  convertAllFromGb2312ToUtf8();
  gb2312FixJPM();
  gb2312FixOriginal(1, 80);
}

void testFunctions() {
  SEPERATE("HLM test", " started ");
  //  testSearchTextIsOnlyPartOfOtherKeys();
  //  testLineNumber();
  //  testLinkOperation();
  //  testAttachmentOperations();
  //  testContainer();
  //  testConstructSubStory();
  //  testFindFirstInFiles();
  //  testRemovePersonalViewpoints();
  //  testConvertToUtf8();
}
