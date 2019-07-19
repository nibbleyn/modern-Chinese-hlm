#include "coupledBodyTextWithLink.hpp"

bool CoupledBodyTextWithLink::isEmbeddedObject(Object::OBJECT_TYPE type,
                                               size_t offset) {
  if (type == Object::OBJECT_TYPE::LINKFROMMAIN) {
    try {
      auto linkInfo = m_linkStringTable.at(offset);
      return linkInfo.embedded;
    } catch (exception &) {
      METHOD_OUTPUT << "no such object " << Object::getNameOfObjectType(type)
                    << "at " << offset;
    }
  }
  if (type == Object::OBJECT_TYPE::COMMENT) {
    try {
      auto commentInfo = m_commentStringTable.at(offset);
      return commentInfo.embedded;
    } catch (exception &) {
      METHOD_OUTPUT << "no such object " << Object::getNameOfObjectType(type)
                    << "at " << offset;
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
    m_foundTypes[Object::OBJECT_TYPE::LINENUMBER] = offset;
    m_offsetOfTypes[offset] = Object::OBJECT_TYPE::LINENUMBER;
  }
  for (const auto &type :
       {Object::OBJECT_TYPE::SPACE, Object::OBJECT_TYPE::POEM}) {
    auto offset = containedLine.find(Object::getStartTagOfObjectType(type));
    if (offset != string::npos) {
      m_foundTypes[type] = offset;
      m_offsetOfTypes[offset] = type;
    }
  }
  bool firstOccurence = true;
  offset = string::npos;
  do {
    offset = containedLine.find(
        Object::getStartTagOfObjectType(Object::OBJECT_TYPE::PERSONALCOMMENT),
        (firstOccurence) ? 0 : offset + 1);
    if (offset == string::npos)
      break;
    m_personalCommentStringTable[offset] = containedLine.find(
        Object::getEndTagOfObjectType(Object::OBJECT_TYPE::PERSONALCOMMENT),
        offset);
    if (firstOccurence == true) {
      m_foundTypes[Object::OBJECT_TYPE::PERSONALCOMMENT] = offset;
      m_offsetOfTypes[offset] = Object::OBJECT_TYPE::PERSONALCOMMENT;
      firstOccurence = false;
    }
  } while (true);

  firstOccurence = true;
  offset = string::npos;
  do {
    offset = containedLine.find(
        Object::getStartTagOfObjectType(Object::OBJECT_TYPE::POEMTRANSLATION),
        (firstOccurence) ? 0 : offset + 1);
    if (offset == string::npos)
      break;
    m_poemTranslationStringTable[offset] = containedLine.find(
        Object::getEndTagOfObjectType(Object::OBJECT_TYPE::POEMTRANSLATION),
        offset);
    if (firstOccurence == true) {
      m_foundTypes[Object::OBJECT_TYPE::POEMTRANSLATION] = offset;
      m_offsetOfTypes[offset] = Object::OBJECT_TYPE::POEMTRANSLATION;
      firstOccurence = false;
    }
  } while (true);

  firstOccurence = true;
  offset = string::npos;
  do {
    offset = containedLine.find(
        Object::getStartTagOfObjectType(Object::OBJECT_TYPE::COMMENT),
        (firstOccurence) ? 0 : offset + 1);
    if (offset == string::npos)
      break;
    CommentStringInfo info{offset,
                           containedLine.find(Object::getEndTagOfObjectType(
                                                  Object::OBJECT_TYPE::COMMENT),
                                              offset),
                           false};
    m_commentStringTable[offset] = info;
    if (firstOccurence == true) {
      m_foundTypes[Object::OBJECT_TYPE::COMMENT] = offset;
      m_offsetOfTypes[offset] = Object::OBJECT_TYPE::COMMENT;
      firstOccurence = false;
    }
  } while (true);

  offset = string::npos;
  try {
    auto type = m_offsetOfTypes.at(0);
    // skip first line number as a link actually
    if (type == Object::OBJECT_TYPE::LINENUMBER)
      offset = containedLine.find(
          Object::getStartTagOfObjectType(Object::OBJECT_TYPE::LINKFROMMAIN),
          1);
  } catch (exception &) {
    offset = containedLine.find(
        Object::getStartTagOfObjectType(Object::OBJECT_TYPE::LINKFROMMAIN), 0);
  }
  if (offset != string::npos) {
    m_foundTypes[Object::OBJECT_TYPE::LINKFROMMAIN] = offset;
    m_offsetOfTypes[offset] = Object::OBJECT_TYPE::LINKFROMMAIN;
    LinkStringInfo info{
        offset,
        containedLine.find(
            Object::getEndTagOfObjectType(Object::OBJECT_TYPE::LINKFROMMAIN),
            offset),
        false};
    m_linkStringTable[offset] = info;
    do {
      offset = containedLine.find(
          Object::getStartTagOfObjectType(Object::OBJECT_TYPE::LINKFROMMAIN),
          offset + 1);
      if (offset == string::npos)
        break;
      LinkStringInfo info{
          offset,
          containedLine.find(
              Object::getEndTagOfObjectType(Object::OBJECT_TYPE::LINKFROMMAIN),
              offset),
          false};
      m_linkStringTable[offset] = info;
    } while (true);
  }
  searchForEmbededLinks();
}

using ObjectPtr = unique_ptr<Object>;

ObjectPtr createObjectFromType(Object::OBJECT_TYPE type,
                               const string &fromFile) {
  if (type == Object::OBJECT_TYPE::LINENUMBER)
    return make_unique<LineNumberPlaceholderLink>();
  else if (type == Object::OBJECT_TYPE::SPACE)
    return make_unique<Space>();
  else if (type == Object::OBJECT_TYPE::POEM)
    return make_unique<Poem>();
  else if (type == Object::OBJECT_TYPE::LINKFROMMAIN)
    return make_unique<LinkFromMain>(fromFile);
  else if (type == Object::OBJECT_TYPE::PERSONALCOMMENT)
    return make_unique<PersonalComment>(fromFile);
  else if (type == Object::OBJECT_TYPE::POEMTRANSLATION)
    return make_unique<PoemTranslation>(fromFile);
  else if (type == Object::OBJECT_TYPE::COMMENT)
    return make_unique<Comment>(fromFile);
  return nullptr;
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
      if (debug >= LOG_INFO) {
        METHOD_OUTPUT << "whole string: " << current->getWholeString() << endl;
        METHOD_OUTPUT << "display as:" << current->getDisplayString() << "||"
                      << endl;
      }
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
    // never try to find another LINENUMBER
    if (type != Object::OBJECT_TYPE::LINENUMBER) {
      auto nextOffsetOfSameType = originalString.find(
          Object::getStartTagOfObjectType(type), offset + 1);
      do {
        if (nextOffsetOfSameType != string::npos and
            isEmbeddedObject(type, nextOffsetOfSameType))
          nextOffsetOfSameType = originalString.find(
              Object::getStartTagOfObjectType(type), nextOffsetOfSameType + 1);
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

Object::SET_OF_OBJECT_TYPES
CoupledBodyTextWithLink::getContainedObjectTypes(const string &originalString) {
  Object::SET_OF_OBJECT_TYPES resultSet;
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
  unsigned int endOfSubStringOffset = 0;
  bool pureTextFound = false;
  do {
    if (m_offsetOfTypes.empty())
      break;
    auto first = m_offsetOfTypes.begin();
    auto type = first->second;
    auto offset = first->first;
    if (not isEmbeddedObject(type, offset)) {
      string text = originalString.substr(endOfSubStringOffset,
                                          offset - endOfSubStringOffset);
      if (debug >= LOG_INFO)
        METHOD_OUTPUT << text << endl;
      if (offset > endOfSubStringOffset and not isMixedOfSpaceBrackets(text))
        pureTextFound = true;
      resultSet.insert(type);
      auto current = createObjectFromType(type, m_file);
      current->loadFirstFromContainedLine(originalString, offset);
      // should add length of substring above loadFirstFromContainedLine gets
      // so require the string be fixed before
      endOfSubStringOffset = offset + current->length();
      if (debug >= LOG_INFO)
        METHOD_OUTPUT << current->length() << " " << endOfSubStringOffset
                      << endl;
    }
    m_offsetOfTypes.erase(first);
    // never try to find another LINENUMBER
    if (type != Object::OBJECT_TYPE::LINENUMBER) {
      auto nextOffsetOfSameType = originalString.find(
          Object::getStartTagOfObjectType(type), offset + 1);
      do {
        if (nextOffsetOfSameType != string::npos and
            isEmbeddedObject(type, nextOffsetOfSameType))
          nextOffsetOfSameType = originalString.find(
              Object::getStartTagOfObjectType(type), nextOffsetOfSameType + 1);
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
  string lastPart = originalString.substr(endOfSubStringOffset);
  if (debug >= LOG_INFO)
    METHOD_OUTPUT << lastPart << endl;
  if (lastPart != emptyString and not isMixedOfSpaceBrackets(lastPart))
    pureTextFound = true;
  if (pureTextFound)
    resultSet.insert(Object::OBJECT_TYPE::TEXT);
  m_foundTypes.clear();
  m_offsetOfTypes.clear();
  m_linkStringTable.clear();
  m_commentStringTable.clear();
  m_personalCommentStringTable.clear();
  m_poemTranslationStringTable.clear();
  return resultSet;
}

void CoupledBodyTextWithLink::render() {
  ifstream infile(m_inputFile);
  if (!infile) {
    METHOD_OUTPUT << ERROR_FILE_NOT_EXIST << m_inputFile << endl;
    return;
  }
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
