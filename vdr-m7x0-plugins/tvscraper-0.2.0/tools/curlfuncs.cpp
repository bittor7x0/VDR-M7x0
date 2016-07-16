/*
Copyright (c) 2002, Mayukh Bose
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

* Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.  

* Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

* Neither the name of Mayukh Bose nor the names of other
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
  Change History:
  11/23/2004 - Removed the #include <unistd.h> line because I didn't 
               need it. Wonder why I had it there in the first place :).
  10/20/2004 - Publicly released this code. 
*/
#include <string>
#include <cstdio>
#include <curl/curl.h>
#include <curl/easy.h>
#include "curlfuncs.h"

#ifndef TRUE
#define TRUE 1
#endif

using namespace std;

// Local function prototypes
int CurlDoPost(const char *url, string *sOutput, const string &sReferer,
	       struct curl_httppost *formpost, struct curl_slist *headerlist);

namespace curlfuncs {
  string sBuf;
  bool bInitialized = false;
  CURL *curl = NULL;
}

size_t collect_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
  string sTmp;
  register size_t actualsize = size * nmemb;
  if ((FILE *)stream == NULL) {
    sTmp.assign((char *)ptr, actualsize);
    curlfuncs::sBuf += sTmp;
  }
  else {
    fwrite(ptr, size, nmemb, (FILE *)stream);
  }
  return actualsize;
}

inline void InitCurlLibraryIfNeeded() 
{
  if (!curlfuncs::bInitialized) {
    curl_global_init(CURL_GLOBAL_ALL);
    curlfuncs::curl = curl_easy_init();
    if (!curlfuncs::curl)
      throw string("Could not create new curl instance");
    curl_easy_setopt(curlfuncs::curl, CURLOPT_NOPROGRESS, 1);       // Do not show progress
    curl_easy_setopt(curlfuncs::curl, CURLOPT_WRITEFUNCTION, collect_data);
    curl_easy_setopt(curlfuncs::curl, CURLOPT_WRITEDATA, 0);       // Set option to write to string
    curl_easy_setopt(curlfuncs::curl, CURLOPT_FOLLOWLOCATION, TRUE);
    curl_easy_setopt(curlfuncs::curl, CURLOPT_USERAGENT, "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0; Mayukh's libcurl wrapper http://www.mayukhbose.com/)");
    curlfuncs::bInitialized = true;
  }
}

int CurlGetUrl(const char *url, string *sOutput, const string &sReferer) 
{
  InitCurlLibraryIfNeeded();

  curl_easy_setopt(curlfuncs::curl, CURLOPT_URL, url);            // Set the URL to get
  if (sReferer != "")
    curl_easy_setopt(curlfuncs::curl, CURLOPT_REFERER, sReferer.c_str());
  curl_easy_setopt(curlfuncs::curl, CURLOPT_HTTPGET, TRUE);
  curl_easy_setopt(curlfuncs::curl, CURLOPT_WRITEDATA, 0);       // Set option to write to string
  curlfuncs::sBuf = "";
  if (curl_easy_perform(curlfuncs::curl) == 0)
    *sOutput = curlfuncs::sBuf;
  else {
    // We have an error here mate!
    *sOutput = "";
    return 0;
  }
  
  return 1;
}

int CurlGetUrlFile(const char *url, const char *filename, const string &sReferer)
{
  int nRet = 0;
  InitCurlLibraryIfNeeded();
  
  // Point the output to a file
  FILE *fp;
  if ((fp = fopen(filename, "w")) == NULL)
    return 0;

  curl_easy_setopt(curlfuncs::curl, CURLOPT_WRITEDATA, fp);       // Set option to write to file
  curl_easy_setopt(curlfuncs::curl, CURLOPT_URL, url);            // Set the URL to get
  if (sReferer != "")
    curl_easy_setopt(curlfuncs::curl, CURLOPT_REFERER, sReferer.c_str());
  curl_easy_setopt(curlfuncs::curl, CURLOPT_HTTPGET, TRUE);
  if (curl_easy_perform(curlfuncs::curl) == 0)
    nRet = 1;
  else
    nRet = 0;

  curl_easy_setopt(curlfuncs::curl, CURLOPT_WRITEDATA, NULL);     // Set option back to default (string)
  fclose(fp);
  return nRet;
}

int CurlPostUrl(const char *url, const string &sPost, string *sOutput, const string &sReferer)
{
  InitCurlLibraryIfNeeded();

  int retval = 1;
  string::size_type nStart = 0, nEnd, nPos;
  string sTmp, sName, sValue;
  struct curl_httppost *formpost=NULL;
  struct curl_httppost *lastptr=NULL;
  struct curl_slist *headerlist=NULL;

  // Add the POST variables here
  while ((nEnd = sPost.find("##", nStart)) != string::npos) {
    sTmp = sPost.substr(nStart, nEnd - nStart);
    if ((nPos = sTmp.find("=")) == string::npos)
      return 0;
    sName = sTmp.substr(0, nPos);
    sValue = sTmp.substr(nPos+1);
    curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, sName.c_str(), CURLFORM_COPYCONTENTS, sValue.c_str(), CURLFORM_END);
    nStart = nEnd + 2;
  }
  sTmp = sPost.substr(nStart);
  if ((nPos = sTmp.find("=")) == string::npos)
    return 0;
  sName = sTmp.substr(0, nPos);
  sValue = sTmp.substr(nPos+1);
  curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, sName.c_str(), CURLFORM_COPYCONTENTS, sValue.c_str(), CURLFORM_END);

  retval = CurlDoPost(url, sOutput, sReferer, formpost, headerlist);

  curl_formfree(formpost);
  curl_slist_free_all(headerlist);
  return retval;
}

int CurlPostRaw(const char *url, const string &sPost, string *sOutput, const string &sReferer)
{
  InitCurlLibraryIfNeeded();

  int retval;
  struct curl_httppost *formpost=NULL;
  struct curl_slist *headerlist=NULL;

  curl_easy_setopt(curlfuncs::curl, CURLOPT_POSTFIELDS, sPost.c_str());
  curl_easy_setopt(curlfuncs::curl, CURLOPT_POSTFIELDSIZE, 0); //FIXME: Should this be the size instead, in case this is binary string?

  retval = CurlDoPost(url, sOutput, sReferer, formpost, headerlist);

  curl_formfree(formpost);
  curl_slist_free_all(headerlist);
  return retval;  
}

int CurlDoPost(const char *url, string *sOutput, const string &sReferer,
		struct curl_httppost *formpost, struct curl_slist *headerlist) 
{
  headerlist = curl_slist_append(headerlist, "Expect:");

  // Now do the form post
  curl_easy_setopt(curlfuncs::curl, CURLOPT_URL, url);
  if (sReferer != "")
    curl_easy_setopt(curlfuncs::curl, CURLOPT_REFERER, sReferer.c_str());
  curl_easy_setopt(curlfuncs::curl, CURLOPT_HTTPPOST, formpost);

  curl_easy_setopt(curlfuncs::curl, CURLOPT_WRITEDATA, 0); // Set option to write to string
  curlfuncs::sBuf = "";
  if (curl_easy_perform(curlfuncs::curl) == 0) {
    *sOutput = curlfuncs::sBuf;
    return 1;
  }
  else {
    // We have an error here mate!
    *sOutput = "";
    return 0;
  }
}

void FreeCurlLibrary(void)
{
  if (curlfuncs::curl)
    curl_easy_cleanup(curlfuncs::curl);
  curl_global_cleanup();
  curlfuncs::bInitialized = false;
}

int CurlSetCookieFile(char *filename)
{
  InitCurlLibraryIfNeeded();
  if (curl_easy_setopt(curlfuncs::curl, CURLOPT_COOKIEFILE, filename) != 0)
    return 0;
  if (curl_easy_setopt(curlfuncs::curl, CURLOPT_COOKIEJAR, filename) != 0)
    return 0;
  return 1;
}

char *CurlEscape(const char *url) {
    InitCurlLibraryIfNeeded();
    return curl_escape(url , strlen(url));
}
