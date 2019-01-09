#pragma once
#include "container.hpp"
#include "link.hpp"

// link fixing for main files
void fixLinksFromMain();

// link fixing for attachment files
void fixLinksFromAttachment();

void findFirstInNoAttachmentFiles(const string &key, const string &fileType,
                                  const string &outputFilename);

void removePersonalViewpoints(int minTarget, int maxTarget, FILE_TYPE fileType);

using LinkRange = vector<pair<string, string>>;
// make a sub story
void reConstructStory(string indexFilename);
