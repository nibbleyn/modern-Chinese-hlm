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
