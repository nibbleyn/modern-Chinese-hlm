#include "coupledBodyTextWithLink.hpp"

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
      if (paraHeaderLoaded.isFirstParaHeader()) {
        m_numberOfFirstParaHeader++;
        if (isAutoNumbering()) {
          ParaHeaderInfo info{0, seqOfLines, 0};
          m_paraHeaderTable[seqOfLines] = info;
        }
      } else if (paraHeaderLoaded.isLastParaHeader()) {
        m_numberOfLastParaHeader++;
        m_lastSeqNumberOfLine = seqOfLines;
        break;
      } else if (paraHeaderLoaded.isMiddleParaHeader())
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

  size_t totalLines = 0;
  size_t paraNo = 1;
  size_t lastAdded = 0;
  for (const auto &element : m_lineAttrTable) {
    if (totalLines + element.second.numberOfLines > m_SizeOfReferPage) {
      METHOD_OUTPUT << "add para here" << endl;
      ParaHeaderInfo info{paraNo++, lastAdded, totalLines};
      m_paraHeaderTable[element.first] = info;
      lastAdded = element.first;
      totalLines = element.second.numberOfLines;
    } else {
      lastAdded = element.first;
      totalLines += element.second.numberOfLines;
    }
    METHOD_OUTPUT << totalLines << endl;
    METHOD_OUTPUT << element.first << "  " << element.second.numberOfLines
                  << "  " << getDisplayTypeString(element.second.type) << "  "
                  << element.second.cap << endl;
  }
  if (m_numberOfLastParaHeader != 0 and isAutoNumbering()) {
    ParaHeaderInfo info{paraNo, m_lastSeqNumberOfLine, 0};
    m_paraHeaderTable[m_lastSeqNumberOfLine] = info;
  }

  printParaHeaderTable();
}

void CoupledBodyTextWithLink::addLineNumber(const string &separatorColor,
                                            bool forceUpdate,
                                            bool hideParaHeader) {
  setInputOutputFiles();
  ifstream infile(m_inputFile);
  if (!infile) {
    METHOD_OUTPUT << "file doesn't exist:" << m_inputFile << endl;
    return;
  }
  ofstream outfile(m_outputFile);

  if (isAutoNumbering()) {
    calculateParaHeaderPositions();
  }

  size_t para = 0; // paragraph index
  ParaHeader paraHeader;
  int lineNo = 1; // LINE index within each group
  size_t seqOfLines = 0;
  bool enterLastPara = false;
  string inLine;
  while (!infile.eof()) {
    // Saves the line in inLine.
    getline(infile, inLine);
    // output directly if not in both tables
    auto inLineTable = isInLineAttrTable(seqOfLines);
    auto inParaTable = isInParaHeaderTable(seqOfLines);
    if (inLineTable == false and inParaTable == false)
      outfile << inLine << endl;
    else {
      if (inParaTable) {
        // first para Header
        if (m_paraHeaderTable[seqOfLines].seqOfParaHeader == 0) {
          paraHeader.loadFrom(inLine);
          paraHeader.fixFromTemplate();
          outfile << paraHeader.getFixedResult() << endl;
          para = 1;
          if (debug >= LOG_INFO) {
            METHOD_OUTPUT << "processed :" << endl;
            METHOD_OUTPUT << inLine << endl;
          }
        } else if (para == m_paraHeaderTable.size() - 1) {
          paraHeader.markAsLastParaHeader();
          paraHeader.fixFromTemplate();
          outfile << paraHeader.getFixedResult() << endl;
          if (debug >= LOG_INFO) {
            METHOD_OUTPUT << "processed :" << endl;
            METHOD_OUTPUT << inLine << endl;
          }
          break;
        }
      }
      // needs numbering
      if (inLineTable and
          m_lineAttrTable[seqOfLines].type == DISPLY_LINE_TYPE::TEXT) {
        LineNumber ln;
        ln.loadFirstFromContainedLine(inLine);

        LineNumber newLn(para, lineNo);
        if (forceUpdate or not ln.equal(newLn)) {
          if (ln.valid()) // remove old line number
          {
            removeOldLineNumber(inLine);
          }
          removeNbspsAndSpaces(inLine);
          outfile << newLn.getWholeString() << doubleSpace << displaySpace
                  << inLine << endl; // Prints our line
        } else
          outfile << inLine << endl;
        lineNo++;
        if (debug >= LOG_INFO) {
          METHOD_OUTPUT << "processed :" << endl;
          METHOD_OUTPUT << inLine << endl;
        }
        // needs to append para header afterwards
        if (inParaTable) {
          enterLastPara = (para == m_paraHeaderTable.size() - 2);
          paraHeader.m_currentParaNo = para++;
          paraHeader.m_lastPara = enterLastPara;
          paraHeader.markAsMiddleParaHeader();
          paraHeader.fixFromTemplate();
          outfile << paraHeader.getFixedResult() << endl;
          if (not enterLastPara) {
            lineNo = 1; // LINE index within each group
          }
          if (debug >= LOG_INFO) {
            METHOD_OUTPUT << "para header added :" << endl;
            METHOD_OUTPUT << paraHeader.getFixedResult() << endl;
          }
        }
      }
    }
    seqOfLines++;
  }
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << "numbering finished." << endl;
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
