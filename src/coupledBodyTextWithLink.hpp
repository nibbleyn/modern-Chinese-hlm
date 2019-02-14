#pragma once

#include "coupledBodyText.hpp"
#include "link.hpp"

class CoupledBodyTextWithLink : public CoupledBodyText {
public:
  CoupledBodyTextWithLink() = default;
  CoupledBodyTextWithLink(const string &filePrefix)
      : CoupledBodyText(filePrefix) {}

  void fixLinksFromFile(fileSet referFiles, int minPara = 0, int maxPara = 0,
                        int minLine = 0, int maxLine = 0);

  void removePersonalCommentsOverNumberedFiles();
  int sizeAfterRendering(const string &lineStr);

private:
  using LinkPtr = std::unique_ptr<Link>;
  LinkPtr m_linkPtr{nullptr};
  LinkPtr m_followingLinkPtr{nullptr};
};
