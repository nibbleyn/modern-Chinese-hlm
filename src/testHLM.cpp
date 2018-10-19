#include "hlm.hpp"
#include "hlmGb2312.hpp"

int debug = LOG_EXCEPTION;

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

void testLinkOperation() {

  string linkString =
      R"(<a hidden href="a080.htm#top">原是老奶奶（薛姨妈）使唤的</a>)";
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
        href="a080.htm#top">原是老奶奶（薛姨妈）使唤的</a>)",
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

  testLinkFromMain("07",
                   fixLinkFromMainTemplate(
                       "", "80", LINK_DISPLAY_TYPE::HIDDEN, "菱角菱花",
                       "第80章1.1节:", "原是老奶奶（薛姨妈）使唤的", "94"),
                   false);
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
  clearLinksInContainerBodyText(1);
  addFirstParagraphInContainerBodyText(LineNumber::getStartNumber(), 1);
  appendNumberLineInContainerBodyText(
      R"(<a unhidden name="91202">2.2</a>　　不多日，黛玉、贾雨村一行便抵达京城。贾雨村整了整衣冠，带了小童，拿着自己的名帖，到荣国府门前便投了上去。贾政看了妹夫的来信，立刻请入相见。这贾政本就最喜欢读书之人，礼贤下士，济弱扶危，见贾雨村相貌魁伟，言语不俗，又有妹夫的面子，因此不敢怠慢，特别照顾优待贾雨村，竭心尽力地在幕后协助，写了一本折子，奏请皇上应准一个起复旧吏的位置。不到两个月，南京应天府刚好缺人，便让贾雨村去做了应天府府尹。贾雨村拜辞了贾政，择日上任去了，不在话下。（<i unhidden>贾政不同于冷子兴。虽附庸风雅，其人自己则风流潇洒。</i>）<br>)",
      1);
  appendTextInContainerBodyText("聚赌嫖娼", 1);
  appendLinkInContainerBodyText(
      R"(<a href="a043.htm"><i hidden>KeyNotFound</i><i
	        hidden>头2社</i><bunhidden>第43章7.5节:</b>头一社</a>)",
      1);
  addLastParagraphInContainerBodyText(LineNumber::getStartNumber(),
                                      LineNumber::getStartNumber(), 1);
  string inputHtmlFile = HTML_CONTAINER + "1" + HTML_SUFFIX;
  string inputBodyTextFile = BODY_TEXT_CONTAINER + "1" + BODY_TEXT_SUFFIX;
  string outputFile = HTML_CONTAINER + "1_gen" + HTML_SUFFIX;
  assembleBackToHTM(inputHtmlFile, inputBodyTextFile, outputFile);
}

void testConstructSubStory() {
  ParaStruct res = getNumberOfPara(BODY_TEXT_OUTPUT + "Main06.txt");
  cout << GetTupleElement(res, 0) << " " << GetTupleElement(res, 1) << " "
       << GetTupleElement(res, 2) << endl;
}

void testFindFirstInFiles() {
  //  findFirstInFiles("聚赌嫖娼", MAIN, 1, 80, "xxx2.htm");
  //  findFirstInFiles("头一社", MAIN, 43, 43, "xxx2.htm");
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
  //    testLineNumber();
  //  testLinkOperation();
  //  testAttachmentOperations();
  //  testContainer();
  //  testConstructSubStory();
  //  testFindFirstInFiles();
  //  testRemovePersonalViewpoints();
  //  testConvertToUtf8();
}
