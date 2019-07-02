#pragma once
#include "link.hpp"
#include <fstream>

static constexpr const char *REF_ATTACHMENT_LIST_FILENAME =
    R"(RefAttachments.txt)";
static constexpr const char *SRC_REF_ATTACHMENT_LIST_PATH =
    R"(utf8HTML/src/RefAttachments.txt)";
static constexpr const char *OUTPUT_REF_ATTACHMENT_LIST_PATH =
    R"(utf8HTML/output/RefAttachments.txt)";

static constexpr const char *ATTACHMENT_DIR = R"(attachment/)";
static const string HTML_SRC_ATTACHMENT = R"(utf8HTML/src/attachment/)";
static const string htmlTitleStart = R"(<title>)";
static const string htmlTitleEnd = R"(</title>)";
static const string endOfHtmlHead = R"(/head)";

enum class ATTACHMENT_TYPE { PERSONAL, REFERENCE, NON_EXISTED };

// statistics about links to attachments
struct AttachmentDetails {
  string fromfilename;
  string fromLine;
  string title;
  ATTACHMENT_TYPE type{ATTACHMENT_TYPE::NON_EXISTED};
};

using AttachmentSet = map<AttachmentNumber, AttachmentDetails>;

/**
 * personalAttachmentType means a personal review
 * which could be removed thru removePersonalViewpoints();
 * referenceAttachmentType means reference to other
 * stories or about a person
 */
static const string personalAttachmentType = R"(1)";
static const string referenceAttachmentType = R"(0)";

string attachmentTypeAsString(ATTACHMENT_TYPE type);
ATTACHMENT_TYPE attachmentTypeFromString(const string &str);

class AttachmentList {
public:
  static bool isTitleMatch(string annotation, string title);
  static string getAttachmentTitleFromFile(AttachmentNumber num);

public:
  void setSourceFile(const string &filepath) { m_sourceFile = filepath; }
  void loadReferenceAttachmentList();
  void addOrUpdateOneItem(AttachmentNumber num, AttachmentDetails &detail);
  void saveAttachmentList();
  string getFromLineOfAttachment(AttachmentNumber num);
  ATTACHMENT_TYPE getAttachmentType(AttachmentNumber num);
  void displayNewlyAddedAttachments();
  LinkStringSet allAttachmentsAsLinksByType(ATTACHMENT_TYPE type);

private:
  string getAttachmentTitle(AttachmentNumber num);
  AttachmentSet m_table;
  using AttachmentNumberSet = set<AttachmentNumber>;
  AttachmentNumberSet m_newlyAddedAttachmentSet;
  AttachmentNumberSet m_notUpdatedAttachmentSet;
  string m_sourceFile{SRC_REF_ATTACHMENT_LIST_PATH};
  string m_outputFile{OUTPUT_REF_ATTACHMENT_LIST_PATH};
};
