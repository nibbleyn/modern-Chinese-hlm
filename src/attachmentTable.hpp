#pragma once
#include "utf8StringUtil.hpp"
#include <fstream>

enum class ATTACHMENT_TYPE { PERSONAL, REFERENCE, NON_EXISTED };
using AttachmentNumber =
    std::pair<int, int>; // chapter number, attachment number

// statistics about links to attachments
struct AttachmentDetails {
  string fromfilename;
  string fromLine;
  string title;
  ATTACHMENT_TYPE type{ATTACHMENT_TYPE::NON_EXISTED};
};

using AttachmentSet = std::map<AttachmentNumber, AttachmentDetails>;
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

static const string HTML_SRC_REF_ATTACHMENT_LIST =
    "utf8HTML/src/RefAttachments.txt";

class AttachmentList {
public:
  void setSourceFile(const string &filepath) { m_sourceFile = filepath; }
  void loadReferenceAttachmentList();
  void addOrUpdateOneItem(AttachmentNumber num, AttachmentDetails &detail);
  void saveAttachmentList();
  string getFromLineOfAttachment(AttachmentNumber num);
  ATTACHMENT_TYPE getAttachmentType(AttachmentNumber num);

private:
  AttachmentSet m_table;
  string m_sourceFile{HTML_SRC_REF_ATTACHMENT_LIST};
  string m_outputFile{HTML_SRC_REF_ATTACHMENT_LIST};
};
