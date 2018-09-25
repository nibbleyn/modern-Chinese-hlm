#pragma once
#include "hlmLink.hpp"

// link fixing for main files
void fixReturnLinkForAttachmentFile(const string &referFile,
                                    const string &inputHtmlFile,
                                    const string &outputFile);
void fixMainLinksOverNumberedFiles(const string &referFile, fileSet files);
void fixMainLinks(int minTarget, int maxTarget, int minReference,
                  int maxReference);
void fixReturnLinkForAttachments(int minTarget, int maxTarget);
void fixMainHtml(int minTarget, int maxTarget, int minReference,
                 int maxReference);

// link fixing for attachment files
void fixAttachmentLinksOverNumberedFiles(const string &referFile, fileSet files,
                                         int attachNo);
void fixLinksToMainForAttachments(int minTarget, int maxTarget,
                                  int minReference, int maxReference,
                                  int minAttachNo, int maxAttachNo);
void fixAttachments(int minTarget, int maxTarget, int minReference,
                    int maxReference, int minAttachNo = 0, int maxAttachNo = 0);
