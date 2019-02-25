#include "coupledBodyTextWithLink.hpp"

void CoupledBodyTextWithLink::removePersonalCommentsOverNumberedFiles() {
  setInputOutputFiles();
  ifstream infile(m_inputFile);
  if (!infile) {
    cout << "file doesn't exist:" << m_inputFile << endl;
    return;
  }
  ofstream outfile(m_outputFile);
  string inLine{"not found"};
  while (!infile.eof()) // To get all the lines.
  {
    getline(infile, inLine); // Saves the line in inLine.
    if (debug >= LOG_INFO)
      cout << inLine << endl;
    auto orgLine = inLine; // inLine would change in loop below
    string start = personalCommentStartChars;
    string end = personalCommentEndChars;
    string to_replace = "";
    // first loop to remove all personal Comments
    auto removePersonalCommentLine = orgLine;
    auto personalCommentBegin = removePersonalCommentLine.find(start);
    while (personalCommentBegin != string::npos) {
      auto personalCommentEnd = removePersonalCommentLine.find(end);
      string personalComment = removePersonalCommentLine.substr(
          personalCommentBegin,
          personalCommentEnd + end.length() - personalCommentBegin);
      //      cout << personalComment << endl;
      to_replace = personalComment;
      auto replaceBegin = orgLine.find(to_replace);
      orgLine.replace(replaceBegin, to_replace.length(), "");
      removePersonalCommentLine =
          removePersonalCommentLine.substr(personalCommentEnd + end.length());
      // find next personalComment in the removePersonalCommentLine
      personalCommentBegin = removePersonalCommentLine.find(start);
    }
    // the second loop to remove all expected attachment link from result
    // orgLine
    auto removeSpecialLinkLine = orgLine;
    string linkStart = linkStartChars;
    string linkEnd = linkEndChars;
    auto specialLinkBegin = removeSpecialLinkLine.find(linkStart);
    while (specialLinkBegin != string::npos) {
      auto specialLinkEnd = removeSpecialLinkLine.find(linkEnd);
      string specialLink = removeSpecialLinkLine.substr(
          specialLinkBegin,
          specialLinkEnd + linkEnd.length() - specialLinkBegin);
      LinkFromAttachment m_linkPtr(m_file, specialLink);
      auto num = make_pair(m_linkPtr.getchapterNumer(),
                           m_linkPtr.getattachmentNumber());
      if (m_linkPtr.isTargetToOtherAttachmentHtm() and
          LinkFromMain::getAttachmentType(num) == ATTACHMENT_TYPE::PERSONAL) {
        cout << specialLink << endl;
        to_replace = specialLink;
        auto replaceBegin = orgLine.find(to_replace);
        orgLine.replace(replaceBegin, to_replace.length(), "");
      }
      removeSpecialLinkLine =
          removeSpecialLinkLine.substr(specialLinkEnd + linkEnd.length());
      // find next specialLink in the removeSpecialLinkLine
      specialLinkBegin = removeSpecialLinkLine.find(linkStart);
    }

    outfile << orgLine << endl;
  }
}

/**
 * fix links of certain type in file which refer to one of file in referFiles
 * @param file
 * @param referFiles
 */
void CoupledBodyTextWithLink::fixLinksFromFile(fileSet referFiles, int minPara,
                                               int maxPara, int minLine,
                                               int maxLine) {
  setInputOutputFiles();

  ifstream infile(m_inputFile);
  if (!infile) {
    cout << "file doesn't exist:" << m_inputFile << endl;
    return;
  }
  ofstream outfile(m_outputFile);
  string inLine{""};
  while (!infile.eof()) // To get all the lines.
  {
    getline(infile, inLine); // Saves the line in inLine.
    auto orgLine = inLine;   // inLine would change in loop below
    LineNumber ln;
    ln.loadFirstFromContainedLine(orgLine);
    if (ln.isParagraphHeader() or not ln.valid() or
        not ln.isWithinLineRange(minPara, maxPara, minLine, maxLine)) {
      outfile << orgLine << endl;
      continue; // not fix headers or non-numbered lines
    }
    inLine = inLine.substr(
        ln.generateLinePrefix().length()); // skip line number link
    if (debug >= LOG_INFO)
      cout << inLine << endl;
    auto start = linkStartChars;
    string targetFile{""};
    do {
      auto linkBegin = inLine.find(start);
      if (linkBegin == string::npos) // no link any more, continue with next
                                     // line
        break;
      auto linkEnd = inLine.find(linkEndChars, linkBegin);
      auto link = inLine.substr(linkBegin, linkEnd + 4 - linkBegin);

      if (m_attachNumber == 0) {
        m_linkPtr = std::make_unique<LinkFromMain>(m_file, link);
      } else {
        m_linkPtr = std::make_unique<LinkFromAttachment>(
            m_file + attachmentFileMiddleChar + TurnToString(m_attachNumber),
            link);
      }
      m_linkPtr->readReferFileName(
          link); // second step of construction, this is
                 // needed to check isTargetToSelfHtm
      if (m_linkPtr->isTargetToOtherAttachmentHtm()) {
        m_linkPtr->fixFromString(link); // third step of construction
        m_linkPtr->setSourcePara(ln);
        m_linkPtr->doStatistics();
      }
      if (m_linkPtr->isTargetToSelfHtm()) {
        m_linkPtr->setSourcePara(ln);
        m_linkPtr->fixFromString(link); // third step of construction
        if (m_linkPtr->needUpdate())    // replace old value
        {
          auto orglinkBegin = orgLine.find(link);
          orgLine.replace(orglinkBegin, link.length(), m_linkPtr->asString());
        }
      }
      if (m_linkPtr->isTargetToOtherMainHtm()) {
        targetFile = m_linkPtr->getChapterName();
        auto e = find(referFiles.begin(), referFiles.end(), targetFile);
        if (e != referFiles.end()) // need to check and fix
        {
          m_linkPtr->fixFromString(link); // third step of construction
          m_linkPtr->setSourcePara(ln);
          string next = originalLinkStartChars + linkStartChars;
          bool needAddOrginalLink = true;
          // still have above "next" and </a>
          if (inLine.length() > (link.length() + next.length() + 4)) {
            if (inLine.substr(linkEnd + 4, next.length()) == next) {
              // skip </a> and first parenthesis of next
              auto followingLink = inLine.substr(
                  linkEnd + next.length() + 2); // find next link in the inLine
              if (m_attachNumber == 0) {
                m_followingLinkPtr =
                    std::make_unique<LinkFromMain>(m_file, followingLink);
              } else {
                m_followingLinkPtr = std::make_unique<LinkFromAttachment>(
                    m_file + attachmentFileMiddleChar +
                        TurnToString(m_attachNumber),
                    followingLink);
              }
              if (m_followingLinkPtr->isTargetToOriginalHtm()) {
                needAddOrginalLink = false;
              }
            }
          }
          if (needAddOrginalLink)
            m_linkPtr->generateLinkToOrigin();
          m_linkPtr->doStatistics();
          if (m_linkPtr->needUpdate()) // replace old value
          {
            auto orglinkBegin = orgLine.find(link);
            orgLine.replace(orglinkBegin, link.length(), m_linkPtr->asString());
          }
        }
      }
      if (m_linkPtr->isTargetToOriginalHtm()) {
        targetFile = m_linkPtr->getChapterName();
        auto e = find(referFiles.begin(), referFiles.end(), targetFile);
        if (e != referFiles.end()) // need to check and fix
        {
          m_linkPtr->fixFromString(link); // third step of construction
          if (m_linkPtr->needUpdate())    // replace old value
          {
            auto orglinkBegin = orgLine.find(link);
            if (debug >= LOG_INFO)
              SEPERATE("isTargetToOriginalHtm", orgLine + "\n" + link);
            orgLine.replace(orglinkBegin, link.length(), m_linkPtr->asString());
          }
        }
      }
      inLine = inLine.substr(linkEnd + 4); // find next link in the inLine
    } while (1);
    outfile << orgLine << endl;
  }
}

/**
 * get size of links, Comments, image reference, poems, personal views embedded
 * to get actual size without rendering one line,
 */
int CoupledBodyTextWithLink::sizeAfterRendering(const string &lineStr) {
  return 0;
}

using OffsetToObjectType = std::map<size_t, OBJECT_TYPE>;
using ObjectTypeToOffset = std::map<OBJECT_TYPE, size_t>;
using ObjectPtr = std::unique_ptr<Object>;
OffsetToObjectType offsetOfTypes;
ObjectTypeToOffset foundTypes;
struct LinkStringInfo {
  size_t startOffset{0};
  size_t endOffset{0};
  bool embedded{false};
};
using LinkStringTable = std::map<size_t, LinkStringInfo>;
using PersonalCommentStringTable = std::map<size_t, size_t>;
using PoemTranslationStringTable = std::map<size_t, size_t>;
LinkStringTable linkStringTable;
PersonalCommentStringTable personalCommentStringTable;
PoemTranslationStringTable poemTranslationStringTable;

string getNameOfObjectType(OBJECT_TYPE type) {
  if (type == OBJECT_TYPE::LINENUMBER)
    return "LINENUMBER";
  else if (type == OBJECT_TYPE::IMAGEREF)
    return "IMAGEREF";
  else if (type == OBJECT_TYPE::SPACE)
    return "SPACE";
  else if (type == OBJECT_TYPE::POEM)
    return "POEM";
  else if (type == OBJECT_TYPE::LINKFROMMAIN)
    return "LINKFROMMAIN";
  else if (type == OBJECT_TYPE::PERSONALCOMMENT)
    return "PERSONALCOMMENT";
  else if (type == OBJECT_TYPE::POEMTRANSLATION)
    return "POEMTRANSLATION";
  return "";
}

void printOffsetToObjectType() {
  for (const auto &element : offsetOfTypes) {
    cout << element.first << "  " << getNameOfObjectType(element.second)
         << endl;
  }
}

void printObjectTypeToOffset() {
  for (const auto &element : foundTypes) {
    cout << getNameOfObjectType(element.first) << "  " << element.second
         << endl;
  }
}

void printLinkStringTable() {
  if (not linkStringTable.empty())
    cout << "linkStringTable:" << endl;
  for (const auto &element : linkStringTable) {
    cout << element.first << "  " << element.second.endOffset << "  "
         << element.second.embedded << endl;
  }
}

void printPersonalCommentStringTable() {
  if (not personalCommentStringTable.empty())
    cout << "personalCommentStringTable:" << endl;
  for (const auto &element : personalCommentStringTable) {
    cout << element.first << "  " << element.second << endl;
  }
}

void printPoemTranslationStringTable() {
  if (not poemTranslationStringTable.empty())
    cout << "poemTranslationStringTable:" << endl;
  for (const auto &element : poemTranslationStringTable) {
    cout << element.first << "  " << element.second << endl;
  }
}

ObjectPtr createObjectFromType(OBJECT_TYPE type) {
  if (type == OBJECT_TYPE::LINENUMBER)
    return std::make_unique<LineNumber>();
  else if (type == OBJECT_TYPE::IMAGEREF)
    return std::make_unique<ImageReferText>();
  else if (type == OBJECT_TYPE::SPACE)
    return std::make_unique<Space>();
  else if (type == OBJECT_TYPE::POEM)
    return std::make_unique<Poem>();
  else if (type == OBJECT_TYPE::LINKFROMMAIN)
    return std::make_unique<LinkFromMain>();
  else if (type == OBJECT_TYPE::PERSONALCOMMENT)
    return std::make_unique<PersonalComment>();
  else if (type == OBJECT_TYPE::POEMTRANSLATION)
    return std::make_unique<PoemTranslation>();
  return nullptr;
}

string getStartTagOfObjectType(OBJECT_TYPE type) {
  if (type == OBJECT_TYPE::LINENUMBER)
    return UnhiddenLineNumberStart;
  else if (type == OBJECT_TYPE::IMAGEREF)
    return ImgRefBeginChars;
  else if (type == OBJECT_TYPE::SPACE)
    return space;
  else if (type == OBJECT_TYPE::POEM)
    return poemBeginChars;
  else if (type == OBJECT_TYPE::LINKFROMMAIN)
    return linkStartChars;
  else if (type == OBJECT_TYPE::PERSONALCOMMENT)
    return personalCommentStartChars;
  else if (type == OBJECT_TYPE::POEMTRANSLATION)
    return poemTranslationBeginChars;
  return "";
}

string getEndTagOfObjectType(OBJECT_TYPE type) {
  if (type == OBJECT_TYPE::LINKFROMMAIN)
    return linkEndChars;
  else if (type == OBJECT_TYPE::PERSONALCOMMENT)
    return personalCommentEndChars;
  else if (type == OBJECT_TYPE::POEMTRANSLATION)
    return poemTranslationEndChars;
  return "";
}

bool isEmbeddedObject(OBJECT_TYPE type, size_t offset) {
  if (type == OBJECT_TYPE::LINKFROMMAIN) {
    try {
      auto linkInfo = linkStringTable.at(offset);
      return linkInfo.embedded;
    } catch (exception &) {
      cout << "no such object " << getNameOfObjectType(type) << "at " << offset;
    }
  }
  return false;
}

void searchForEmbededLinks() {
  for (auto &linkInfo : linkStringTable) {
    for (const auto &element : personalCommentStringTable) {
      if (linkInfo.second.startOffset > element.first and
          linkInfo.second.endOffset < element.second) {
        linkInfo.second.embedded = true;
        break;
      }
    }
  }
  for (auto &linkInfo : linkStringTable) {
    for (const auto &element : poemTranslationStringTable) {
      if (linkInfo.second.startOffset > element.first and
          linkInfo.second.endOffset < element.second) {
        linkInfo.second.embedded = true;
        break;
      }
    }
  }
}

void scanForTypes(string containedLine) {
  for (const auto &type : {OBJECT_TYPE::LINENUMBER, OBJECT_TYPE::IMAGEREF,
                           OBJECT_TYPE::SPACE, OBJECT_TYPE::POEM}) {
    auto offset = containedLine.find(getStartTagOfObjectType(type));
    if (offset != string::npos) {
      foundTypes[type] = offset;
      offsetOfTypes[offset] = type;
    }
  }
  bool firstOccurence = true;
  auto offset = string::npos;
  do {
    offset = containedLine.find(
        getStartTagOfObjectType(OBJECT_TYPE::PERSONALCOMMENT),
        (firstOccurence) ? 0 : offset + 1);
    if (offset == string::npos)
      break;
    personalCommentStringTable[offset] = containedLine.find(
        getEndTagOfObjectType(OBJECT_TYPE::PERSONALCOMMENT), offset);
    if (firstOccurence == true) {
      foundTypes[OBJECT_TYPE::PERSONALCOMMENT] = offset;
      offsetOfTypes[offset] = OBJECT_TYPE::PERSONALCOMMENT;
      firstOccurence = false;
    }
  } while (true);

  firstOccurence = true;
  offset = string::npos;
  do {
    offset = containedLine.find(
        getStartTagOfObjectType(OBJECT_TYPE::POEMTRANSLATION),
        (firstOccurence) ? 0 : offset + 1);
    if (offset == string::npos)
      break;
    poemTranslationStringTable[offset] = containedLine.find(
        getEndTagOfObjectType(OBJECT_TYPE::POEMTRANSLATION), offset);
    if (firstOccurence == true) {
      foundTypes[OBJECT_TYPE::POEMTRANSLATION] = offset;
      offsetOfTypes[offset] = OBJECT_TYPE::POEMTRANSLATION;
      firstOccurence = false;
    }
  } while (true);

  offset = string::npos;
  try {
    auto type = offsetOfTypes.at(0);
    // skip first line number as a link actually
    if (type == OBJECT_TYPE::LINENUMBER)
      offset = containedLine.find(
          getStartTagOfObjectType(OBJECT_TYPE::LINKFROMMAIN), 1);
  } catch (exception &) {
    offset = containedLine.find(
        getStartTagOfObjectType(OBJECT_TYPE::LINKFROMMAIN), 0);
  }
  if (offset != string::npos) {
    foundTypes[OBJECT_TYPE::LINKFROMMAIN] = offset;
    offsetOfTypes[offset] = OBJECT_TYPE::LINKFROMMAIN;
    LinkStringInfo info{
        offset,
        containedLine.find(getEndTagOfObjectType(OBJECT_TYPE::LINKFROMMAIN),
                           offset),
        false};
    linkStringTable[offset] = info;
    do {
      offset = containedLine.find(
          getStartTagOfObjectType(OBJECT_TYPE::LINKFROMMAIN), offset + 1);
      if (offset == string::npos)
        break;
      LinkStringInfo info{
          offset,
          containedLine.find(getEndTagOfObjectType(OBJECT_TYPE::LINKFROMMAIN),
                             offset),
          false};
      linkStringTable[offset] = info;
    } while (true);
  }
  //  printLinkStringTable();
  searchForEmbededLinks();
}

void testMixedObjects() {
  //  string line =
  //      R"(<a unhidden id="P3L2">3.2</a>&nbsp;&nbsp;
  //      贾母又说：“请姑娘们来。今日有远客来，就不必上学去了。”<a hidden
  //      href="a057.htm#top">原是（<cite unhidden>薛姨妈1</cite>）老奶奶（<cite
  //      unhidden>薛姨妈1</cite>）使唤的</a>众人答应了一声，便派了两个人去请。没多久，只见三个奶妈和五六个丫鬟，簇拥着三个姊妹来了。第一个乃二姐<var
  //      unhidden font style="font-size: 13.5pt; font-family:楷体;
  //      color:#ff00ff">迎春----->（见右图）</var>，生的肌肤微丰，合中身材，腮凝新荔，鼻腻鹅脂，温柔沉默，观之可亲。<a
  //      unhidden href="attachment\b018_7.htm">happy</a>第二个是三妹<var
  //      unhidden font style="font-size: 13.5pt; font-family:楷体;
  //      color:#ff00ff">（见左图）<-----探春</var>，削肩细腰，长挑身材，鸭蛋脸面，俊眼修眉，顾盼神飞，文彩精华，见之忘俗。（<cite
  //      unhidden>迎春外表就有懦小姐的感觉，探春外表就文采甚好，可起诗社。</cite>）<br>)";

  //  string line =
  //      R"(前儿老太太（<cite unhidden>贾母</cite>）因要把<a
  //      href="a050.htm#P15L2"><i hidden>说媒</i><sub
  //      hidden>第50章15.2节:</sub>你妹妹（<cite
  //      unhidden>薛宝琴</cite>）说给宝玉</a>（<a unhidden
  //      href="original\c050.htm#P14L2"><i hidden>说媒</i><sub
  //      hidden>第50章14.2节:</sub>原文</a>），偏生（<cite
  //      unhidden>薛宝琴</cite>）又有了人家（<cite
  //      unhidden>梅翰林家</cite>），不然（<cite
  //      unhidden>宝琴宝玉他二人</cite>）倒是一门好亲。老太太离了鸳鸯，饭也吃不下去的，哪里就舍得了？（<cite
  //      unhidden>凤姐并不知道贾赦要鸳鸯是要<a unhidden href="a071.htm#P7L2"><i
  //      hidden>作兴</i><sub hidden>第71章7.2节:</sub>争宠之意</a>（<a unhidden
  //      href="original\c071.htm#P5L3"><i hidden>作兴</i><sub
  //      hidden>第71章5.3节:</sub>原文</a>）（<u unhidden
  //      style="text-decoration-color: #F0BEC0;text-decoration-style:
  //      wavy;opacity: 0.4">这句又接到宝玉生日探春让李纨打黛玉，李纨说黛玉<a
  //      unhidden href="a063.htm#P13L3"><i hidden>挨打</i><sub
  //      hidden>第63章13.3节:</sub>人家不得贵婿反挨打</a>（<a unhidden
  //      href="original\c063.htm#P6L3"><i hidden>挨打</i><sub
  //      hidden>第63章6.3节:</sub>原文</a>），黛玉的婚姻是镜中花，他们二人的一个不能完成的愿望而已。</u>）)";

  string line =
      R"(<a unhidden id="P8L3">8.3</a>&nbsp;&nbsp; <strong unhidden>惯养娇生笑你痴，菱花空对雪澌澌。好防佳节元宵后，便是烟消火灭时。</strong>&nbsp;&nbsp;&nbsp;&nbsp;<samp unhidden font style="font-size: 13.5pt; font-family:楷体; color:#ff00ff">你这么痴心 娇生惯养她，实在是可笑。你知道她有多么生不逢时吗？<a  href="a080.htm#P1L1"><i hidden>菱角菱花</i><sub hidden>第80章1.1节:</sub>菱角菱花皆盛于秋</a>（<a unhidden href="original\c080.htm#P1L3"><i hidden>菱角菱花</i><sub hidden>第80章1.3节:</sub>原文</a>），可却毫无办法要面对茫茫大雪。要小心元宵佳节一过，一切都会烟消云散（<cite unhidden> <a unhidden href="a022.htm#P13L4"><i hidden>清净孤独</i><sub hidden>第22章13.4节:</sub>不详灯谜</a>（<a unhidden href="original\c022.htm#P12L1"><i hidden>清净孤独</i><sub hidden>第22章12.1节:</sub>原文</a>）、<a unhidden href="a054.htm#P13L1"><i hidden>进贡</i><sub hidden>第54章13.1节:</sub>聋子放炮仗</a>（<a unhidden href="original\c054.htm#P8L2"><i hidden>进贡</i><sub hidden>第54章8.2节:</sub>原文</a>）、<a unhidden href="a058.htm#P1L2"><i hidden>按爵</i><sub hidden>第58章1.2节:</sub>老太妃薨毙</a>（<a unhidden href="original\c058.htm#P1L2"><i hidden>按爵</i><sub hidden>第58章1.2节:</sub>原文</a>）。</cite>）</samp><br>)";
  LineNumber ln;
  ln.loadFirstFromContainedLine(line);
  if (ln.isParagraphHeader()) {
    return;
  }
  // after this, there could be only one line number at the beginning
  scanForTypes(line);
  printOffsetToObjectType();
  printLinkStringTable();
  //  printPersonalCommentStringTable();
  printPoemTranslationStringTable();

  do {
    if (offsetOfTypes.empty())
      break;
    auto first = offsetOfTypes.begin();
    auto type = first->second;
    auto offset = first->first;
    if (not isEmbeddedObject(type, offset)) {
      auto current = createObjectFromType(type);
      current->loadFirstFromContainedLine(line, offset);
      cout << "whole string: " << current->getWholeString() << endl;
      cout << "display as:" << current->getDisplayString() << "||" << endl;
    }
    offsetOfTypes.erase(first);
    auto nextOffsetOfSameType =
        line.find(getStartTagOfObjectType(type), offset + 1);
    do {
      if (nextOffsetOfSameType != string::npos and
          isEmbeddedObject(type, nextOffsetOfSameType))
        nextOffsetOfSameType =
            line.find(getStartTagOfObjectType(type), nextOffsetOfSameType + 1);
      else
        break;
    } while (true);
    if (nextOffsetOfSameType != string::npos) {
      foundTypes[type] = nextOffsetOfSameType;
      offsetOfTypes[nextOffsetOfSameType] = type;
    }
    printOffsetToObjectType();
  } while (true);
}
