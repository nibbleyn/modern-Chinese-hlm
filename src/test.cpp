#include "test.hpp"
#include <array>

void testSearchTextIsOnlyPartOfOtherKeys() {
  string line1 =
      R"(弄得（<cite unhidden>宝玉</cite>）情色若痴，语言常乱，似染怔忡之疾，慌得袭人等又不敢回贾母，只百般逗他玩笑（<cite unhidden>指望他早日康复</cite>）。（<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">怔忡，为病名，首见于《济生方·惊悸怔忡健忘门》中“惊者，心卒动而不宁也；悸者,心跳动而怕惊也；怔忡者，心中躁动不安，惕惕然后人将捕之也”,是心悸的一种，是指多因久病体虚、心脏受损导致气血、阴阳亏虚，或邪毒、痰饮、瘀血阻滞心脉，日久导致心失濡养，心脉不畅，从而引起的心中剔剔不安，不能自控的一种病证，常和惊悸合并称为心悸</u>）)";
  string key = R"(怔忡)";
  FUNCTION_OUTPUT << line1 << endl;
  FUNCTION_OUTPUT << isFoundAsNonKeys(line1, key) << endl;
  string line2 =
      R"(（<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">宝玉真病了的时候，不过袭人<a unhidden href="#P94" title="怔忡">瞒着不回贾母</a>，晨昏定省还是去的</u>）。宝玉方（<cite unhidden>放下笔，先</cite>）去请安（<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">贾母关切，姊妹们则看在眼里</u>）)";
  FUNCTION_OUTPUT << line2 << endl;
  FUNCTION_OUTPUT << isFoundAsNonKeys(line2, key) << endl;

  key = R"(乖滑)";
  string line3 =
      R"(周瑞家的虽不管事（<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">不过管管<a unhidden title="租子" href="a006.htm#P94"><i hidden>租子</i>出门的事</a></u>），因她素日仗着是王夫人的陪房，原（<cite unhidden>本</cite>）有些体面，（<cite unhidden>又兼</cite>）心性乖滑，专管（<cite unhidden>在</cite>）各处（<cite unhidden>主子面前</cite>）献勤讨好，所以各处房里的主人都喜欢她。（<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">至此方将两番接待刘姥姥并送宫花等诸事热情之因说明。“体面”对应和费婆子的不同，“陪房和讨好”又对应共同点。</u>）)";
  FUNCTION_OUTPUT << line3 << endl;
  FUNCTION_OUTPUT << isFoundOutsidePersonalComments(line3, key) << endl;

  string line4 =
      R"(周瑞家的听了（<cite unhidden>凤姐的命令</cite>），巴不得一声儿，素日因与这几个人（<cite unhidden>本就</cite>）不睦（<cite unhidden>，越发要趁机公报私仇</cite>），（<cite unhidden>一从凤姐院里</cite>）出来了便（<cite unhidden>一面</cite>）命一个小厮到林之孝家传凤姐的话，立刻叫林之孝家的进（<cite unhidden>园</cite>）来见大奶奶（<cite unhidden>尤氏</cite>），一面又传人立刻捆起（<cite unhidden>犯事的</cite>）这两个婆子来，交到马圈里派人看守。（<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">凤姐和尤氏本有默契<a unhidden title="宽洪大量" href="#P94"><i hidden>宽洪大量</i>且放着</a>，若不是周瑞家的心性乖滑加上公报私仇，不至于陷害凤姐到众人议论。然而心性乖滑、公报私仇其实恰恰又是说凤姐本人。“素日不睦”已经渐渐带出她们的特殊身份，乃邢夫人陪房，否则也不会跟周瑞家的不睦。</u>）)";
  FUNCTION_OUTPUT << line4 << endl;
  FUNCTION_OUTPUT << isFoundOutsidePersonalComments(line4, key) << endl;
}

void testLineHeader(string lnStr) {
  FUNCTION_OUTPUT << "original: " << endl;
  FUNCTION_OUTPUT << lnStr << endl;
  LineNumberPlaceholderLink ln(lnStr);
  FUNCTION_OUTPUT << "display Property: " << endl;
  FUNCTION_OUTPUT << ln.displayPropertyAsString() << endl;
  FUNCTION_OUTPUT << ln.getParaLineString() << endl;
  FUNCTION_OUTPUT << "is paragraph header? " << std::boolalpha
                  << ln.isPartOfParagraphHeader() << endl;
  FUNCTION_OUTPUT << "whole string: " << endl;
  FUNCTION_OUTPUT << ln.getWholeString() << endl;
  FUNCTION_OUTPUT << "display as:" << ln.getDisplayString() << "||" << endl;
  if (not ln.isPartOfParagraphHeader()) {
    if (not ln.isPureTextOnly()) {
      FUNCTION_OUTPUT << "generate line prefix: " << endl;
      FUNCTION_OUTPUT << ln.get().generateLinePrefix() << endl;
    } else
      FUNCTION_OUTPUT << "pure text" << endl;
  }
}

void testLineHeaderFromContainedLine(string containedLine) {
  FUNCTION_OUTPUT << "original: " << endl;
  FUNCTION_OUTPUT << containedLine << endl;
  LineNumberPlaceholderLink ln;
  ln.loadFirstFromContainedLine(containedLine);
  FUNCTION_OUTPUT << "display Property: " << endl;
  FUNCTION_OUTPUT << ln.displayPropertyAsString() << endl;
  FUNCTION_OUTPUT << ln.getParaLineString() << endl;
  FUNCTION_OUTPUT << "is paragraph header? " << std::boolalpha
                  << ln.isPartOfParagraphHeader() << endl;
  FUNCTION_OUTPUT << "whole string: " << endl;
  FUNCTION_OUTPUT << ln.getWholeString() << endl;
  FUNCTION_OUTPUT << "display as:" << ln.getDisplayString() << "||" << endl;
  if (not ln.isPartOfParagraphHeader()) {
    if (not ln.isPureTextOnly()) {
      FUNCTION_OUTPUT << "generate line prefix: " << endl;
      FUNCTION_OUTPUT << ln.get().generateLinePrefix() << endl;
    } else
      FUNCTION_OUTPUT << "pure text" << endl;
  }
}

void testParagraphHeader(string lnStr) {
  FUNCTION_OUTPUT << "original: " << endl;
  FUNCTION_OUTPUT << lnStr << endl;
  LineNumberPlaceholderLink ln(lnStr);
  FUNCTION_OUTPUT << "display Property: " << endl;
  FUNCTION_OUTPUT << ln.displayPropertyAsString() << endl;
  FUNCTION_OUTPUT << ln.getParaLineString() << endl;
  FUNCTION_OUTPUT << "is paragraph header? " << std::boolalpha
                  << ln.isPartOfParagraphHeader() << endl;
  FUNCTION_OUTPUT << "whole string: " << endl;
  FUNCTION_OUTPUT << ln.getWholeString() << endl;
  FUNCTION_OUTPUT << "display as:" << ln.getDisplayString() << "||" << endl;
  if (not ln.isPartOfParagraphHeader()) {
    if (not ln.isPureTextOnly()) {
      FUNCTION_OUTPUT << "generate line prefix: " << endl;
      FUNCTION_OUTPUT << ln.get().generateLinePrefix() << endl;
    } else
      FUNCTION_OUTPUT << "pure text" << endl;
  }
}

void testParagraphHeaderFromContainedLine(string containedLine) {
  FUNCTION_OUTPUT << "original: " << endl;
  FUNCTION_OUTPUT << containedLine << endl;
  LineNumberPlaceholderLink ln;
  ln.loadFirstFromContainedLine(containedLine);
  FUNCTION_OUTPUT << "display Property: " << endl;
  FUNCTION_OUTPUT << ln.displayPropertyAsString() << endl;
  FUNCTION_OUTPUT << ln.getParaLineString() << endl;
  FUNCTION_OUTPUT << "is paragraph header? " << std::boolalpha
                  << ln.isPartOfParagraphHeader() << endl;
  FUNCTION_OUTPUT << "whole string: " << endl;
  FUNCTION_OUTPUT << ln.getWholeString() << endl;
  FUNCTION_OUTPUT << "display as:" << ln.getDisplayString() << "||" << endl;
  if (not ln.isPartOfParagraphHeader()) {
    if (not ln.isPureTextOnly())
      FUNCTION_OUTPUT << "generate line prefix: " << endl
                      << ln.get().generateLinePrefix() << endl;
    else
      FUNCTION_OUTPUT << "pure text" << endl;
  }
}

void testLineNumber() {
  LineNumber::setStartNumber(START_PARA_NUMBER);
  testLineHeader("P1L4");
  SEPERATE("ln1", " finished ");
  testLineHeader("P13L4");
  SEPERATE("ln2", " finished ");
  testLineHeaderFromContainedLine(
      R"(<a unhidden id="P12L8">12.8</a>　　原来 这一个名唤 贾蔷，也系 宁国府 正派玄孙，父母早亡，从小 跟贾珍过活，如今 长了十六岁，比 贾蓉 生得还 风流俊俏。贾蓉、贾蔷兄弟二人最相亲厚，常相共处。<br>)");
  SEPERATE("ln3", " finished ");

  CoupledParaHeader paraHeader;
  paraHeader.setStartNumber(LineNumber::getStartNumber());
  paraHeader.markAsFirstParaHeader();
  paraHeader.fixFromTemplate();
  testParagraphHeaderFromContainedLine(paraHeader.getFixedResult());

  CoupledParaHeader paraHeaderLoaded;
  paraHeaderLoaded.loadFrom(paraHeader.getFixedResult());
  paraHeaderLoaded.fixFromTemplate();
  FUNCTION_OUTPUT << paraHeaderLoaded.getFixedResult() << endl;
  printCompareResult(paraHeader.getFixedResult(),
                     paraHeaderLoaded.getFixedResult());
  FUNCTION_OUTPUT << "display as:" << endl;
  FUNCTION_OUTPUT << paraHeaderLoaded.getDisplayString() << endl;

  paraHeader.setCurrentParaNo(7);
  paraHeader.markAsMiddleParaHeader();
  paraHeader.fixFromTemplate();
  testParagraphHeaderFromContainedLine(paraHeader.getFixedResult());

  paraHeaderLoaded.loadFrom(paraHeader.getFixedResult());
  paraHeaderLoaded.fixFromTemplate();
  FUNCTION_OUTPUT << paraHeaderLoaded.getFixedResult() << endl;
  printCompareResult(paraHeader.getFixedResult(),
                     paraHeaderLoaded.getFixedResult());
  FUNCTION_OUTPUT << "display as:" << endl;
  FUNCTION_OUTPUT << paraHeaderLoaded.getDisplayString() << endl;
  SEPERATE("ln5", " finished ");

  paraHeader.setCurrentParaNo(12);
  paraHeader.markAsLastParaHeader();
  paraHeader.fixFromTemplate();
  testParagraphHeaderFromContainedLine(paraHeader.getFixedResult());

  paraHeaderLoaded.loadFrom(paraHeader.getFixedResult());
  paraHeaderLoaded.fixFromTemplate();
  FUNCTION_OUTPUT << paraHeaderLoaded.getFixedResult() << endl;
  printCompareResult(paraHeader.getFixedResult(),
                     paraHeaderLoaded.getFixedResult());
  FUNCTION_OUTPUT << "display as:" << endl;
  FUNCTION_OUTPUT << paraHeaderLoaded.getDisplayString() << endl;

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
  FUNCTION_OUTPUT << "original link: " << endl;
  FUNCTION_OUTPUT << linkString << endl;
  LinkFromMain lfm(fromFile, linkString);
  // second step of construction
  lfm.readReferFileName(linkString);
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
  // second step of construction
  lfm.readReferFileName(linkString);
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

void testLink(CoupledLink &lfm, string linkString, bool needToGenerateOrgLink) {
  FUNCTION_OUTPUT << "original link: " << endl;
  FUNCTION_OUTPUT << linkString << endl;
  // second step of construction
  lfm.readReferFileName(linkString);
  if (not lfm.isReverseLink()) {
    lfm.fixFromString(linkString);
    if (needToGenerateOrgLink)
      lfm.generateLinkToOrigin();
    auto fixed = lfm.asString();
    FUNCTION_OUTPUT << "need Update: " << lfm.needUpdate() << endl;
    FUNCTION_OUTPUT << "after fixed: " << endl;
    FUNCTION_OUTPUT << fixed << endl;
  } else {
    // str would get from fixLinkFromReverseLinkTemplate when generate it
    //	  string str = linkString;
    // this would happen to render the link
    string str = linkString;
    lfm.loadFirstFromContainedLine(str);
  }
  FUNCTION_OUTPUT << "display as:" << lfm.getDisplayString() << "||" << endl;
}

void testLinkOperation() {
  SEPERATE("testLinkOperation", " starts ");

  auto str = getWholeStringBetweenTags("<111>22>", "<111>", ">");
  FUNCTION_OUTPUT << str << endl;

  //clang-format off
  string linkString =
      R"(<a unhidden href="a025.htm#P9L1">被<sub>25回</sub>引用</a>)";
  FUNCTION_OUTPUT << linkString << endl;
  LinkFromMain link0("05", linkString);
  testLink(link0, linkString, false);
  SEPERATE("unhidden invalid reverse link", " finished ");

  linkString =
      R"(<a unhidden href="a025.htm#P9L1">↓<sub>第3回1.2节:</sub>引用</a>)";
  FUNCTION_OUTPUT << linkString << endl;
  LinkFromMain link_1("05", linkString);
  testLink(link_1, linkString, false);
  SEPERATE("unhidden valid reverse link", " finished ");

  testLinkFromAttachment(
      "07",
      R"(<a title="IMAGE" href="#nwbt.jpg">（图示：女娲补天）</a>)", false);
  SEPERATE("direct image link", " finished ");

  testLinkFromAttachment(
      "07",
      R"(<a hidden title="IMAGE" href="a019.htm#tcf.jpg">（图示：探春房）</a>)",
      false);
  SEPERATE("hidden image link", " finished ");

  linkString =
      R"(<a unhidden href="a080.htm#top">原是)" + commentStart + displaySpace +
      unhiddenDisplayProperty + endOfBeginTag +
      R"(薛姨妈1)" + commentEnd +
      R"(老奶奶)" + commentStart + displaySpace + unhiddenDisplayProperty +
      endOfBeginTag +
      R"(薛姨妈1)" + commentEnd +
      R"(使唤的</a>)";
  FUNCTION_OUTPUT << "original link: " << endl;
  FUNCTION_OUTPUT << linkString << endl;
  LinkFromMain lfm("75", linkString);
  // second step of construction
  lfm.readReferFileName(linkString);
  FUNCTION_OUTPUT << "change to refer to file 57. " << endl;
  lfm.fixReferFile(57);
  lfm.fixFromString(linkString);
  auto fixed = lfm.asString();
  FUNCTION_OUTPUT << "need Update: " << lfm.needUpdate() << endl;
  FUNCTION_OUTPUT << "after fixed: " << endl;
  FUNCTION_OUTPUT << fixed << endl;
  FUNCTION_OUTPUT << "display as:" << lfm.getDisplayString() << "||" << endl;
  SEPERATE("fixReferFile", " finished ");

  linkString =
      fixLinkFromJPMTemplate(jpmDirForLinkFromMain, "017", R"(床帐)",
                             emptyString, R"(雪梅相妒，无复桂月争辉)", "P1L1");
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
      R"(<a hidden href="a080.htm#top">原是)" + commentStart + displaySpace +
      unhiddenDisplayProperty + endOfBeginTag +
      R"(薛姨妈1)" + commentEnd +
      R"(老奶奶)" + commentStart + displaySpace + unhiddenDisplayProperty +
      endOfBeginTag +
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
                       DISPLAY_TYPE::UNHIDDEN, "菱角菱花",
                       "第80回1.1节:", "原是老奶奶（薛姨妈）使唤的", "94"),
                   false);
  SEPERATE("fixLinkFromSameFileTemplate", " finished ");

  linkString = fixLinkFromMainTemplate(
      emptyString, "80", DISPLAY_TYPE::UNHIDDEN, "菱角菱花",
      "第80回1.1节:", "原是老奶奶（薛姨妈）使唤的", "94");
  LinkFromMain link("07", linkString);
  testLink(link, linkString, false);

  SEPERATE("fixLinkFromMainTemplate", " finished ");

  testLinkFromMain("03",
                   fixLinkFromMainTemplate(
                       emptyString, "80", DISPLAY_TYPE::UNHIDDEN, "菱角菱花",
                       "第80回1.1节:", "原是老奶奶（薛姨妈）使唤的", "94"),
                   true);
  SEPERATE("generate original link afterwards", " finished ");

  testLinkFromMain("07",
                   fixLinkFromOriginalTemplate(
                       originalDirForLinkFromMain, "18", "happy",
                       "第80回1.1节:", annotationToOriginal, "90101"),
                   false);
  SEPERATE("fixLinkFromOriginalTemplate", " finished ");

  testLinkFromMain("07",
                   fixLinkFromAttachmentTemplate(attachmentDirForLinkFromMain,
                                                 "18", "7", "happy"),
                   false);
  SEPERATE("fixLinkFromOriginalTemplate", " finished ");

  SEPERATE("testLinkFromMain", " finished ");

  string linkString2 =
      fixLinkFromAttachmentTemplate(emptyString, "18", "7", "happy");
  FUNCTION_OUTPUT << "original link: " << endl;
  FUNCTION_OUTPUT << linkString2 << endl;
  LinkFromAttachment lfm1("03_9", linkString2);
  // second step of construction
  lfm1.readReferFileName(linkString2);
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
      fixLinkFromSameFileTemplate(DISPLAY_TYPE::UNHIDDEN, "西北",
                                  "第80回1.1节:", "原是老奶奶（薛姨妈）使唤的",
                                  "94"),
      false);
  SEPERATE("fixLinkFromSameFileTemplate", " finished ");

  testLinkFromAttachment("03_9",
                         fixLinkFromMainTemplate(
                             R"(..\)", "80", DISPLAY_TYPE::UNHIDDEN, "菱角菱花",
                             "第80回1.1节:", "原是老奶奶（薛姨妈）使唤的",
                             "94"),
                         true);
  SEPERATE("fixLinkFromMainTemplate", " finished ");

  testLinkFromAttachment(
      "03_9",
      fixLinkFromOriginalTemplate(R"(..\original\)", "80", "菱角菱花",
                                  "第80回1.1节:", annotationToOriginal, "94"),
      false);
  SEPERATE("fixLinkFromOriginalTemplate", " finished ");

  testLinkFromAttachment(
      "03_9", fixLinkFromAttachmentTemplate(emptyString, "18", "7", "happy"),
      false);
  SEPERATE("fixLinkFromAttachmentTemplate", " finished ");
  //clang-format on
  SEPERATE("testLinkFromAttachment", " finished ");
}

void testListContainer() {
  ListContainer container("1_gen");
  auto link = fixLinkFromMainTemplate(
      emptyString, "80", DISPLAY_TYPE::UNHIDDEN, "菱角菱花",
      "第80回1.1节:", "原是老奶奶（薛姨妈）使唤的", "94");

  // to test link to original file
  link =
      fixLinkFromOriginalTemplate(originalDirForLinkFromMain, "80", "菱角菱花",
                                  "第80回1.1节:", annotationToOriginal, "94");

  LinkFromMain lfm(emptyString, link);
  // second step of construction
  lfm.readReferFileName(link);
  lfm.fixFromString(link);
  link = lfm.asString();

  container.clearExistingBodyText();
  CoupledParaHeader paraHeader;
  paraHeader.setStartNumber(LineNumber::getStartNumber());
  paraHeader.markAsFirstParaHeader();
  paraHeader.fixFromTemplate();
  container.appendParagrapHeader(paraHeader.getFixedResult());
  container.appendParagraphInBodyText(link);
  container.appendParagraphInBodyText("18 links are found.");

  paraHeader.setCurrentParaNo(1);
  paraHeader.markAsLastParaHeader();
  paraHeader.fixFromTemplate();
  paraHeader.fixFromTemplate();
  container.appendParagrapHeader(paraHeader.getFixedResult());
  container.setTitle("test");
  container.setDisplayTitle("test container");
  container.assembleBackToHTM();
  FUNCTION_OUTPUT << "result is in file " << container.getoutputHtmlFilepath()
                  << endl;
}

void testAttachmentOperations() {
  CoupledBodyTextContainer container;
  container.setFileType(FILE_TYPE::ATTACHMENT);
  container.setFileAndAttachmentNumber("28");
  FUNCTION_OUTPUT << "attachments for 28:" << endl;
  for (const auto &attNo : container.getAttachmentFileList(2, 5)) {
    FUNCTION_OUTPUT << attNo << endl;
  }
  FUNCTION_OUTPUT << endl;
  container.setFileAndAttachmentNumber("22");
  FUNCTION_OUTPUT << "attachments for 22:" << endl;
  for (const auto &attNo : container.getAttachmentFileList(5, 2)) {
    FUNCTION_OUTPUT << attNo << endl;
  }
  FUNCTION_OUTPUT << endl;
}

void testTableContainer() {
  TableContainer container("2_gen");
  container.insertFrontParagrapHeader(4, searchUnit);
  container.appendLeftParagraphInBodyText("line1-left");
  container.appendRightParagraphInBodyText("line1-right");
  container.appendLeftParagraphInBodyText("line2-left");
  // if only 3 is added, patch last right part
  container.appendRightParagraphInBodyText(emptyString);
  container.insertBackParagrapHeader(0, 4, searchUnit);
  container.setTitle("content index table");
  container.setDisplayTitle("content");
  container.assembleBackToHTM();
  FUNCTION_OUTPUT << "result is in file: " << container.getoutputHtmlFilepath()
                  << endl;
}

void testContainer(int num) {
  SEPERATE("testContainer", " started ");
  switch (num) {
  case 1:
    testListContainer();
    break;
  case 2:
    testTableContainer();
    break;
  default:
    FUNCTION_OUTPUT << "invalid test." << endl;
  }
}

void testPoem() {
  string poemStr =
      R"(<strong hidden>杜鹃无语正黄昏，荷锄归去掩重门。青灯照壁人初睡，冷雨敲窗被未温。</strong>)";
  string line =
      R"(<a unhidden id="P11L1">11.1</a>&nbsp;&nbsp; <strong unhidden>杜鹃无语正黄昏，荷锄归去掩重门。青灯照壁人初睡，冷雨敲窗被未温。</strong>&nbsp;&nbsp;&nbsp;&nbsp;<samp unhidden font style="font-size: 13.5pt; font-family:楷体; color:#ff00ff">（像杜鹃啼血一样）我泣尽了血泪默默无语，只发现愁惨的黄昏正在降临，只好扛着花锄忍痛归去，一层层带上身后的门。闺中点起青冷的灯光，摇摇曳曳照射着四壁，我才要躺下，拉上尚是冰凉的被裘，却又听见轻寒的春雨敲打着窗棂，更增加了一层寒意。</samp><br>)";
  unique_ptr<Poem> poem1 = make_unique<Poem>();
  poem1->loadFirstFromContainedLine(poemStr);
  FUNCTION_OUTPUT << "display Property: " << endl;
  FUNCTION_OUTPUT << poem1->displayPropertyAsString() << endl;
  FUNCTION_OUTPUT << "length: " << poem1->length()
                  << " display size: " << poem1->displaySize() << endl;
  FUNCTION_OUTPUT << "whole string: " << endl;
  FUNCTION_OUTPUT << poem1->getWholeString() << endl;
  FUNCTION_OUTPUT << "display as:" << endl;
  FUNCTION_OUTPUT << poem1->getDisplayString() << "||" << endl;
  poemStr =
      R"(<strong>杜鹃无语正黄昏，荷锄归去掩重门。青灯照壁人初睡，冷雨敲窗被未温。</strong>)";
  poem1 = make_unique<Poem>();
  poem1->loadFirstFromContainedLine(poemStr);
  FUNCTION_OUTPUT << "display Property: " << endl;
  FUNCTION_OUTPUT << poem1->displayPropertyAsString() << endl;
  FUNCTION_OUTPUT << "length: " << poem1->length()
                  << " display size: " << poem1->displaySize() << endl;
  FUNCTION_OUTPUT << "whole string: " << endl;
  FUNCTION_OUTPUT << poem1->getWholeString() << endl;
  FUNCTION_OUTPUT << "display as:" << endl;
  FUNCTION_OUTPUT << poem1->getDisplayString() << "||" << endl;
  unique_ptr<Poem> poem2 = make_unique<Poem>();
  auto offset = poem2->loadFirstFromContainedLine(line);
  FUNCTION_OUTPUT << "display Property: " << endl;
  FUNCTION_OUTPUT << poem2->displayPropertyAsString() << endl;
  FUNCTION_OUTPUT << "first appearance offset: " << endl;
  FUNCTION_OUTPUT << offset << endl;
  FUNCTION_OUTPUT << "length: " << poem2->length()
                  << " display size: " << poem2->displaySize() << endl;
  FUNCTION_OUTPUT << "whole string: " << endl;
  FUNCTION_OUTPUT << poem2->getWholeString() << endl;
  FUNCTION_OUTPUT << "display as:" << endl;
  FUNCTION_OUTPUT << poem2->getDisplayString() << "||" << endl;
}

void testSpace() {
  string line =
      R"(<a unhidden id="P11L1">11.1</a>&nbsp;&nbsp; <strong unhidden>杜鹃无语正黄昏，荷锄归去掩重门。青灯照壁人初睡，冷雨敲窗被未温。</strong>&nbsp;&nbsp;&nbsp;&nbsp;<samp unhidden font style="font-size: 13.5pt; font-family:楷体; color:#ff00ff">（像杜鹃啼血一样）我泣尽了血泪默默无语，只发现愁惨的黄昏正在降临，只好扛着花锄忍痛归去，一层层带上身后的门。闺中点起青冷的灯光，摇摇曳曳照射着四壁，我才要躺下，拉上尚是冰凉的被裘，却又听见轻寒的春雨敲打着窗棂，更增加了一层寒意。</samp><br>)";
  unique_ptr<Space> sp = make_unique<Space>();
  auto offset = sp->loadFirstFromContainedLine(line);
  FUNCTION_OUTPUT << "display Property: " << endl;
  FUNCTION_OUTPUT << sp->displayPropertyAsString() << endl;
  FUNCTION_OUTPUT << "first appearance offset: " << endl;
  FUNCTION_OUTPUT << offset << endl;
  FUNCTION_OUTPUT << "length: " << sp->length()
                  << " display size: " << sp->displaySize() << endl;
  FUNCTION_OUTPUT << "whole string: " << endl;
  FUNCTION_OUTPUT << sp->getWholeString() << endl;
  FUNCTION_OUTPUT << "display as:" << sp->getDisplayString() << "||" << endl;
}

void testMixedObjects() {
  string line2 =
      R"(我哥哥（<cite unhidden>其实</cite>）已经相准了（<cite unhidden>媳妇</cite>），只等来家（<cite unhidden>就派人往这边</cite>）就下定了，（<cite unhidden>我</cite>）也不必提出人来（<cite unhidden>明说相准了谁</cite>），我方才说你认不得娘，你细想去。”（<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">亲兄妹不能做亲，所以相准了黛玉就变成亲妹妹就不能下定了。玩笑必是<a unhidden title="编派" href="a054.htm#P12L1"><sub hidden>第54回12.1节:</sub>编派现场一个人</a>（<a unhidden title="编派" href="original\c054.htm#P8L1"><sub hidden>第54回8.1节:</sub>原文</a>）。想当初，宝玉就借<a unhidden title="真正的" href="a019.htm#P13L3"><sub hidden>第19回13.3节:</sub>林子洞编派过黛玉</a>（<a unhidden title="真正的" href="original\c019.htm#P14L1"><sub hidden>第19回14.1节:</sub>原文</a>）</u>）说着，便和她母亲（<cite unhidden>薛姨妈</cite>）挤眼儿发笑。黛玉听（<cite unhidden>明白了宝钗的玩笑</cite>）了，便也一头伏在薛姨妈身上，说道：“姨妈不打（<cite unhidden>宝钗</cite>）她我不依。”（<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">这句又接到宝玉生日探春让李纨打黛玉，李纨说黛玉<a unhidden title="挨打" href="a063.htm#P14L3"><sub hidden>第63回14.3节:</sub>人家不得贵婿反挨打</a>（<a unhidden title="挨打" href="original\c063.htm#P6L3"><sub hidden>第63回6.3节:</sub>原文</a>），黛玉的婚姻是镜中花，他们二人的一个不能完成的愿望而已。</u>）)";
  string compareTo2 =
      R"(我哥哥（其实）已经相准了（媳妇），只等来家（就派人往这边）就下定了，（我）也不必提出人来（明说相准了谁），我方才说你认不得娘，你细想去。”（亲兄妹不能做亲，所以相准了黛玉就变成亲妹妹就不能下定了。玩笑必是↑编派现场一个人（↑原文）。想当初，宝玉就借↑林子洞编派过黛玉（↑原文））说着，便和她母亲（薛姨妈）挤眼儿发笑。黛玉听（明白了宝钗的玩笑）了，便也一头伏在薛姨妈身上，说道：“姨妈不打（宝钗）她我不依。”（这句又接到宝玉生日探春让李纨打黛玉，李纨说黛玉↑人家不得贵婿反挨打（↑原文），黛玉的婚姻是镜中花，他们二人的一个不能完成的愿望而已。）)";
  string line3 =
      R"(<a unhidden id="P9L3">9.3</a>&nbsp;&nbsp; <strong unhidden>惯养娇生笑你痴，菱花空对雪澌澌。好防佳节元宵后，便是烟消火灭时。</strong>&nbsp;&nbsp;&nbsp;&nbsp;<samp unhidden font style="font-size: 12pt; font-family: 宋体; color:#ff00ff">你这么痴心 娇生惯养她，实在是可笑。你知道她有多么生不逢时吗？<a unhidden title="菱角菱花" href="a080.htm#P1L4"><sub hidden>第80回1.4节:</sub>菱角菱花皆盛于秋</a>（<a unhidden title="菱角菱花" href="original\c080.htm#P1L3"><sub hidden>第80回1.3节:</sub>原文</a>），可却毫无办法要面对茫茫大雪。要小心元宵佳节一过，一切都会烟消云散</samp>（<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4"><a unhidden title="清净孤独" href="a022.htm#P13L4"><sub hidden>第22回13.4节:</sub>不详灯谜</a>（<a unhidden title="清净孤独" href="original\c022.htm#P12L1"><sub hidden>第22回12.1节:</sub>原文</a>）、<a unhidden title="进贡" href="a054.htm#P13L1"><sub hidden>第54回13.1节:</sub>聋子放炮仗</a>（<a unhidden title="进贡" href="original\c054.htm#P8L2"><sub hidden>第54回8.2节:</sub>原文</a>）、<a unhidden title="按爵" href="a058.htm#P1L2"><sub hidden>第58回1.2节:</sub>老太妃薨毙</a>（<a unhidden title="按爵" href="original\c058.htm#P1L2"><sub hidden>第58回1.2节:</sub>原文</a>）。</u>）)";
  string compareTo3 =
      R"(9.3   惯养娇生笑你痴，菱花空对雪澌澌。好防佳节元宵后，便是烟消火灭时。    你这么痴心 娇生惯养她，实在是可笑。你知道她有多么生不逢时吗？↑菱角菱花皆盛于秋（↑原文），可却毫无办法要面对茫茫大雪。要小心元宵佳节一过，一切都会烟消云散（↑不详灯谜（↑原文）、↑聋子放炮仗（↑原文）、↑老太妃薨毙（↑原文）。）)";
  string line4 =
      R"(<a unhidden id="P1L5">1.5</a>&nbsp;&nbsp; 一天，神石正在独自嗟呀怀悼，突然看见一个僧人和一个道士远远走来。他们生得骨格不凡，丰神迥别（<cite unhidden>神石是神仙，所以看他们才骨骼不凡，否则也跟贾政眼里的<a unhidden title="见那和尚" href="a025.htm#P11L3"><sub hidden>第25回11.3节:</sub>模样</a>（<a unhidden title="见那和尚" href="original\c025.htm#P9L4"><sub hidden>第25回9.4节:</sub>原文</a>）一样了</cite>），说说笑笑走到青埂峰下，坐在神石边高谈快论。先说一些云山雾海神仙玄幻之事，又说到红尘中如何荣华富贵。神石听了，不觉动了凡心（<cite unhidden>对比甄士隐所悟之“真”空，“世人都晓神仙好”，这里就很荒唐了，没想到神仙居然也想做世人，也是因为“空”生万物的缘故，只羡鸳鸯不羡仙，非羡慕酒色财气也。</cite>），也想要到人间去享受享受，但只恨自己粗蠢而不得其道（<cite unhidden>不像神瑛，就住在三生石边上，自然该着投胎，连投胎都不得其道，自然也就不懂人情世故了</cite>）。)";
  string compareTo4 =
      R"(1.5   一天，神石正在独自嗟呀怀悼，突然看见一个僧人和一个道士远远走来。他们生得骨格不凡，丰神迥别（神石是神仙，所以看他们才骨骼不凡，否则也跟贾政眼里的↑模样（↑原文）一样了），说说笑笑走到青埂峰下，坐在神石边高谈快论。先说一些云山雾海神仙玄幻之事，又说到红尘中如何荣华富贵。神石听了，不觉动了凡心（对比甄士隐所悟之“真”空，“世人都晓神仙好”，这里就很荒唐了，没想到神仙居然也想做世人，也是因为“空”生万物的缘故，只羡鸳鸯不羡仙，非羡慕酒色财气也。），也想要到人间去享受享受，但只恨自己粗蠢而不得其道（不像神瑛，就住在三生石边上，自然该着投胎，连投胎都不得其道，自然也就不懂人情世故了）。)";
  string line5 =
      R"(<a unhidden id="P14L4">14.4</a>&nbsp;&nbsp; （<cite unhidden>贾琏</cite>）揭起衾单一看，只见这尤二姐面色如生，比活着还美貌。贾琏又搂着大哭，只叫“奶奶，你死的不明，都是我坑了你！”（<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">还是像当初<a unhidden title="提三说二" href="a065.htm#P1L2"><sub hidden>第65回1.2节:</sub>按大房</a>（<a unhidden title="提三说二" href="original\c065.htm#P1L4"><sub hidden>第65回1.4节:</sub>原文</a>）叫尤二姐“奶奶”</u>）贾蓉忙上来劝（<cite unhidden>贾琏</cite>）：“叔叔解着些儿（<cite unhidden>伤悲</cite>），（<cite unhidden>都是</cite>）我这个姨娘（<cite unhidden>尤二姐她</cite>）自己没福。”说着，又向南指（<cite unhidden>着梨香院和</cite>）<a title="群山" href="a069.htm#P15L3"><sub hidden>第69回15.3节:</sub>大观园的界墙</a>，贾琏会意（<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">凤姐此时仍在偷听</u>），只悄悄跌脚说：“我忽略了（<cite unhidden>，只当凤姐她是好意对尤二姐</cite>），终久（<cite unhidden>把凤姐她的所作所为我都</cite>）对出（<cite unhidden>词</cite>）来，我替（<cite unhidden>尤二姐</cite>）你报仇。”（<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">贾琏、凤姐彻底<a unhidden title="连理" href="a064.htm#P19L3"><sub hidden>第64回19.3节:</sub>决裂</a>（<a unhidden title="连理" href="original\c064.htm#P11L3"><sub hidden>第64回11.3节:</sub>原文</a>）。</u>）)";
  string compareTo5 =
      R"(14.4   （贾琏）揭起衾单一看，只见这尤二姐面色如生，比活着还美貌。贾琏又搂着大哭，只叫“奶奶，你死的不明，都是我坑了你！”（还是像当初↑按大房（↑原文）叫尤二姐“奶奶”）贾蓉忙上来劝（贾琏）：“叔叔解着些儿（伤悲），（都是）我这个姨娘（尤二姐她）自己没福。”说着，又向南指（着梨香院和）↑大观园的界墙，贾琏会意（凤姐此时仍在偷听），只悄悄跌脚说：“我忽略了（，只当凤姐她是好意对尤二姐），终久（把凤姐她的所作所为我都）对出（词）来，我替（尤二姐）你报仇。”（贾琏、凤姐彻底↑决裂（↑原文）。）)";

  string line6 =
      R"(<a unhidden id="P7L2">7.2</a>&nbsp;&nbsp; <strong unhidden>西施一代倾城逐浪花，吴宫空自忆儿家。效颦莫笑东村女，头白溪边尚浣纱。</strong>&nbsp;&nbsp;&nbsp;&nbsp;<samp unhidden font style="font-size: 12pt; font-family: 宋体; color:#ff00ff">日日在吴王夫差的宫殿里思念儿时的家园又如何，倾国倾城的一代佳人西施却再也没有回到当年浣纱的耶溪边，相反，如溪里那浪花互相追逐着奔向远方，她也消失不归。所以，也不必嘲笑效颦的东施，毕竟她相安无事地在溪边一直浣纱到白头。</samp>（<a unhidden href="attachment\b003_2.htm">西施</a>）（<a unhidden href="attachment\b064_7.htm">临江仙·滚滚长江东逝水</a>）（<a unhidden href="attachment\b064_8.htm">念奴娇·赤壁怀古</a>）（<a unhidden href="attachment\b064_9.htm">杜甫《登高》</a>）（<a unhidden href="attachment\b064_10.htm">王维《西施咏》</a>）（<a unhidden href="attachment\b064_11.htm">《庄子·天运》 东施效颦</a>）<br>)";
  string compareTo6 =
      R"(7.2   西施一代倾城逐浪花，吴宫空自忆儿家。效颦莫笑东村女，头白溪边尚浣纱。    日日在吴王夫差的宫殿里思念儿时的家园又如何，倾国倾城的一代佳人西施却再也没有回到当年浣纱的耶溪边，相反，如溪里那浪花互相追逐着奔向远方，她也消失不归。所以，也不必嘲笑效颦的东施，毕竟她相安无事地在溪边一直浣纱到白头。（西施）（临江仙·滚滚长江东逝水）（念奴娇·赤壁怀古）（杜甫《登高》）（王维《西施咏》）（《庄子·天运》 东施效颦）<br>)";
  string line7 =
      R"(<a unhidden id="P4L1">4.1</a>&nbsp;&nbsp; <strong unhidden>桃花帘外东风软，桃花帘内晨妆懒。帘外桃花帘内人，人与桃花隔不远。</strong>&nbsp;&nbsp;&nbsp;&nbsp;<samp unhidden font style="font-size: 12pt; font-family: 宋体; color:#ff00ff">轻柔的春风又一次吹开了帘外的桃花，不禁让我回想起帘内的美人曾经情绪慵懒、无心梳妆。看似仅仅一帘之隔，帘外的桃花和帘内人隔得并不远。岂知隔花人远天涯近，这桃花和美人已经天各一方。</samp>（<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">“不远”对应“月痕”</u>）<br>)";
  string compareTo7 =
      R"(4.1   桃花帘外东风软，桃花帘内晨妆懒。帘外桃花帘内人，人与桃花隔不远。    轻柔的春风又一次吹开了帘外的桃花，不禁让我回想起帘内的美人曾经情绪慵懒、无心梳妆。看似仅仅一帘之隔，帘外的桃花和帘内人隔得并不远。岂知隔花人远天涯近，这桃花和美人已经天各一方。（“不远”对应“月痕”）<br>)";

  string line9 =
      R"(<a unhidden id="P7L1">7.1</a>&nbsp;&nbsp; 此时宝玉正坐着（<cite unhidden>发呆</cite>）纳闷，想袭人之母不知是死是活（<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">独宝玉一人记挂袭人因母伤悲，黛玉则<a unhidden title="落后" href="a052.htm#P7L4"><sub hidden>第52回7.4节:</sub>担心袭人不在无人照顾宝玉</a>（<a unhidden title="落后" href="original\c052.htm#P5L2"><sub hidden>第52回5.2节:</sub>原文</a>）</u>），忽听见晴雯如此说，便自己起身出去，放下镜套，划上消息，进来笑（<cite unhidden>向晴雯</cite>）道：“你们暖和罢，（<cite unhidden>我</cite>）都（<cite unhidden>放</cite>）完了。”晴雯笑道：“（<cite unhidden>我这</cite>）终久暖和不成的，我又想起来汤婆子<a unhidden title="IMAGE" href="a013.htm#xlong.jpg">（图示：汤婆子）</a> （<cite unhidden>放在被窝里面的热水壶</cite>）还没拿来呢。”麝月道：“这（<cite unhidden>也</cite>）难为你（<cite unhidden>这么暖和还</cite>）想着（<cite unhidden>汤婆子也</cite>）！（<cite unhidden>宝玉</cite>）他素日（<cite unhidden>睡在暖阁</cite>）又不要汤婆子，（<cite unhidden>就咱们俩要汤婆子，</cite>）咱们那熏笼上暖和（<cite unhidden>咱们俩今天都睡熏笼上</cite>），比不得那（<cite unhidden>外屋</cite>）屋里（<cite unhidden>咱们常睡的</cite>）炕冷，今儿可以不用（<cite unhidden>汤婆子</cite>）。”（<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">平时看样子都是袭人在暖阁里外面床上陪睡</u>）宝玉笑道：“（<cite unhidden>按照你们说的</cite>）这个话，你们两个都在那（<cite unhidden>熏笼</cite>）上头睡了，我这（<cite unhidden>暖阁里面的</cite>）外边（<cite unhidden>那张床上</cite>）没个人（<cite unhidden>陪睡</cite>），我（<cite unhidden>一个人</cite>）怪怕的，一夜也睡不着。”（<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">暖阁里面除了宝玉自己睡的主床，还有一张“外面”的陪睡的床。</u>）晴雯道：“我（<cite unhidden>反正</cite>）是在（<cite unhidden>熏笼</cite>）这里。麝月（<cite unhidden>你</cite>）往（<cite unhidden>暖阁里宝玉</cite>）他外边（<cite unhidden>那张床上</cite>）睡去。”说话之间，天已二更，麝月早已放下（<cite unhidden>宝玉床的</cite>）帘幔，移灯炷香，伏侍宝玉卧下，二人方睡。（<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">晴雯一点争着靠近宝玉的心都没有，宝玉却慢慢靠近晴雯，看下面邀她进自己被窝。袭人回去这次是转折点，虽然自从王夫人收了的时候<a unhidden title="同房" href="a077.htm#P16L3"><sub hidden>第77回16.3节:</sub>她就离宝玉渐渐远了</a>（<a unhidden title="同房" href="original\c077.htm#P9L2"><sub hidden>第77回9.2节:</sub>原文</a>）。</u>）)";
  string compareTo9 =
      R"(7.1   此时宝玉正坐着（发呆）纳闷，想袭人之母不知是死是活（独宝玉一人记挂袭人因母伤悲，黛玉则↑担心袭人不在无人照顾宝玉（↑原文）），忽听见晴雯如此说，便自己起身出去，放下镜套，划上消息，进来笑（向晴雯）道：“你们暖和罢，（我）都（放）完了。”晴雯笑道：“（我这）终久暖和不成的，我又想起来汤婆子↑（图示：汤婆子） （放在被窝里面的热水壶）还没拿来呢。”麝月道：“这（也）难为你（这么暖和还）想着（汤婆子也）！（宝玉）他素日（睡在暖阁）又不要汤婆子，（就咱们俩要汤婆子，）咱们那熏笼上暖和（咱们俩今天都睡熏笼上），比不得那（外屋）屋里（咱们常睡的）炕冷，今儿可以不用（汤婆子）。”（平时看样子都是袭人在暖阁里外面床上陪睡）宝玉笑道：“（按照你们说的）这个话，你们两个都在那（熏笼）上头睡了，我这（暖阁里面的）外边（那张床上）没个人（陪睡），我（一个人）怪怕的，一夜也睡不着。”（暖阁里面除了宝玉自己睡的主床，还有一张“外面”的陪睡的床。）晴雯道：“我（反正）是在（熏笼）这里。麝月（你）往（暖阁里宝玉）他外边（那张床上）睡去。”说话之间，天已二更，麝月早已放下（宝玉床的）帘幔，移灯炷香，伏侍宝玉卧下，二人方睡。（晴雯一点争着靠近宝玉的心都没有，宝玉却慢慢靠近晴雯，看下面邀她进自己被窝。袭人回去这次是转折点，虽然自从王夫人收了的时候↑她就离宝玉渐渐远了（↑原文）。）)";

  CoupledBodyTextWithLink bodyText;
  bodyText.setFilePrefixFromFileType(FILE_TYPE::MAIN);
  array<string, 7> lineSet = {line3, line2, line4, line5, line6, line7, line9};
  array<string, 7> compareToSet = {compareTo3, compareTo2, compareTo4,
                                   compareTo5, compareTo6, compareTo7,
                                   compareTo9};
  for (size_t i = 0; i < lineSet.size(); i++) {
    string line = lineSet[i];
    string compareTo = compareToSet[i];
    bodyText.getDisplayString(line);
    printCompareResult(bodyText.getResultDisplayString(), compareTo);
    SEPERATE("compare test " + TurnToString(i), " finished ");
  }

  string line10 =
      R"(（<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">不问不成人的事情，反而问谁说的，全都是王家的体面更重要。<a unhidden href="attachment\b062_2.htm">旧诗 哭刘司户二首</a></u>）)";
  string line11 =
      R"(<a unhidden id="P1L1">1.1</a>&nbsp;&nbsp; <u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">总批：此回专为佞佛邀福者下一针砭。</u><br>)";
  string line12 =
      R"(<a unhidden id="P1L2">1.2</a>&nbsp;&nbsp; <u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">玉皇庙，两番描写，俱是热闹时候。即后文荐亡，亦是热闹之时，特特与永福寺对照也，看他平空撰出两付对联，一个疏头，却使玉皇庙是真庙，吴道官、西门庆等俱是活人。妙绝之笔。</u><br>)";
  string line13 = line3;
  array<string, 4> lineSet2 = {line10, line11, line12, line13};
  for (size_t i = 0; i < lineSet2.size(); i++) {
    string line = lineSet2[i];
    bodyText.getDisplayString(line, true);
    auto typeSet = bodyText.getResultSet();
    FUNCTION_OUTPUT << Object::typeSetAsString(typeSet) << endl;
    SEPERATE("type test " + TurnToString(i), " finished ");
  }
}

void testNumberingStatistics() {
  CoupledBodyTextWithLink::setReferFilePrefix(
      getBodyTextFilePrefixFromFileType(FILE_TYPE::MAIN));
  CoupledBodyTextWithLink::setStatisticsOutputFilePath(
      getStatisticsOutputFilePathFromString(MAIN));
  CoupledBodyTextWithLink::clearExistingNumberingStatistics();
  CoupledBodyTextWithLink bodyText;
  bodyText.setFilePrefixFromFileType(FILE_TYPE::MAIN);
  bodyText.setFileAndAttachmentNumber(72);
  bodyText.doStatisticsByScanningLines();
}

void testFunctions(int num) {
  SEPERATE("HLM test", " started ");
  switch (num) {
  case 1:
    testSearchTextIsOnlyPartOfOtherKeys();
    break;
  case 2:
    testLineNumber();
    break;
  case 3:
    testLinkOperation();
    break;
  case 4:
    testAttachmentOperations();
    break;
  case 5:
    testContainer(1);
    break;
  case 6:
    testContainer(2);
    break;
  case 7:
    testPoem();
    break;
  case 8:
    testSpace();
    break;
  case 9:
    testMixedObjects();
    break;
  case 10:
    testNumberingStatistics();
    break;
  default:
    FUNCTION_OUTPUT << "no test executed." << endl;
  }
}
