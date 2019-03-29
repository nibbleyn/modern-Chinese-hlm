#include "coupledBodyTextWithLink.hpp"

using ObjectPtr = std::unique_ptr<Object>;

string getDisplayTypeString(DISPLY_LINE_TYPE type) {
  if (type == DISPLY_LINE_TYPE::EMPTY)
    return "empty";
  if (type == DISPLY_LINE_TYPE::PARA)
    return "para";
  if (type == DISPLY_LINE_TYPE::TEXT)
    return "text";
  if (type == DISPLY_LINE_TYPE::IMAGE)
    return "image";
  return "bad";
}

ObjectPtr createObjectFromType(OBJECT_TYPE type, const string &fromFile) {
  if (type == OBJECT_TYPE::LINENUMBER)
    return std::make_unique<LineNumber>();
  else if (type == OBJECT_TYPE::SPACE)
    return std::make_unique<Space>();
  else if (type == OBJECT_TYPE::POEM)
    return std::make_unique<Poem>();
  else if (type == OBJECT_TYPE::LINKFROMMAIN)
    return std::make_unique<LinkFromMain>(fromFile);
  else if (type == OBJECT_TYPE::PERSONALCOMMENT)
    return std::make_unique<PersonalComment>(fromFile);
  else if (type == OBJECT_TYPE::POEMTRANSLATION)
    return std::make_unique<PoemTranslation>(fromFile);
  else if (type == OBJECT_TYPE::COMMENT)
    return std::make_unique<Comment>(fromFile);
  return nullptr;
}

string getStartTagOfObjectType(OBJECT_TYPE type) {
  if (type == OBJECT_TYPE::LINENUMBER)
    return UnhiddenLineNumberStart;
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
  else if (type == OBJECT_TYPE::COMMENT)
    return commentBeginChars;
  return "";
}

string getEndTagOfObjectType(OBJECT_TYPE type) {
  if (type == OBJECT_TYPE::LINKFROMMAIN)
    return linkEndChars;
  else if (type == OBJECT_TYPE::PERSONALCOMMENT)
    return personalCommentEndChars;
  else if (type == OBJECT_TYPE::POEMTRANSLATION)
    return poemTranslationEndChars;
  else if (type == OBJECT_TYPE::COMMENT)
    return commentEndChars;
  return "";
}

bool CoupledBodyTextWithLink::isEmbeddedObject(OBJECT_TYPE type,
                                               size_t offset) {
  if (type == OBJECT_TYPE::LINKFROMMAIN) {
    try {
      auto linkInfo = m_linkStringTable.at(offset);
      return linkInfo.embedded;
    } catch (exception &) {
      METHOD_OUTPUT << "no such object " << getNameOfObjectType(type) << "at "
                    << offset;
    }
  }
  if (type == OBJECT_TYPE::COMMENT) {
    try {
      auto commentInfo = m_commentStringTable.at(offset);
      return commentInfo.embedded;
    } catch (exception &) {
      METHOD_OUTPUT << "no such object " << getNameOfObjectType(type) << "at "
                    << offset;
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
       {OBJECT_TYPE::LINENUMBER, OBJECT_TYPE::SPACE, OBJECT_TYPE::POEM}) {
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
    METHOD_OUTPUT << "file doesn't exist:" << m_inputFile << endl;
    return;
  }
  ofstream outfile(m_outputFile);
  string inLine{"not found"};
  ParaHeader paraHeaderLoaded;
  while (!infile.eof()) // To get all the lines.
  {
    getline(infile, inLine); // Saves the line in inLine.
    if (debug >= LOG_INFO) {
      METHOD_OUTPUT << inLine << endl;
    }
    LineNumber ln;
    ln.loadFirstFromContainedLine(inLine);
    if (ln.isParagraphHeader() and hideParaHeader == false) {
      paraHeaderLoaded.loadFrom(inLine);
      paraHeaderLoaded.fixFromTemplate();
      if (debug >= LOG_INFO) {
        METHOD_OUTPUT << paraHeaderLoaded.getDisplayString() << endl;
      }
      outfile << paraHeaderLoaded.getDisplayString() << endl;
    } else if (isLeadingBr(inLine)) {
      string LF{0x0A};
      outfile << LF;
    } else if (not isEmptyLine(inLine)) {
      auto outputLine = getDisplayString(inLine);
      auto lastBr = outputLine.find(brTab);
      if (debug >= LOG_INFO) {
        METHOD_OUTPUT << outputLine.substr(0, lastBr) << endl;
      }
      outfile << outputLine.substr(0, lastBr) << endl;
    }
  }
}

void CoupledBodyTextWithLink::addLineNumber(const string &separatorColor,
                                            bool forceUpdate,
                                            bool hideParaHeader) {}

void CoupledBodyTextWithLink::calculateParaHeaderPositions() {
  setInputOutputFiles();
  ifstream infile(m_inputFile);
  if (!infile) {
    METHOD_OUTPUT << "file doesn't exist:" << m_inputFile << endl;
    return;
  }

  scanLines(); // first scan

  if (m_numberOfFirstParaHeader == 0 or m_numberOfLastParaHeader == 0) {
    METHOD_OUTPUT << "no top or bottom paragraph found:" << m_inputFile << endl;
    return;
    LineNumber::setStartNumber(START_PARA_NUMBER);
  }

  auto totalLines = 0;
  for (const auto &element : m_lineAttrTable) {
    if (totalLines + element.second.numberOfLines > m_SizeOfReferPage) {
      METHOD_OUTPUT << "add para here" << endl;
      totalLines = element.second.numberOfLines;
    } else
      totalLines += element.second.numberOfLines;
    METHOD_OUTPUT << totalLines << endl;
    METHOD_OUTPUT << element.first << "  " << element.second.numberOfLines
                  << "  " << getDisplayTypeString(element.second.type) << "  "
                  << element.second.cap << endl;
  }
}

/**
 * only based on length of string. So font must be same for all characters
 */
size_t
CoupledBodyTextWithLink::getLinesOfDisplayText(const string &dispString) {
  if (m_averageSizeOfOneLine == 0) {
    METHOD_OUTPUT
        << "getAverageLineLengthFromReferenceFile must be called before."
        << endl;
    return 0;
  }
  auto totalLines = 1;
  int size = utf8length(dispString);
  if (size != 0) {
    size_t end = -1;
    totalLines = 0;
    bool firstline = true;
    while (size > 0) {
      // should be +2 and 0 if more accurate
      // but to be conservative here
      // avoid to return lines less than really displayed
      auto cutSize = (firstline) ? (m_averageSizeOfOneLine + 1)
                                 : (m_averageSizeOfOneLine - 1);
      auto cutLine = utf8substr(dispString, end + 1, end, cutSize);
      size -= cutSize;
      if (firstline)
        firstline = false;
      if (debug >= LOG_INFO) {
        METHOD_OUTPUT << cutLine << endl;
        METHOD_OUTPUT << utf8length(cutLine) << endl;
        METHOD_OUTPUT << size << endl;
      }
      totalLines++;
      if (debug >= LOG_INFO) {
        METHOD_OUTPUT << totalLines << endl;
      }
    }
  }
  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << totalLines << endl;
  }
  return totalLines;
}

size_t CoupledBodyTextWithLink::getAverageLineLengthFromReferenceFile() {
  ifstream referLinesFile(REFERENCE_LINES);
  if (!referLinesFile) // doesn't exist
  {
    METHOD_OUTPUT << "referLines file doesn't exist:" << REFERENCE_LINES
                  << endl;
    return 0;
  }

  auto totalSizes = 0;
  auto totalLines = 0;

  while (!referLinesFile.eof()) // To get you all the lines.
  {
    string line{""};
    getline(referLinesFile, line);
    line = std::regex_replace(line, std::regex("(?:\\r\\n|\\n|\\r)"), "");
    if (debug >= LOG_INFO) {
      METHOD_OUTPUT << line << endl;
      METHOD_OUTPUT << utf8length(line) << endl;
    }
    if (not line.empty()) {
      totalSizes += utf8length(line);
      totalLines++;
    }
  }
  return totalSizes / totalLines;
}

size_t CoupledBodyTextWithLink::getLinesofReferencePage() {
  m_averageSizeOfOneLine = getAverageLineLengthFromReferenceFile();
  ifstream infile(REFERENCE_PAGE);
  if (!infile) {
    METHOD_OUTPUT << "file doesn't exist:" << REFERENCE_PAGE << endl;
    return 0;
  }
  auto totalLines = 0;
  string line;
  while (!infile.eof()) // To get you all the lines.
  {
    getline(infile, line);
    totalLines += getLinesOfDisplayText(line);
    if (debug >= LOG_INFO) {
      METHOD_OUTPUT << line << endl; // excluding start line
      METHOD_OUTPUT << totalLines << endl;
    }
  }
  return totalLines;
}

void CoupledBodyTextWithLink::scanLines() {
  m_numberOfFirstParaHeader = 0;
  m_numberOfMiddleParaHeader = 0;
  m_numberOfLastParaHeader = 0;
  m_numberOfImageGroupNotIncludedInPara = 0;
  m_lineAttrTable.clear();

  m_SizeOfReferPage = getLinesofReferencePage();
  setInputOutputFiles();
  ifstream infile(m_inputFile);

  if (!infile) {
    METHOD_OUTPUT << "file doesn't exist:" << m_inputFile << endl;
    return;
  }

  string orgLine;
  size_t seqOfLines = 0;
  ParaHeader paraHeaderLoaded;
  string dispLine{""};
  while (!infile.eof()) // To get you all the lines.
  {
    getline(infile, orgLine);
    if (debug >= LOG_INFO) {
      METHOD_OUTPUT << orgLine << endl;
    }
    LineNumber ln;
    ln.loadFirstFromContainedLine(orgLine);
    if (ln.isParagraphHeader()) {
      paraHeaderLoaded.loadFrom(orgLine);
      if (paraHeaderLoaded.isFirstParaHeader())
        m_numberOfFirstParaHeader++;
      if (paraHeaderLoaded.isLastParaHeader())
        m_numberOfLastParaHeader++;
      if (paraHeaderLoaded.isMiddleParaHeader())
        m_numberOfMiddleParaHeader++;
      paraHeaderLoaded.fixFromTemplate();
      dispLine = paraHeaderLoaded.getDisplayString();
      if (debug >= LOG_INFO) {
        METHOD_OUTPUT << dispLine << endl;
      }
      size_t end = -1;
      LineInfo info{0, DISPLY_LINE_TYPE::PARA, utf8substr(dispLine, 0, end, 5)};
      m_lineAttrTable[seqOfLines] = info;
    } else if (isLeadingBr(orgLine)) {
      LineInfo info{1, DISPLY_LINE_TYPE::EMPTY, "    "};
      m_lineAttrTable[seqOfLines] = info;
    } else if (isImageGroupLine(orgLine)) {
      LineInfo info{m_SizeOfReferPage - 1, DISPLY_LINE_TYPE::IMAGE, "image"};
      m_lineAttrTable[seqOfLines] = info;
    } else if (not isEmptyLine(orgLine)) {
      auto lastBr = orgLine.find(brTab);
      if (debug >= LOG_INFO) {
        METHOD_OUTPUT << orgLine.substr(0, lastBr) << endl;
      }
      dispLine = getDisplayString(orgLine.substr(0, lastBr));
      if (debug >= LOG_INFO) {
        METHOD_OUTPUT << dispLine << endl; // excluding start line
        METHOD_OUTPUT << utf8length(dispLine) << endl;
        METHOD_OUTPUT << getLinesOfDisplayText(dispLine) << endl;
      }
      size_t end = -1;
      LineInfo info{getLinesOfDisplayText(dispLine), DISPLY_LINE_TYPE::TEXT,
                    utf8substr(dispLine, 0, end, 5)};
      m_lineAttrTable[seqOfLines] = info;
      // try to merge leading BR of this line
      try {
        auto previousLine = m_lineAttrTable.at(seqOfLines - 1);
        if (previousLine.type == DISPLY_LINE_TYPE::EMPTY)
          m_lineAttrTable.erase(seqOfLines - 1);
        m_lineAttrTable[seqOfLines].numberOfLines++;
      } catch (exception &) {
        METHOD_OUTPUT << "no leading br at line" << seqOfLines - 1 << endl;
      }
    }
    seqOfLines++;
  }
  printLineAttrTable();

  m_lastSeqNumberOfLine = seqOfLines;
  using ToDeleteLines = std::set<size_t>;
  ToDeleteLines lineSet;

  bool deleteBR = false;
  size_t BRDeleted = 0;
  size_t lastDeleted = 0;
  for (const auto &element : m_lineAttrTable) {
    if (deleteBR == true) {
      if (element.second.type == DISPLY_LINE_TYPE::EMPTY) {
        lastDeleted = element.first;
        lineSet.insert(lastDeleted);
        BRDeleted++;
      } else {
        deleteBR = false;
        if (BRDeleted > 0) {
          lineSet.erase(lastDeleted);
        }
      }
    }
    if (deleteBR == false and element.second.type == DISPLY_LINE_TYPE::IMAGE) {
      deleteBR = true;
      BRDeleted = 0;
    }
  }
  for (const auto &element : lineSet) {
    METHOD_OUTPUT << element << endl;
    m_lineAttrTable.erase(element);
  }

  if (debug >= LOG_INFO) {
    METHOD_OUTPUT << endl;
    METHOD_OUTPUT << "Result of getNumberOfPara:" << endl;
    printLineAttrTable();
    METHOD_OUTPUT << "m_numberOfFirstParaHeader: " << m_numberOfFirstParaHeader
                  << endl;
    METHOD_OUTPUT << "m_numberOfMiddleParaHeader: "
                  << m_numberOfMiddleParaHeader << endl;
    METHOD_OUTPUT << "m_numberOfLastParaHeader: " << m_numberOfLastParaHeader
                  << endl;
    METHOD_OUTPUT << "m_numberOfImageGroupNotIncludedInPara: "
                  << m_numberOfImageGroupNotIncludedInPara << endl;
    METHOD_OUTPUT << "m_lastSeqNumberOfLine: " << m_lastSeqNumberOfLine << endl;
  }
}

void CoupledBodyTextWithLink::printStringInLines() {
  m_SizeOfReferPage = getLinesofReferencePage();
  setInputOutputFiles();
  //  ifstream infile(m_inputFile);
  ifstream infile(TO_CHECK_FILE);
  if (!infile) {
    METHOD_OUTPUT << "file doesn't exist:" << m_inputFile << endl;
    return;
  }

  string line;
  while (!infile.eof()) // To get you all the lines.
  {
    getline(infile, line);
    if (debug >= LOG_INFO) {
      METHOD_OUTPUT << line << endl; // excluding start line
      METHOD_OUTPUT << utf8length(line) << endl;
      METHOD_OUTPUT << getLinesOfDisplayText(line) << endl;
    }
  }
}

string CoupledBodyTextWithLink::getDisplayString(const string &originalString) {
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << originalString.length() << endl;
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
      if (debug >= LOG_INFO)
        METHOD_OUTPUT << endOfSubStringOffset << " "
                      << offset - endOfSubStringOffset << " "
                      << originalString.substr(endOfSubStringOffset,
                                               offset - endOfSubStringOffset)
                      << endl;
      result += originalString.substr(endOfSubStringOffset,
                                      offset - endOfSubStringOffset);
      if (debug >= LOG_INFO)
        METHOD_OUTPUT << result << "|8|" << endl;
      auto current = createObjectFromType(type, m_file);
      current->loadFirstFromContainedLine(originalString, offset);
      METHOD_OUTPUT << "whole string: " << current->getWholeString() << endl;
      METHOD_OUTPUT << "display as:" << current->getDisplayString() << "||"
                    << endl;
      result += current->getDisplayString();
      if (debug >= LOG_INFO)
        METHOD_OUTPUT << result << "|0|" << endl;
      // should add length of substring above loadFirstFromContainedLine gets
      // so require the string be fixed before
      endOfSubStringOffset = offset + current->length();
      if (debug >= LOG_INFO)
        METHOD_OUTPUT << current->length() << " " << endOfSubStringOffset
                      << endl;
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
    METHOD_OUTPUT << "file doesn't exist:" << m_inputFile << endl;
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
      METHOD_OUTPUT << inLine << endl;
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
