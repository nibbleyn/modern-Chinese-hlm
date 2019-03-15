#include "coupledBodyTextWithLink.hpp"

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
  for (const auto &type :
       {OBJECT_TYPE::LINENUMBER, OBJECT_TYPE::IMAGE, OBJECT_TYPE::IMAGEREF,
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

void CoupledBodyTextWithLink::render(bool hideParaHeader) {
  setInputOutputFiles();
  ifstream infile(m_inputFile);
  if (!infile) {
    cout << "file doesn't exist:" << m_inputFile << endl;
    return;
  }
  ofstream outfile(m_outputFile);
  string inLine{"not found"};
  int paraNo = 1;
  while (!infile.eof()) // To get all the lines.
  {
    getline(infile, inLine); // Saves the line in inLine.
    LineNumber ln;
    ln.loadFirstFromContainedLine(inLine);
    if (ln.isParagraphHeader()) {
      if (hideParaHeader == false) {
        string paraString = R"(第)" + TurnToString(paraNo) + R"(段)";
        if (paraNo == 1)
          paraString += R"( v向下    页面底部->||)";
        else
          paraString += R"( 向下    向上)";
        paraNo++;
        outfile << paraString << endl;
      }
      continue;
    }

    if (debug >= LOG_INFO) {
      cout << inLine << endl;
      cout << getDisplayString(inLine) << endl;
    }
    if (isLeadingBr(inLine)) {
      outfile << endl;
    } else if (not isEmptyLine(inLine)) {
      auto outputLine = getDisplayString(inLine);
      auto lastBr = outputLine.find(brTab);
      outfile << outputLine.substr(0, lastBr) << endl;
    }
  }
  if (hideParaHeader == false) {
    string paraString = R"(||<-页面顶部    ^向上)";
    outfile << paraString << endl;
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
      cout << endOfSubStringOffset << " " << offset - endOfSubStringOffset
           << " "
           << originalString.substr(endOfSubStringOffset,
                                    offset - endOfSubStringOffset)
           << endl;
      result += originalString.substr(endOfSubStringOffset,
                                      offset - endOfSubStringOffset);
      cout << result << "|8|" << endl;
      auto current = createObjectFromType(type, m_file);
      current->loadFirstFromContainedLine(originalString, offset);
      cout << "whole string: " << current->getWholeString() << endl;
      cout << "display as:" << current->getDisplayString() << "||" << endl;
      result += current->getDisplayString();
      cout << result << "|0|" << endl;
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
  string line5 =
      R"(<a unhidden id="P14L4">14.4</a>&nbsp;&nbsp; （<cite unhidden>贾琏</cite>）揭起衾单一看，只见这尤二姐面色如生，比活着还美貌。贾琏又搂着大哭，只叫“奶奶，你死的不明，都是我坑了你！”（<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">还是像当初<a unhidden title="提三说二" href="a065.htm#P1L2"><i hidden>提三说二</i><sub hidden>第65章1.2节:</sub>按大房</a>（<a unhidden title="提三说二" href="original\c065.htm#P1L4"><i hidden>提三说二</i><sub hidden>第65章1.4节:</sub>原文</a>）叫尤二姐“奶奶”</u>）贾蓉忙上来劝（<cite unhidden>贾琏</cite>）：“叔叔解着些儿（<cite unhidden>伤悲</cite>），（<cite unhidden>都是</cite>）我这个姨娘（<cite unhidden>尤二姐她</cite>）自己没福。”说着，又向南指（<cite unhidden>着梨香院和</cite>）<a title="群山" href="#P15L3"><i hidden>群山</i>大观园的界墙</a>，贾琏会意（<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">凤姐此时仍在偷听</u>），只悄悄跌脚说：“我忽略了（<cite unhidden>，只当凤姐她是好意对尤二姐</cite>），终久（<cite unhidden>把凤姐她的所作所为我都</cite>）对出（<cite unhidden>词</cite>）来，我替（<cite unhidden>尤二姐</cite>）你报仇。”（<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">贾琏、凤姐彻底<a unhidden title="连理" href="a064.htm#P19L3"><i hidden>连理</i><sub hidden>第64章19.3节:</sub>决裂</a>（<a unhidden title="连理" href="original\c064.htm#P11L3"><i hidden>连理</i><sub hidden>第64章11.3节:</sub>原文</a>）。</u>）)";
  string compareTo5 =
      R"(14.4   （贾琏）揭起衾单一看，只见这尤二姐面色如生，比活着还美貌。贾琏又搂着大哭，只叫“奶奶，你死的不明，都是我坑了你！”（还是像当初按大房（原文）叫尤二姐“奶奶”）贾蓉忙上来劝（贾琏）：“叔叔解着些儿（伤悲），（都是）我这个姨娘（尤二姐她）自己没福。”说着，又向南指（着梨香院和）大观园的界墙，贾琏会意（凤姐此时仍在偷听），只悄悄跌脚说：“我忽略了（，只当凤姐她是好意对尤二姐），终久（把凤姐她的所作所为我都）对出（词）来，我替（尤二姐）你报仇。”（贾琏、凤姐彻底决裂（原文）。）)";

  string line6 =
      R"(<a unhidden id="P7L2">7.2</a>&nbsp;&nbsp; <strong unhidden>西施一代倾城逐浪花，吴宫空自忆儿家。效颦莫笑东村女，头白溪边尚浣纱。</strong>&nbsp;&nbsp;&nbsp;&nbsp;<samp unhidden font style="font-size: 13.5pt; font-family:楷体; color:#ff00ff">日日在吴王夫差的宫殿里思念儿时的家园又如何，倾国倾城的一代佳人西施却再也没有回到当年浣纱的耶溪边，相反，如溪里那浪花互相追逐着奔向远方，她也消失不归。所以，也不必嘲笑效颦的东施，毕竟她相安无事地在溪边一直浣纱到白头。</samp>（<a unhidden href="attachment\b003_2.htm">西施</a>）（<a unhidden href="attachment\b064_7.htm">临江仙·滚滚长江东逝水</a>）（<a unhidden href="attachment\b064_8.htm">念奴娇·赤壁怀古</a>）（<a unhidden href="attachment\b064_9.htm">杜甫《登高》</a>）（<a unhidden href="attachment\b064_10.htm">王维《西施咏》</a>）（<a unhidden href="attachment\b064_11.htm">《庄子·天运》 东施效颦</a>）<br>)";
  string compareTo6 =
      R"(7.2   西施一代倾城逐浪花，吴宫空自忆儿家。效颦莫笑东村女，头白溪边尚浣纱。    日日在吴王夫差的宫殿里思念儿时的家园又如何，倾国倾城的一代佳人西施却再也没有回到当年浣纱的耶溪边，相反，如溪里那浪花互相追逐着奔向远方，她也消失不归。所以，也不必嘲笑效颦的东施，毕竟她相安无事地在溪边一直浣纱到白头。（西施）（临江仙·滚滚长江东逝水）（念奴娇·赤壁怀古）（杜甫《登高》）（王维《西施咏》）（《庄子·天运》 东施效颦）<br>)";
  string line7 =
      R"(<a unhidden id="P4L1">4.1</a>&nbsp;&nbsp; <strong unhidden>桃花帘外东风软，桃花帘内晨妆懒。帘外桃花帘内人，人与桃花隔不远。</strong>&nbsp;&nbsp;&nbsp;&nbsp;<samp unhidden font style="font-size: 13.5pt; font-family:楷体; color:#ff00ff">轻柔的春风又一次吹开了帘外的桃花，不禁让我回想起帘内的美人曾经情绪慵懒、无心梳妆。看似仅仅一帘之隔，帘外的桃花和帘内人隔得并不远。岂知隔花人远天涯近，这桃花和美人已经天各一方。</samp>（<u unhidden style="text-decoration-color: #F0BEC0;text-decoration-style: wavy;opacity: 0.4">“不远”对应“月痕”</u>） <br>)";
  string compareTo7 =
      R"(4.1   桃花帘外东风软，桃花帘内晨妆懒。帘外桃花帘内人，人与桃花隔不远。    轻柔的春风又一次吹开了帘外的桃花，不禁让我回想起帘内的美人曾经情绪慵懒、无心梳妆。看似仅仅一帘之隔，帘外的桃花和帘内人隔得并不远。岂知隔花人远天涯近，这桃花和美人已经天各一方。（“不远”对应“月痕”） <br>)";

  string line8 =
      R"(<a unhidden id="P11L2">11.2</a>&nbsp;&nbsp; <img unhidden src="pictures\szy.jpg" align="left" width="300"><strong unhidden>落霞与孤鹜齐飞，风急江天过雁哀，却是一只折足雁，叫的人九回肠，这是鸿雁来宾。</strong>&nbsp;&nbsp;&nbsp;&nbsp;<samp unhidden font style="font-size: 13.5pt; font-family:楷体; color:#ff00ff">水天一色，孤雁随着流动的落霞一起在江边飞翔。看它迎着江风急切切飞去，却一叫千回首。再看，它竟然是一只折足雁，再听，它的哀叫实在让人回肠九转，内心无比痛苦焦虑。话说季秋之月，鸿雁来宾，这是深秋将至，鸿雁南飞的季节，岂能留住它呢。</samp>（<a unhidden href="attachment\b062_1.htm">古文 王勃 《滕王阁序》</a>）（<a unhidden href="attachment\b062_2.htm">旧诗 哭刘司户二首</a>）（<a unhidden href="attachment\b062_3.htm">骨牌名</a><var unhidden font style="font-size: 13.5pt; font-family:楷体; color:#ff00ff">（见左图）<-----折足雁</var>）（<a unhidden href="attachment\b062_4.htm">曲牌名 九回肠</a>）（<a unhidden href="attachment\b062_5.htm">历书 鸿雁来宾</a>）)";
  string compareTo8 =
      R"(11.2   落霞与孤鹜齐飞，风急江天过雁哀，却是一只折足雁，叫的人九回肠，这是鸿雁来宾。    水天一色，孤雁随着流动的落霞一起在江边飞翔。看它迎着江风急切切飞去，却一叫千回首。再看，它竟然是一只折足雁，再听，它的哀叫实在让人回肠九转，内心无比痛苦焦虑。话说季秋之月，鸿雁来宾，这是深秋将至，鸿雁南飞的季节，岂能留住它呢。（古文 王勃 《滕王阁序》）（旧诗 哭刘司户二首）（骨牌名（见左图）<-----折足雁）（曲牌名 九回肠）（历书 鸿雁来宾）)";
  string line = line8;
  string compareTo = compareTo8;
  LineNumber ln;
  ln.loadFirstFromContainedLine(line);
  if (ln.isParagraphHeader()) {
    return;
  }
  // after this, there could be only one line number at the beginning
  CoupledBodyTextWithLink bodyText;
  bodyText.setFilePrefixFromFileType(FILE_TYPE::MAIN);
  bodyText.setFileAndAttachmentNumber("69");
  printCompareResult(bodyText.getDisplayString(line), compareTo);
}
