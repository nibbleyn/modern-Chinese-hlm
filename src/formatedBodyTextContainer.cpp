#include "formatedBodyTextContainer.hpp"
#include "coupledBodyTextWithLink.hpp"

void FormatedBodyTextContainer::numbering() {
  dissembleFromHTM();
  CoupledBodyTextWithLink bodyText;
  bodyText.setInputBodyTextFilePath(getBodyTextFilePath());
  bodyText.setOutputBodyTextFilePath(getTempBodyTextFixFilePath());
  bodyText.disableNumberingStatistics();
  bodyText.addLineNumber();
  loadFixedBodyTexts();
  assembleBackToHTM();
}

void FormatedBodyTextContainer::loadFixedBodyTexts() {
  Poco::File fileToCopy(getTempBodyTextFixFilePath());
  fileToCopy.copyTo(getBodyTextFilePath());
}
