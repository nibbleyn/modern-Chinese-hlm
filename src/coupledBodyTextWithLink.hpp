#pragma once

#include "link.hpp"

class CoupledBodyTextWithLink : public CoupledBodyText {
public:
  CoupledBodyTextWithLink() = default;
  CoupledBodyTextWithLink(const string &filePrefix)
      : CoupledBodyText(filePrefix) {}

  void fixLinksFromFile(fileSet referFiles, bool forceUpdate = true,
                        int minPara = 0, int maxPara = 0, int minLine = 0,
                        int maxLine = 0);

  void removePersonalCommentsOverNumberedFiles();
  void removeImageForAutoNumbering() {}
  void addImageBackForManualNumbering() {}

  int sizeAfterRendering(const string &lineStr);

private:
  using LinkPtr = std::unique_ptr<Link>;
  LinkPtr m_linkPtr{nullptr};
  LinkPtr m_followingLinkPtr{nullptr};
};

void testMixedObjects();
