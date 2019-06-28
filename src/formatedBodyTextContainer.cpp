#include "formatedBodyTextContainer.hpp"

void FormatedBodyTextContainer::numbering() {
  dissembleFromHTM();
  m_bodyText.setInputBodyTextFilePath(getBodyTextFilePath());
  m_bodyText.setOutputBodyTextFilePath(getTempBodyTextFixFilePath());
  m_bodyText.addLineNumber();
  loadFixedBodyTexts();
  assembleBackToHTM();
}

void FormatedBodyTextContainer::loadFixedBodyTexts() {
  Poco::File fileToCopy(getTempBodyTextFixFilePath());
  fileToCopy.copyTo(getBodyTextFilePath());
}
