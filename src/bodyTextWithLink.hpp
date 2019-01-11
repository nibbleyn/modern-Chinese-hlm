#pragma once

#include "bodyText.hpp"
#include "link.hpp"

class BodyTextWithLink : public BodyText {
public:
  BodyTextWithLink() = default;
  BodyTextWithLink(const string &filePrefix) : BodyText(filePrefix) {}

  void fixLinksFromFile(const string &file, fileSet files, int attachNo = 0,
                        int minPara = 0, int maxPara = 0, int minLine = 0,
                        int maxLine = 0);

  void removePersonalCommentsOverNumberedFiles(const string &file,
                                               int attachNo = 0);

private:
  using LinkPtr = std::unique_ptr<Link>;
  LinkPtr m_linkPtr{nullptr};
  LinkPtr m_followingLinkPtr{nullptr};
};
