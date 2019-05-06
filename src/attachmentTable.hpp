#pragma once
#include "link.hpp"
#include <fstream>

static const string HTML_SRC_REF_ATTACHMENT_LIST =
    "utf8HTML/src/RefAttachments.txt";
static const string HTML_OUTPUT_REF_ATTACHMENT_LIST =
    "utf8HTML/output/RefAttachments.txt";
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
 * personalAttachmentType means a personal review could be removed thru
 * removePersonalViewpoints() referenceAttachmentType means reference to other
 * stories or about a person
 */
static const string personalAttachmentType = R"(1)";
static const string referenceAttachmentType = R"(0)";

string attachmentTypeAsString(ATTACHMENT_TYPE type);
ATTACHMENT_TYPE attachmentTypeFromString(const string &str);
AttachmentNumber getAttachmentNumber(const string &filename);

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
  string m_sourceFile{HTML_SRC_REF_ATTACHMENT_LIST};
  string m_outputFile{HTML_OUTPUT_REF_ATTACHMENT_LIST};
};
