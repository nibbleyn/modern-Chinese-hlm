#include "coupledBodyTextWithLink.hpp"

void CoupledBodyTextWithLink::printLinkStringTable() {
  if (not m_linkStringTable.empty())
    cout << "m_linkStringTable:" << endl;
  for (const auto &element : m_linkStringTable) {
    cout << element.first << "  " << element.second.endOffset << "  "
         << element.second.embedded << endl;
  }
}

void CoupledBodyTextWithLink::printCommentStringTable() {
  if (not m_commentStringTable.empty())
    cout << "m_commentStringTable:" << endl;
  for (const auto &element : m_commentStringTable) {
    cout << element.first << "  " << element.second.endOffset << "  "
         << element.second.embedded << endl;
  }
}

void CoupledBodyTextWithLink::printPersonalCommentStringTable() {
  if (not m_personalCommentStringTable.empty())
    cout << "m_personalCommentStringTable:" << endl;
  for (const auto &element : m_personalCommentStringTable) {
    cout << element.first << "  " << element.second << endl;
  }
}

void CoupledBodyTextWithLink::printPoemTranslationStringTable() {
  if (not m_poemTranslationStringTable.empty())
    cout << "m_poemTranslationStringTable:" << endl;
  for (const auto &element : m_poemTranslationStringTable) {
    cout << element.first << "  " << element.second << endl;
  }
}

bool CoupledBodyTextWithLink::isEmbeddedObject(OBJECT_TYPE type,
                                               size_t offset) {
  if (type == OBJECT_TYPE::LINKFROMMAIN) {
    try {
      auto linkInfo = m_linkStringTable.at(offset);
      return linkInfo.embedded;
    } catch (exception &) {
      cout << "no such object " << getNameOfObjectType(type) << "at " << offset;
    }
  }
  if (type == OBJECT_TYPE::COMMENT) {
    try {
      auto commentInfo = m_commentStringTable.at(offset);
      return commentInfo.embedded;
    } catch (exception &) {
      cout << "no such object " << getNameOfObjectType(type) << "at " << offset;
    }
  }
  return false;
}

void CoupledBodyTextWithLink::searchForEmbededLinks() {
  for (auto &linkInfo : m_linkStringTable) {
    for (const auto &element : m_personalCommentStringTable) {
      if (linkInfo.second.startOffset > element.first and
          linkInfo.second.endOffset < element.second) {
        linkInfo.second.embedded = true;
        break;
      }
    }
  }
  for (auto &linkInfo : m_linkStringTable) {
    for (const auto &element : m_poemTranslationStringTable) {
      if (linkInfo.second.startOffset > element.first and
          linkInfo.second.endOffset < element.second) {
        linkInfo.second.embedded = true;
        break;
      }
    }
  }
  for (auto &linkInfo : m_linkStringTable) {
    for (const auto &commentInfo : m_commentStringTable) {
      if (linkInfo.second.startOffset > commentInfo.second.startOffset and
          linkInfo.second.endOffset < commentInfo.second.endOffset) {
        linkInfo.second.embedded = true;
        break;
      }
    }
  }
  for (auto &commentInfo : m_commentStringTable) {
    for (const auto &linkInfo : m_linkStringTable) {
      if (linkInfo.second.startOffset < commentInfo.second.startOffset and
          linkInfo.second.endOffset > commentInfo.second.endOffset) {
        commentInfo.second.embedded = true;
        break;
      }
    }
  }
}

void CoupledBodyTextWithLink::scanForTypes(const string &containedLine) {
  for (const auto &type : {OBJECT_TYPE::LINENUMBER, OBJECT_TYPE::IMAGEREF,
                           OBJECT_TYPE::SPACE, OBJECT_TYPE::POEM}) {
    auto offset = containedLine.find(getStartTagOfObjectType(type));
    if (offset != string::npos) {
      m_foundTypes[type] = offset;
      m_offsetOfTypes[offset] = type;
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
    m_personalCommentStringTable[offset] = containedLine.find(
        getEndTagOfObjectType(OBJECT_TYPE::PERSONALCOMMENT), offset);
    if (firstOccurence == true) {
      m_foundTypes[OBJECT_TYPE::PERSONALCOMMENT] = offset;
      m_offsetOfTypes[offset] = OBJECT_TYPE::PERSONALCOMMENT;
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
    m_poemTranslationStringTable[offset] = containedLine.find(
        getEndTagOfObjectType(OBJECT_TYPE::POEMTRANSLATION), offset);
    if (firstOccurence == true) {
      m_foundTypes[OBJECT_TYPE::POEMTRANSLATION] = offset;
      m_offsetOfTypes[offset] = OBJECT_TYPE::POEMTRANSLATION;
      firstOccurence = false;
    }
  } while (true);

  firstOccurence = true;
  offset = string::npos;
  do {
    offset = containedLine.find(getStartTagOfObjectType(OBJECT_TYPE::COMMENT),
                                (firstOccurence) ? 0 : offset + 1);
    if (offset == string::npos)
      break;
    CommentStringInfo info{
        offset,
        containedLine.find(getEndTagOfObjectType(OBJECT_TYPE::COMMENT), offset),
        false};
    m_commentStringTable[offset] = info;
    if (firstOccurence == true) {
      m_foundTypes[OBJECT_TYPE::COMMENT] = offset;
      m_offsetOfTypes[offset] = OBJECT_TYPE::COMMENT;
      firstOccurence = false;
    }
  } while (true);

  offset = string::npos;
  try {
    auto type = m_offsetOfTypes.at(0);
    // skip first line number as a link actually
    if (type == OBJECT_TYPE::LINENUMBER)
      offset = containedLine.find(
          getStartTagOfObjectType(OBJECT_TYPE::LINKFROMMAIN), 1);
  } catch (exception &) {
    offset = containedLine.find(
        getStartTagOfObjectType(OBJECT_TYPE::LINKFROMMAIN), 0);
  }
  if (offset != string::npos) {
    m_foundTypes[OBJECT_TYPE::LINKFROMMAIN] = offset;
    m_offsetOfTypes[offset] = OBJECT_TYPE::LINKFROMMAIN;
    LinkStringInfo info{
        offset,
        containedLine.find(getEndTagOfObjectType(OBJECT_TYPE::LINKFROMMAIN),
                           offset),
        false};
    m_linkStringTable[offset] = info;
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
      m_linkStringTable[offset] = info;
    } while (true);
  }
  searchForEmbededLinks();
}

void CoupledBodyTextWithLink::render() {
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
    LineNumber ln;
    ln.loadFirstFromContainedLine(inLine);
    if (ln.isParagraphHeader()) {
      continue;
    }

    if (debug >= LOG_INFO) {
      cout << inLine << endl;
      cout << getDisplayString(inLine) << endl;
    }
    outfile << getDisplayString(inLine) << endl;
  }
}

string CoupledBodyTextWithLink::getDisplayString(const string &originalString) {
  scanForTypes(originalString);
  if (debug >= LOG_INFO) {
    printOffsetToObjectType();
    printLinkStringTable();
    printCommentStringTable();
    printPersonalCommentStringTable();
    printPoemTranslationStringTable();
  }
  string result;
  unsigned int endOfSubStringOffset = 0;
  do {
    if (m_offsetOfTypes.empty())
      break;
    auto first = m_offsetOfTypes.begin();
    auto type = first->second;
    auto offset = first->first;
    if (not isEmbeddedObject(type, offset)) {
      result += originalString.substr(endOfSubStringOffset,
                                      offset - endOfSubStringOffset);
      auto current = createObjectFromType(type);
      current->loadFirstFromContainedLine(originalString, offset);
      cout << "whole string: " << current->getWholeString() << endl;
      cout << "display as:" << current->getDisplayString() << "||" << endl;
      result += current->getDisplayString();
      // should add length of substring above loadFirstFromContainedLine gets
      // so require the string be fixed before
      endOfSubStringOffset = offset + current->length();
    }
    m_offsetOfTypes.erase(first);
    auto nextOffsetOfSameType =
        originalString.find(getStartTagOfObjectType(type), offset + 1);
    do {
      if (nextOffsetOfSameType != string::npos and
          isEmbeddedObject(type, nextOffsetOfSameType))
        nextOffsetOfSameType = originalString.find(
            getStartTagOfObjectType(type), nextOffsetOfSameType + 1);
      else
        break;
    } while (true);
    if (nextOffsetOfSameType != string::npos) {
      m_foundTypes[type] = nextOffsetOfSameType;
      m_offsetOfTypes[nextOffsetOfSameType] = type;
    }
    printOffsetToObjectType();
  } while (true);
  if (endOfSubStringOffset < originalString.length())
    result += originalString.substr(endOfSubStringOffset);
  m_foundTypes.clear();
  m_offsetOfTypes.clear();
  m_linkStringTable.clear();
  m_commentStringTable.clear();
  m_personalCommentStringTable.clear();
  m_poemTranslationStringTable.clear();
  return result;
}

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
void CoupledBodyTextWithLink::fixLinksFromFile(
    fileSet referMainFiles, fileSet referOriginalFiles, fileSet referJPMFiles,
    bool forceUpdate, int minPara, int maxPara, int minLine, int maxLine) {
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
      auto link =
          inLine.substr(linkBegin, linkEnd + linkEndChars.length() - linkBegin);

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
        if (forceUpdate or m_linkPtr->needUpdate()) // replace old value
        {
          auto orglinkBegin = orgLine.find(link);
          orgLine.replace(orglinkBegin, link.length(), m_linkPtr->asString());
        }
      }
      if (m_linkPtr->isTargetToOtherMainHtm()) {
        targetFile = m_linkPtr->getChapterName();
        auto e = find(referMainFiles.begin(), referMainFiles.end(), targetFile);
        if (e != referMainFiles.end()) // need to check and fix
        {
          m_linkPtr->fixFromString(link); // third step of construction
          m_linkPtr->setSourcePara(ln);
          string next = originalLinkStartChars + linkStartChars;
          bool needAddOrginalLink = true;
          // still have above "next" and </a>
          if (inLine.length() >
              (link.length() + next.length() + linkEndChars.length())) {
            if (inLine.substr(linkEnd + linkEndChars.length(), next.length()) ==
                next) {
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
          if (forceUpdate or m_linkPtr->needUpdate()) // replace old value
          {
            auto orglinkBegin = orgLine.find(link);
            orgLine.replace(orglinkBegin, link.length(), m_linkPtr->asString());
          }
        }
      }
      if (m_linkPtr->isTargetToJPMHtm()) {
        targetFile = m_linkPtr->getChapterName();
        auto e = find(referJPMFiles.begin(), referJPMFiles.end(), targetFile);
        if (e != referJPMFiles.end()) // need to check and fix
        {
          m_linkPtr->fixFromString(link); // third step of construction
          if (forceUpdate or m_linkPtr->needUpdate()) // replace old value
          {
            auto orglinkBegin = orgLine.find(link);
            if (debug >= LOG_INFO)
              SEPERATE("isTargetToJPMHtm", orgLine + "\n" + link);
            orgLine.replace(orglinkBegin, link.length(), m_linkPtr->asString());
          }
        }
      }
      if (m_linkPtr->isTargetToOriginalHtm()) {
        targetFile = m_linkPtr->getChapterName();
        auto e = find(referOriginalFiles.begin(), referOriginalFiles.end(),
                      targetFile);
        if (e != referOriginalFiles.end()) // need to check and fix
        {
          m_linkPtr->fixFromString(link); // third step of construction
          if (forceUpdate or m_linkPtr->needUpdate()) // replace old value
          {
            auto orglinkBegin = orgLine.find(link);
            if (debug >= LOG_INFO)
              SEPERATE("isTargetToOriginalHtm", orgLine + "\n" + link);
            orgLine.replace(orglinkBegin, link.length(), m_linkPtr->asString());
          }
        }
      }
      inLine = inLine.substr(
          linkEnd + linkEndChars.length()); // find next link in the inLine
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

void testMixedObjects() {
  string line1 =
      R"(<a unhidden id="P3L2">3.2</a>&nbsp;&nbsp; 贾母又说：“请姑娘们来。今日有远客来，就不必上学去了。”<a hidden href="a057.htm#top">原是（<cite unhidden>薛姨妈1</cite>）老奶奶（<cite unhidden>薛姨妈3</cite>）使唤的</a>众人答应了一声，便派了两个人去请。没多久，只见三个奶妈和五六个丫鬟，簇拥着三个姊妹来了。第一个乃二姐<var unhidden font style="font-size: 13.5pt; font-family:楷体; color:#ff00ff">迎春----->（见右图）</var>，生的肌肤微丰，合中身材，腮凝新荔，鼻腻鹅脂，温柔沉默，观之可亲。<a unhidden href="attachment\b018_7.htm">happy</a>第二个是三妹<var unhidden font style="font-size: 13.5pt; font-family:楷体; color:#ff00ff">（见左图）<-----探春</var>，削肩细腰，长挑身材，鸭蛋脸面，俊眼修眉，顾盼神飞，文彩精华，见之忘俗。（<cite unhidden>迎春外表就有懦小姐的感觉，探春外表就文采甚好，可起诗社。</cite>）)";
  string compareTo1 =
      R"(3.2   贾母又说：“请姑娘们来。今日有远客来，就不必上学去了。”众人答应了一声，便派了两个人去请。没多久，只见三个奶妈和五六个丫鬟，簇拥着三个姊妹来了。第一个乃二姐迎春----->（见右图），生的肌肤微丰，合中身材，腮凝新荔，鼻腻鹅脂，温柔沉默，观之可亲。happy第二个是三妹（见左图）<-----探春，削肩细腰，长挑身材，鸭蛋脸面，俊眼修眉，顾盼神飞，文彩精华，见之忘俗。（迎春外表就有懦小姐的感觉，探春外表就文采甚好，可起诗社。）)";
  string line2 =
      R"(前儿老太太（<cite unhidden>贾母</cite>）因要把<a title="梅翰林" href="a050.htm#P15L2"><i hidden>梅翰林</i><sub hidden>第50章15.2节:</sub>你妹妹（<cite unhidden>薛宝琴</cite>）说给宝玉</a>（<a unhidden title="说媒" href="original\c050.htm#P14L2"><i hidden>说媒</i><sub hidden>第50章14.2节:</sub>原文</a>），偏生（<cite unhidden>薛宝琴</cite>）又有了人家（<cite unhidden>梅翰林家</cite>），不然（<cite unhidden>宝琴宝玉他二人</cite>）倒是一门好亲。老太太离了鸳鸯，饭也吃不下去的，哪里就舍得了？（<cite unhidden>凤姐并不知道贾赦要鸳鸯是要<a unhidden title="作兴" href="a071.htm#P7L2"><i hidden>作兴</i><sub hidden>第71章7.2节:</sub>争宠之意</a>（<a unhidden title="作兴" href="original\c071.htm#P5L3"><i hidden>作兴</i><sub hidden>第71章5.3节:</sub>原文</a>）</cite>）（<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">这句又接到宝玉生日探春让李纨打黛玉，李纨说黛玉<a unhidden title="挨打" href="a063.htm#P13L3"><i hidden>挨打</i><sub hidden>第63章13.3节:</sub>人家不得贵婿反挨打</a>（<a unhidden title="挨打" href="original\c063.htm#P6L3"><i hidden>挨打</i><sub hidden>第63章6.3节:</sub>原文</a>），黛玉的婚姻是镜中花，他们二人的一个不能完成的愿望而已。</u>）)";
  string compareTo2 =
      R"(前儿老太太（贾母）因要把你妹妹（薛宝琴）说给宝玉（原文），偏生（薛宝琴）又有了人家（梅翰林家），不然（宝琴宝玉他二人）倒是一门好亲。老太太离了鸳鸯，饭也吃不下去的，哪里就舍得了？（凤姐并不知道贾赦要鸳鸯是要争宠之意（原文））（这句又接到宝玉生日探春让李纨打黛玉，李纨说黛玉人家不得贵婿反挨打（原文），黛玉的婚姻是镜中花，他们二人的一个不能完成的愿望而已。）)";
  string line3 =
      R"(<a unhidden id="P8L3">8.3</a>&nbsp;&nbsp; <strong unhidden>惯养娇生笑你痴，菱花空对雪澌澌。好防佳节元宵后，便是烟消火灭时。</strong>&nbsp;&nbsp;&nbsp;&nbsp;<samp unhidden font style="font-size: 13.5pt; font-family:楷体; color:#ff00ff">你这么痴心 娇生惯养她，实在是可笑。你知道她有多么生不逢时吗？<a title="挨打" href="a080.htm#P1L1"><i hidden>菱角菱花</i><sub hidden>第80章1.1节:</sub>菱角菱花皆盛于秋</a>（<a unhidden title="挨打" href="original\c080.htm#P1L3"><i hidden>菱角菱花</i><sub hidden>第80章1.3节:</sub>原文</a>），可却毫无办法要面对茫茫大雪。要小心元宵佳节一过，一切都会烟消云散（<a unhidden title="清净孤独" href="a022.htm#P13L4"><i hidden>清净孤独</i><sub hidden>第22章13.4节:</sub>不详灯谜</a>（<a unhidden title="清净孤独" href="original\c022.htm#P12L1"><i hidden>清净孤独</i><sub hidden>第22章12.1节:</sub>原文</a>）、<a unhidden title="进贡" href="a054.htm#P13L1"><i hidden>进贡</i><sub hidden>第54章13.1节:</sub>聋子放炮仗</a>（<a unhidden title="进贡" href="original\c054.htm#P8L2"><i hidden>进贡</i><sub hidden>第54章8.2节:</sub>原文</a>）、<a unhidden title="按爵" href="a058.htm#P1L2"><i hidden>按爵</i><sub hidden>第58章1.2节:</sub>老太妃薨毙</a>（<a unhidden title="按爵" href="original\c058.htm#P1L2"><i hidden>按爵</i><sub hidden>第58章1.2节:</sub>原文</a>））。</samp>)";
  string compareTo3 =
      R"(8.3   惯养娇生笑你痴，菱花空对雪澌澌。好防佳节元宵后，便是烟消火灭时。    你这么痴心 娇生惯养她，实在是可笑。你知道她有多么生不逢时吗？菱角菱花皆盛于秋（原文），可却毫无办法要面对茫茫大雪。要小心元宵佳节一过，一切都会烟消云散（不详灯谜（原文）、聋子放炮仗（原文）、老太妃薨毙（原文））。)";
  string line4 =
      R"(<a unhidden id="P1L5">1.5</a>&nbsp;&nbsp; 一天，神石正在独自嗟呀怀悼，突然看见一个僧人和一个道士远远走来。他们生得骨格不凡，丰神迥别（<cite unhidden>神石是神仙，所以看他们才骨骼不凡，否则也跟贾政眼里的<a unhidden title="见那和尚" href="a025.htm#P11L3"><i hidden>见那和尚</i><sub hidden>第25章11.3节:</sub>模样</a>（<a unhidden title="见那和尚" href="original\c025.htm#P9L4"><i hidden>见那和尚</i><sub hidden>第25章9.4节:</sub>原文</a>）一样了</cite>），说说笑笑走到青埂峰下，坐在神石边高谈快论。前儿老太太（<cite unhidden>贾母</cite>）因要把<a title="说媒" href="a050.htm#P15L2"><i hidden>说媒</i><sub hidden>第50章15.2节:</sub>你妹妹（<cite unhidden>薛宝琴</cite>）说给宝玉</a>（<a unhidden title="说媒" href="original\c050.htm#P14L2"><i hidden>说媒</i><sub hidden>第50章14.2节:</sub>原文</a>），偏生（<cite unhidden>薛宝琴</cite>）又有了人家（<cite unhidden>梅翰林家</cite>），不然（<cite unhidden>宝琴宝玉他二人</cite>）倒是一门好亲。)";
  string compareTo4 =
      R"(1.5   一天，神石正在独自嗟呀怀悼，突然看见一个僧人和一个道士远远走来。他们生得骨格不凡，丰神迥别（神石是神仙，所以看他们才骨骼不凡，否则也跟贾政眼里的模样（原文）一样了），说说笑笑走到青埂峰下，坐在神石边高谈快论。前儿老太太（贾母）因要把你妹妹（薛宝琴）说给宝玉（原文），偏生（薛宝琴）又有了人家（梅翰林家），不然（宝琴宝玉他二人）倒是一门好亲。)";
  string line = line1;
  string compareTo = compareTo1;

  LineNumber ln;
  ln.loadFirstFromContainedLine(line);
  if (ln.isParagraphHeader()) {
    return;
  }
  // after this, there could be only one line number at the beginning
  CoupledBodyTextWithLink bodyText;
  printCompareResult(bodyText.getDisplayString(line), compareTo);
}
