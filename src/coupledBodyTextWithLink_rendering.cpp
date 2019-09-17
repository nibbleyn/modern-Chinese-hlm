#include "coupledBodyTextWithLink.hpp"

ObjectPtr createObjectFromType(string type,
                               const string &fromFile = emptyString) {
  if (type == nameOfLineNumberType)
    return make_unique<LineNumberPlaceholderLink>();
  else if (type == nameOfSpaceType)
    return make_unique<Space>();
  else if (type == nameOfPoemType)
    return make_unique<Poem>();
  else if (type == nameOfLinkFromMainType)
    return make_unique<LinkFromMain>(fromFile);
  else if (type == nameOfLinkFromAttachmentType)
    return make_unique<LinkFromAttachment>(fromFile);
  else if (type == nameOfPersonalCommentType)
    return make_unique<PersonalComment>(fromFile);
  else if (type == nameOfPoemTranslationType)
    return make_unique<PoemTranslation>(fromFile);
  else if (type == nameOfCommentType)
    return make_unique<Comment>(fromFile);
  else if (type == nameOfCitationType)
    return make_unique<Citation>();
  return nullptr;
}

string getStartTagOfObjectType(string type) {
  auto ptr = createObjectFromType(type);
  if (ptr != nullptr)
    return ptr->getStartTag();
  else
    return emptyString;
}

string getEndTagOfObjectType(string type) {
  auto ptr = createObjectFromType(type);
  if (ptr != nullptr)
    return ptr->getEndTag();
  else
    return emptyString;
}

bool CoupledBodyTextWithLink::isEmbeddedObject(string type, size_t offset) {
  if (type == nameOfLinkFromMainType) {
    try {
      auto linkInfo = m_linkStringTable.at(offset);
      return linkInfo.embedded;
    } catch (exception &) {
      METHOD_OUTPUT << "no such object " << nameOfLinkFromMainType << "at "
                    << offset;
    }
  }
  if (type == nameOfCommentType) {
    try {
      auto commentInfo = m_commentStringTable.at(offset);
      return commentInfo.embedded;
    } catch (exception &) {
      METHOD_OUTPUT << "no such object " << nameOfCommentType << "at "
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
  LineNumberPlaceholderLink ln;
  auto offset = ln.loadFirstFromContainedLine(containedLine);
  if (offset != string::npos) {
    m_foundTypes[nameOfLineNumberType] = offset;
    m_offsetOfTypes[offset] = nameOfLineNumberType;
  }
  for (const auto &type : {nameOfSpaceType, nameOfPoemType}) {
    auto offset = containedLine.find(getStartTagOfObjectType(type));
    if (offset != string::npos) {
      m_foundTypes[type] = offset;
      m_offsetOfTypes[offset] = type;
    }
  }
  bool firstOccurence = true;
  offset = string::npos;
  do {
    offset =
        containedLine.find(getStartTagOfObjectType(nameOfPersonalCommentType),
                           (firstOccurence) ? 0 : offset + 1);
    if (offset == string::npos)
      break;
    m_personalCommentStringTable[offset] = containedLine.find(
        getEndTagOfObjectType(nameOfPersonalCommentType), offset);
    if (firstOccurence == true) {
      m_foundTypes[nameOfPersonalCommentType] = offset;
      m_offsetOfTypes[offset] = nameOfPersonalCommentType;
      firstOccurence = false;
    }
  } while (true);

  firstOccurence = true;
  offset = string::npos;
  do {
    offset =
        containedLine.find(getStartTagOfObjectType(nameOfPoemTranslationType),
                           (firstOccurence) ? 0 : offset + 1);
    if (offset == string::npos)
      break;
    m_poemTranslationStringTable[offset] = containedLine.find(
        getEndTagOfObjectType(nameOfPoemTranslationType), offset);
    if (firstOccurence == true) {
      m_foundTypes[nameOfPoemTranslationType] = offset;
      m_offsetOfTypes[offset] = nameOfPoemTranslationType;
      firstOccurence = false;
    }
  } while (true);

  firstOccurence = true;
  offset = string::npos;
  do {
    offset = containedLine.find(getStartTagOfObjectType(nameOfCommentType),
                                (firstOccurence) ? 0 : offset + 1);
    if (offset == string::npos)
      break;
    CommentStringInfo info{
        offset,
        containedLine.find(getEndTagOfObjectType(nameOfCommentType), offset),
        false};
    m_commentStringTable[offset] = info;
    if (firstOccurence == true) {
      m_foundTypes[nameOfCommentType] = offset;
      m_offsetOfTypes[offset] = nameOfCommentType;
      firstOccurence = false;
    }
  } while (true);

  offset = string::npos;
  try {
    auto type = m_offsetOfTypes.at(0);
    // skip first line number as a link actually
    if (type == nameOfLineNumberType)
      offset = containedLine.find(
          getStartTagOfObjectType(nameOfLinkFromMainType), 1);
  } catch (exception &) {
    offset =
        containedLine.find(getStartTagOfObjectType(nameOfLinkFromMainType), 0);
  }
  if (offset != string::npos) {
    m_foundTypes[nameOfLinkFromMainType] = offset;
    m_offsetOfTypes[offset] = nameOfLinkFromMainType;
    LinkStringInfo info{
        offset,
        containedLine.find(getEndTagOfObjectType(nameOfLinkFromMainType),
                           offset),
        false};
    m_linkStringTable[offset] = info;
    do {
      offset = containedLine.find(
          getStartTagOfObjectType(nameOfLinkFromMainType), offset + 1);
      if (offset == string::npos)
        break;
      LinkStringInfo info{
          offset,
          containedLine.find(getEndTagOfObjectType(nameOfLinkFromMainType),
                             offset),
          false};
      m_linkStringTable[offset] = info;
    } while (true);
  }
  searchForEmbededLinks();
  if (debug >= LOG_INFO) {
    printOffsetToObjectType();
    printLinkStringTable();
    printCommentStringTable();
    printPersonalCommentStringTable();
    printPoemTranslationStringTable();
  }
}

void CoupledBodyTextWithLink::getDisplayString(const string &originalString,
                                               bool onlyTypes) {
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << originalString.length() << endl;

  if (onlyTypes)
    m_resultSet.clear();
  else
    m_resultDisplayString.clear();

  scanForTypes(originalString);

  unsigned int endOfSubStringOffset = 0;
  bool pureTextFound = false;
  do {
    if (m_offsetOfTypes.empty())
      break;
    auto first = m_offsetOfTypes.begin();
    auto type = first->second;
    auto offset = first->first;
    if (not isEmbeddedObject(type, offset)) {
      if (debug >= LOG_INFO)
        METHOD_OUTPUT << endOfSubStringOffset << displaySpace
                      << offset - endOfSubStringOffset << displaySpace
                      << originalString.substr(endOfSubStringOffset,
                                               offset - endOfSubStringOffset)
                      << endl;
      auto current = createObjectFromType(type, m_file);
      if (onlyTypes) {
        string text = originalString.substr(endOfSubStringOffset,
                                            offset - endOfSubStringOffset);
        if (debug >= LOG_INFO)
          METHOD_OUTPUT << text << endl;
        if (offset > endOfSubStringOffset and not isMixedOfSpaceBrackets(text))
          pureTextFound = true;
        m_resultSet.insert(type);
        current->loadFirstFromContainedLine(originalString, offset);
      } else {
        m_resultDisplayString += originalString.substr(
            endOfSubStringOffset, offset - endOfSubStringOffset);
        if (debug >= LOG_INFO)
          METHOD_OUTPUT << m_resultDisplayString << "|8|" << endl;
        current->shouldHideSubObject(m_hiddenSet);
        current->loadFirstFromContainedLine(originalString, offset);
        if (current->shouldBeHidden(m_hiddenSet)) {
          current->hide();
        } else {
          m_resultDisplayString += current->getDisplayString();
        }
        if (debug >= LOG_INFO)
          METHOD_OUTPUT << m_resultDisplayString << "|0|" << endl;
      }

      // should add length of substring above loadFirstFromContainedLine gets
      // so require the string be fixed before
      endOfSubStringOffset = offset + current->length();
      if (debug >= LOG_INFO) {
        METHOD_OUTPUT << "whole string: " << current->getFormatedFullString()
                      << endl;
        METHOD_OUTPUT << "display as:" << current->getDisplayString() << "||"
                      << endl;
        METHOD_OUTPUT << current->length() << displaySpace
                      << endOfSubStringOffset << endl;
      }
    }
    m_offsetOfTypes.erase(first);
    // never try to find another LINENUMBER
    if (type != nameOfLineNumberType) {
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
      if (debug >= LOG_INFO)
        printOffsetToObjectType();
    }
  } while (true);
  if (onlyTypes) {
    string lastPart = originalString.substr(endOfSubStringOffset);
    if (debug >= LOG_INFO)
      METHOD_OUTPUT << lastPart << endl;
    if (lastPart != emptyString and not isMixedOfSpaceBrackets(lastPart))
      pureTextFound = true;
    if (pureTextFound)
      m_resultSet.insert(nameOfTextType);
  } else if (endOfSubStringOffset < originalString.length())
    m_resultDisplayString += originalString.substr(endOfSubStringOffset);

  m_foundTypes.clear();
  m_offsetOfTypes.clear();
  m_linkStringTable.clear();
  m_commentStringTable.clear();
  m_personalCommentStringTable.clear();
  m_poemTranslationStringTable.clear();
}

string CoupledBodyText::postProcessLine(const string &originalString) {
  string line = originalString;
  string pattern = bracketStartChars + "|" + bracketEndChars + "|" + upArrow;
  std::regex toDelete(pattern);
  line = regex_replace(line, toDelete, emptyString);
  std::regex toReplace(specialDisplayPrefixForCoupledLink +
                       specialDisplayPrefixForCoupledLink);
  line = regex_replace(line, toReplace, specialDisplayPrefixForCoupledLink);
  toReplace =
      specialDisplayPostfixForCoupledLink + specialDisplayPostfixForCoupledLink;
  line = regex_replace(line, toReplace, specialDisplayPostfixForCoupledLink);
  return line;
}

void CoupledBodyTextWithLink::render(bool removeLinkToOriginalAndAttachment) {
  ifstream infile(m_inputFile);
  if (not fileExist(infile, m_inputFile))
    return;
  ofstream outfile(m_outputFile);
  string inLine{"not found"};
  CoupledParaHeader paraHeaderLoaded;
  while (!infile.eof()) {
    getline(infile, inLine);
    if (debug >= LOG_INFO) {
      METHOD_OUTPUT << inLine << endl;
    }
    LineNumberPlaceholderLink ln;
    ln.loadFirstFromContainedLine(inLine);
    if (ln.isPartOfParagraphHeader()) {
      paraHeaderLoaded.loadFrom(inLine);
      paraHeaderLoaded.fixFromTemplate();
      if (debug >= LOG_INFO) {
        METHOD_OUTPUT << paraHeaderLoaded.getDisplayString() << endl;
      }
      if (not m_hideParaHeader)
        outfile << paraHeaderLoaded.getDisplayString() << endl;
    } else if (isLeadingBr(inLine) or isImageGroupLine(inLine)) {
      outfile << LF;
    } else if (not isEmptyLine(inLine)) {
      getDisplayString(inLine);
      auto outputLine = getResultDisplayString();
      auto lastBr = outputLine.find(brTab);
      if (debug >= LOG_INFO) {
        METHOD_OUTPUT << outputLine.substr(0, lastBr) << endl;
      }
      if (m_postProcessLine)
        outputLine = postProcessLine(outputLine.substr(0, lastBr));
      outfile << outputLine << endl;
    }
  }
}

// must be called after called
// CoupledBodyTextContainer::refAttachmentTable.loadReferenceAttachmentList();
void CoupledBodyTextWithLink::removePersonalCommentsOverNumberedFiles() {
  setInputOutputFiles();
  ifstream infile(m_inputFile);
  if (not fileExist(infile, m_inputFile))
    return;
  ofstream outfile(m_outputFile);
  string inLine{"not found"};
  while (!infile.eof()) {
    getline(infile, inLine);
    if (debug >= LOG_INFO)
      METHOD_OUTPUT << inLine << endl;
    // inLine would change in loop below
    auto orgLine = inLine;
    string start = personalCommentStartChars;
    string end = personalCommentEndChars;
    string to_replace = emptyString;
    // first loop to remove all personal Comments
    auto removePersonalCommentLine = orgLine;
    auto personalCommentBegin = removePersonalCommentLine.find(start);
    while (personalCommentBegin != string::npos) {
      auto personalCommentEnd = removePersonalCommentLine.find(end);
      string personalComment = removePersonalCommentLine.substr(
          personalCommentBegin,
          personalCommentEnd + end.length() - personalCommentBegin);
      to_replace = personalComment;
      auto replaceBegin = orgLine.find(to_replace);
      orgLine.replace(replaceBegin, to_replace.length(), emptyString);
      removePersonalCommentLine =
          removePersonalCommentLine.substr(personalCommentEnd + end.length());
      // find next personalComment in the removePersonalCommentLine
      personalCommentBegin = removePersonalCommentLine.find(start);
    }
    // the second loop to remove all expected attachment link from result
    // orgLine
    auto removeSpecialLinkLine = orgLine;
    auto specialLinkBegin = removeSpecialLinkLine.find(linkStartChars);
    while (specialLinkBegin != string::npos) {
      auto specialLinkEnd = removeSpecialLinkLine.find(linkEndChars);
      string specialLink =
          getIncludedStringBetweenTags(removeSpecialLinkLine, linkStartChars,
                                       linkEndChars, specialLinkBegin);
      LinkFromAttachment m_linkPtr(m_file, specialLink);
      auto num = make_pair(m_linkPtr.getchapterNumer(),
                           m_linkPtr.getattachmentNumber());
      if (m_linkPtr.isTargetToOtherAttachmentHtm() and
          LinkFromMain::attachmentTable.getAttachmentType(num) ==
              ATTACHMENT_TYPE::PERSONAL) {
        if (debug >= LOG_INFO)
          METHOD_OUTPUT << specialLink << endl;
        to_replace = specialLink;
        auto replaceBegin = orgLine.find(to_replace);
        orgLine.replace(replaceBegin, to_replace.length(), emptyString);
      }
      removeSpecialLinkLine =
          removeSpecialLinkLine.substr(specialLinkEnd + linkEndChars.length());
      // find next specialLink in the removeSpecialLinkLine
      specialLinkBegin = removeSpecialLinkLine.find(linkStartChars);
    }

    outfile << orgLine << endl;
  }
}
