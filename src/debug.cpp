#include "coupledBodyTextWithLink.hpp"

void CoupledBodyText::printResultLines() {
  if (not m_resultLines.empty()) {
    METHOD_OUTPUT << "m_resultLines:" << endl;
    METHOD_OUTPUT << "lineNumber/line text" << endl;
  } else
    METHOD_OUTPUT << "no entry in m_resultLines." << endl;
  for (const auto &element : m_resultLines) {
    LineNumber ln(element.first.first, element.first.second);
    METHOD_OUTPUT << ln.asString() << "        " << element.second << endl;
  }
}

void CoupledBodyText::printImgGroupFollowingParaTable() {
  if (not m_imgGroupFollowingParaTable.empty()) {
    METHOD_OUTPUT << "m_imgGroupFollowingParaTable:" << endl;
    METHOD_OUTPUT << "img Group line No/line No to add para below" << endl;
  } else
    METHOD_OUTPUT << "no entry in m_imgGroupFollowingParaTable." << endl;
  for (const auto &element : m_imgGroupFollowingParaTable) {
    METHOD_OUTPUT << element.first << "        " << element.second << endl;
  }
}

void CoupledBodyText::printLineAttrTable() {
  if (not m_lineAttrTable.empty()) {
    METHOD_OUTPUT << "m_lineAttrTable:" << endl;
    METHOD_OUTPUT << "line No/numberOfLines/type/summary" << endl;
  } else
    METHOD_OUTPUT << "no entry in m_lineAttrTable." << endl;

  for (const auto &element : m_lineAttrTable) {
    METHOD_OUTPUT << element.first << "        " << element.second.numberOfLines
                  << "          " << getDisplayTypeString(element.second.type)
                  << "  " << element.second.cap << endl;
  }
}

void CoupledBodyTextWithLink::printLinesTable() {
  if (not linesTable.empty()) {
    FUNCTION_OUTPUT << "linesTable:" << endl;
    FUNCTION_OUTPUT << "chapter attachment ParaNumber LineNumber: "
                       "isImageGroup numOfDisplayLines ObjectSet"
                    << endl;
  } else
    FUNCTION_OUTPUT << "no entry in linesTable." << endl;
  for (const auto &element : linesTable) {
    auto num = element.first.first;
    auto paraLine = element.first.second;
    auto detail = element.second;
    FUNCTION_OUTPUT << num.first << displaySpace << num.second << displaySpace
                    << paraLine.first << displaySpace << paraLine.second << ":"
                    << displaySpace << std::boolalpha << detail.isImgGroup
                    << displaySpace << detail.numberOfDisplayedLines
                    << displaySpace
                    << Object::typeSetAsString(detail.objectContains) << endl;
  }
}

void CoupledBodyTextWithLink::printRangeTable() {
  if (not rangeTable.empty()) {
    FUNCTION_OUTPUT << "rangeTable:" << endl;
    FUNCTION_OUTPUT << "chapter_attachment:startParaNumber/startLineNumber:  "
                       "  startParaNumber/startLineNumber ->"
                       "endParaNumber/endLineNumber"
                    << endl;
  } else
    FUNCTION_OUTPUT << "no entry in rangeTable." << endl;
  for (const auto &element : rangeTable) {
    auto num = element.first.first;
    auto paraLine = element.first.second;
    auto startPara = element.second.first;
    auto endPara = element.second.second;
    FUNCTION_OUTPUT << num.first << "_" << num.second << ":P" << paraLine.first
                    << "L" << paraLine.second
                    << ":                                         P"
                    << startPara.first << "L" << startPara.second << "  ->  P"
                    << endPara.first << "L" << endPara.second << endl;
  }
}

void CoupledBodyTextWithLink::printOffsetToObjectType() {
  if (not m_offsetOfTypes.empty())
    METHOD_OUTPUT << "m_offsetOfTypes:" << endl;
  else
    METHOD_OUTPUT << "no entry in m_offsetOfTypes." << endl;
  for (const auto &element : m_offsetOfTypes) {
    METHOD_OUTPUT << element.first << displaySpace << element.second << endl;
  }
}

void CoupledBodyTextWithLink::printObjectTypeToOffset() {
  if (not m_foundTypes.empty())
    METHOD_OUTPUT << "m_foundTypes:" << endl;
  else
    METHOD_OUTPUT << "no entry in m_foundTypes." << endl;
  for (const auto &element : m_foundTypes) {
    METHOD_OUTPUT << element.first << displaySpace << element.second << endl;
  }
}
void CoupledBodyTextWithLink::printLinkStringTable() {
  if (not m_linkStringTable.empty())
    METHOD_OUTPUT << "m_linkStringTable:" << endl;
  else
    METHOD_OUTPUT << "no entry in m_linkStringTable." << endl;
  for (const auto &element : m_linkStringTable) {
    METHOD_OUTPUT << element.first << displaySpace << element.second.endOffset
                  << displaySpace << element.second.embedded << endl;
  }
}

void CoupledBodyTextWithLink::printCommentStringTable() {
  if (not m_commentStringTable.empty())
    METHOD_OUTPUT << "m_commentStringTable:" << endl;
  else
    METHOD_OUTPUT << "no entry in m_commentStringTable." << endl;
  for (const auto &element : m_commentStringTable) {
    METHOD_OUTPUT << element.first << displaySpace << element.second.endOffset
                  << displaySpace << element.second.embedded << endl;
  }
}

void CoupledBodyTextWithLink::printPersonalCommentStringTable() {
  if (not m_personalCommentStringTable.empty())
    METHOD_OUTPUT << "m_personalCommentStringTable:" << endl;
  else
    METHOD_OUTPUT << "no entry in m_personalCommentStringTable." << endl;
  for (const auto &element : m_personalCommentStringTable) {
    METHOD_OUTPUT << element.first << displaySpace << element.second << endl;
  }
}

void CoupledBodyTextWithLink::printPoemTranslationStringTable() {
  if (not m_poemTranslationStringTable.empty())
    METHOD_OUTPUT << "m_poemTranslationStringTable:" << endl;
  else
    METHOD_OUTPUT << "no entry in m_poemTranslationStringTable." << endl;
  for (const auto &element : m_poemTranslationStringTable) {
    METHOD_OUTPUT << element.first << displaySpace << element.second << endl;
  }
}

void CoupledBodyTextWithLink::printOversizedLines() {
  for (const auto &element : m_lineAttrTable) {
    if (element.second.numberOfLines > m_SizeOfReferPage) {
      METHOD_OUTPUT << "file:      " << m_file << endl;
      METHOD_OUTPUT << element.first << displaySpace
                    << element.second.numberOfLines << displaySpace
                    << getDisplayTypeString(element.second.type) << displaySpace
                    << element.second.cap << endl;
    }
  }
}

void CoupledBodyTextWithLink::printParaHeaderTable() {
  if (not m_paraHeaderTable.empty()) {
    METHOD_OUTPUT << "m_paraHeaderTable:" << endl;
    METHOD_OUTPUT << "line No/seqOfParaHeader/totalLinesAbove" << endl;
  } else
    METHOD_OUTPUT << "no entry in m_paraHeaderTable." << endl;
  for (const auto &element : m_paraHeaderTable) {
    METHOD_OUTPUT << element.first << displaySpace
                  << element.second.seqOfParaHeader << "                  "
                  << element.second.totalLinesAbove << endl;
  }
}

#include "linkSetContainer.hpp"

void LinkSetContainer::printParaHeaderTable() {
  if (not m_linkStringSet.empty()) {
    METHOD_OUTPUT << "m_linkStringSet:" << endl;
  } else
    METHOD_OUTPUT << "no entry in m_linkStringSet." << endl;
  for (const auto &element : m_linkStringSet) {
    METHOD_OUTPUT << element.first.first.first << "        "
                  << element.first.first.second << "        "
                  << element.first.second.first << "        "
                  << element.first.second.second << "        " << element.second
                  << endl;
  }
}
