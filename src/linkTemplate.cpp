#include "link.hpp"
extern int debug;

static constexpr const char *linkToImageFile =
    R"(<a unhidden title="IMAGE" href="XX#YY">↑（图示：ZZ）</a>)";
static constexpr const char *linkToAttachmentFile =
    R"(<a unhidden href="PPb0XX_SS.htm#YY">ZZ</a>)";
static constexpr const char *linkToSameFile =
    R"(<a unhidden title="QQ" href="#YY"><sub hidden>WW</sub>ZZ</a>)";
static constexpr const char *linkToMainFile =
    R"(<a unhidden title="QQ" href="PPa0XX.htm#YY">↑<sub hidden>WW</sub>ZZ</a>)";
static constexpr const char *linkToOriginalFile =
    R"(<a unhidden title="QQ" href="PPc0XX.htm#YY">↑<sub hidden>WW</sub>ZZ</a>)";
static constexpr const char *linkToJPMFile =
    R"(<a unhidden title="QQ" href="PPdXXX.htm#YY">↑<sub hidden>WW</sub>ZZ</a>)";
// now only support reverse link from main back to main, no key required
static constexpr const char *reverseLinkToMainFile =
    R"(<a unhidden href="a0XX.htm#YY">↓<sub>WW</sub></a>)";
static constexpr const char *subParaLink =
    R"(<a unhidden title="子段QQ" id="VV" href="#YYUU">向下</a>)";

static constexpr const char *defaultPath = R"(PP)";
static constexpr const char *defaultMainFilename = R"(XX)";
static constexpr const char *defaultAttachmentNumber = R"(SS)";
static constexpr const char *defaultJpmFilename = R"(XXX)";
static constexpr const char *defaultKey = R"(QQ)";
static constexpr const char *defaultWholeKey = R"( title="QQ")";
static constexpr const char *defaultReferPara = R"(YY)";
static constexpr const char *defaultWholeReferPara = R"(#YY)";
static constexpr const char *defaultCitation = R"(WW)";
static constexpr const char *defaultWholeCitation = R"(↑<sub hidden>WW</sub>)";
static constexpr const char *defaultEndOfReverseCitation = R"(</sub>)";
static constexpr const char *defaultAnnotation = R"(ZZ)";
static constexpr const char *defaultImageAnnotation = R"(↑（图示：ZZ）)";
static constexpr const char *defaultSubParaId = R"(VV)";
static constexpr const char *defaultReferSubPara = R"(UU)";
static constexpr const char *defaultWholeSubParaId = R"( id="VV")";

/**
 * generate real correct link within same file
 * by filling right "refer para" based on key searching
 * NOTE: avoid literal strings of YY QQ WW ZZ in the original link
 * @param type display type
 * @param key the key to use in search within this same file
 * @param citation the target place identified by section number
 * @param annotation the original annotation
 * @param referPara the target place identified by line number
 * @return link after fixed
 */
string fixLinkFromSameFileTemplate(DISPLAY_TYPE type, const string &key,
                                   const string &citation,
                                   const string &annotation,
                                   const string &referPara) {
  string link = linkToSameFile;
  replaceDisplayType(link, linkStartChars, type);
  if (referPara.empty()) {
    replacePart(link, defaultWholeReferPara, emptyString);
  } else
    replacePart(link, defaultReferPara, referPara);
  // in case use top/bottom as reference name
  if (key.empty()) {
    replacePart(link, defaultWholeCitation, emptyString);
    replacePart(link, defaultWholeKey, emptyString);
  } else {
    replacePart(link, defaultKey, key);
  }
  replacePart(link, defaultCitation, citation);
  replacePart(link, defaultAnnotation, annotation);
  return link;
}

/**
 * generate real correct link to other main file
 * by filling right "refer para" based on key searching
 * NOTE: avoid literal strings of QQ PP XX YY WW ZZ
 * in the original link to get a unexpected replacement
 * @param path relative path based on the link type
 * refer to getPathOfReferenceFile()
 * @param filename the target main file's chapter name, e.g. 07
 * @param type display type
 * @param key the key to use in search over target main file
 * @param citation the target place identified by section number
 * @param annotation the original annotation
 * @param referPara the target place identified by line number
 * @return link after fixed
 */
string fixLinkFromMainTemplate(const string &path, const string &filename,
                               DISPLAY_TYPE type, const string &key,
                               const string &citation, const string &annotation,
                               const string &referPara) {
  string link = linkToMainFile;
  replaceDisplayType(link, linkStartChars, type);
  replacePart(link, defaultPath, path);
  replacePart(link, defaultMainFilename, filename);
  if (referPara.empty()) {
    replacePart(link, defaultWholeReferPara, emptyString);
  } else
    replacePart(link, defaultReferPara, referPara);
  // in case use top/bottom as reference name
  if (key.empty()) {
    replacePart(link, defaultWholeCitation, emptyString);
    replacePart(link, defaultWholeKey, emptyString);
  } else {
    replacePart(link, defaultKey, key);
  }
  replacePart(link, defaultCitation, citation);
  replacePart(link, defaultAnnotation, annotation);
  return link;
}

string fixLinkFromReverseLinkTemplate(const string &filename, DISPLAY_TYPE type,
                                      const string &citation,
                                      const string &referPara,
                                      const string &annotation) {
  string link = reverseLinkToMainFile;
  replaceDisplayType(link, linkStartChars, type);
  replacePart(link, defaultMainFilename, filename);
  if (referPara.empty()) {
    replacePart(link, defaultWholeReferPara, emptyString);
  } else
    replacePart(link, defaultReferPara, referPara);
  replacePart(link, defaultCitation, citation);
  if (annotation != emptyString)
    replacePart(link, defaultEndOfReverseCitation + linkEndChars,
                defaultEndOfReverseCitation + annotation + linkEndChars);
  return link;
}

/**
 * <a unhidden title="子段3" id="P5L1S3" href="#P5L1">向下</a>
 * <a unhidden title="子段2" id="P5L1S2" href="#P5L1S3">向下</a>
 * <a unhidden title="子段1" href="#P16L3">向下</a>
 */
string fixLinkFromSubParaLinkTemplate(const string &seqenceNumber,
                                      DISPLAY_TYPE type,
                                      const string &referPara, const string &id,
                                      const string &subPara) {
  string link = subParaLink;
  replacePart(link, defaultKey, seqenceNumber);
  replaceDisplayType(link, linkStartChars, type);
  replacePart(link, defaultReferPara, referPara);
  if (id.empty()) {
    replacePart(link, defaultWholeSubParaId, emptyString);
  } else
    replacePart(link, defaultSubParaId, id);
  if (not subPara.empty())
    replacePart(link, defaultReferSubPara, subPara);
  return link;
}

/**
 * generate real correct link to original file
 * by filling right "refer para" based on key searching
 * NOTE: avoid literal strings of QQ PP XX YY WW
 * in the original link to get a unexpected replacement
 * @param path relative path based on the link type
 * refer to getPathOfReferenceFile()
 * @param filename the target origin file's chapter name, e.g. 07
 * @param key the key to use in search over target original file
 * @param citation the target place identified by section number
 * @param referPara the target place identified by line number
 * @param annotation the original annotation
 * @return link after fixed
 */
string fixLinkFromOriginalTemplate(const string &path, const string &filename,
                                   const string &key, const string &citation,
                                   const string &annotation,
                                   const string &referPara) {
  string link = linkToOriginalFile;
  replacePart(link, defaultPath, path);
  replacePart(link, defaultMainFilename, filename);
  if (referPara.empty()) {
    replacePart(link, defaultWholeReferPara, emptyString);
  } else
    replacePart(link, defaultReferPara, referPara);
  // in case use top/bottom as reference name
  if (key.empty()) {
    replacePart(link, defaultWholeCitation, emptyString);
    replacePart(link, defaultWholeKey, emptyString);
  } else {
    replacePart(link, defaultKey, key);
  }
  replacePart(link, defaultCitation, citation);
  replacePart(link, defaultAnnotation, annotation);
  return link;
}

/**
 * generate real correct link to jpm file
 * by filling right "refer para" based on key searching
 * NOTE: avoid literal strings of QQ PP XXX YY WW ZZ
 * in the original link to get a unexpected replacement
 * @param path relative path based on the link type
 * refer to getPathOfReferenceFile()
 * @param filename the target origin file's chapter name, e.g. 07
 * @param key the key to use in search over target original file
 * @param citation the target place identified by section number
 * @param referPara the target place identified by line number
 * @param annotation the original annotation
 * @return link after fixed
 */
string fixLinkFromJPMTemplate(const string &path, const string &filename,
                              const string &key, const string &citation,
                              const string &annotation,
                              const string &referPara) {
  string link = linkToJPMFile;
  replacePart(link, defaultPath, path);
  replacePart(link, defaultJpmFilename, filename);
  if (referPara.empty()) {
    replacePart(link, defaultWholeReferPara, emptyString);
  } else
    replacePart(link, defaultReferPara, referPara);
  // in case use top/bottom as reference name
  if (key.empty()) {
    replacePart(link, defaultWholeCitation, emptyString);
    replacePart(link, defaultWholeKey, emptyString);
  } else {
    replacePart(link, defaultKey, key);
  }
  if (citation.empty()) {
    replacePart(link, defaultWholeCitation, emptyString);
  } else
    replacePart(link, defaultCitation, citation);
  replacePart(link, defaultAnnotation, annotation);
  return link;
}

/**
 * generate real correct link to attachment file
 * NOTE: avoid literal strings of PP XX YY ZZ
 * in the original link to get a unexpected replacement
 * @param path relative path based on the link type
 * refer to getPathOfReferenceFile()
 * @param filename the target attachment file's chapter name,
 * e.g. 07 of b007_3
 * @param attachNo the target attachment file's attachment index,
 * e.g. 3 of b007_3
 * @param annotation the original annotation
 * @return link after fixed
 */
string fixLinkFromAttachmentTemplate(const string &path, const string &filename,
                                     const string &attachNo,
                                     const string &annotation,
                                     const string &referPara) {
  string link = linkToAttachmentFile;
  replacePart(link, defaultPath, path);
  replacePart(link, defaultMainFilename, filename);
  replacePart(link, defaultAttachmentNumber, attachNo);
  if (referPara.empty()) {
    replacePart(link, defaultWholeReferPara, emptyString);
  } else
    replacePart(link, defaultReferPara, referPara);
  replacePart(link, defaultAnnotation, annotation);
  return link;
}

/**
 * Note: do not fix path now
 */
string fixLinkFromImageTemplate(const string &fullReferFilenameWithPathExt,
                                const string &picFilename,
                                const string &fullAnnotation,
                                const string &displayProperty) {
  string link = linkToImageFile;
  if (displayProperty != unhiddenDisplayProperty) {
    if (displayProperty == hiddenDisplayProperty)
      replacePart(link, unhiddenDisplayProperty, hiddenDisplayProperty);
    else
      // direct
      replacePart(link, unhiddenDisplayProperty + displaySpace, emptyString);
  }
  replacePart(link, defaultMainFilename, fullReferFilenameWithPathExt);
  replacePart(link, defaultReferPara, picFilename);
  if (fullAnnotation.find(upArrow) != string::npos) {
    replacePart(link, defaultImageAnnotation, fullAnnotation);
  } else {
    replacePart(link, defaultImageAnnotation, upArrow + fullAnnotation);
  }
  return link;
}
