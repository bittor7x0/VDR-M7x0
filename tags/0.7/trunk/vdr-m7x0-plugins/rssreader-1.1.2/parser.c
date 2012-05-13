/*
 * parser.c: RSS Reader plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/config.h>
#include "common.h"
#include "config.h"
#include "tools.h"
#include "parser.h"
#include "http_fetcher.h"
#include "ezxml.h"

// --- Globals ----------------------------------------------------------

cParser Parser;

// --- cItem(s) ---------------------------------------------------------

cItem::cItem(const char *e): conv(e,DEFAULT_ENCONDING)
{
	debug("CItem::cItem [%s,%s]",e,DEFAULT_ENCONDING);
}


void cItem::Clear(void)
{  
  debug("CItem::Clear [%s]",title.c_str());
  date.clear();
  title.clear();
  link.clear();
  description.clear();
}

void cItem::SetDate(const char *str)
{
  date = conv.Convert(str);
  striphtml(date);
}

void cItem::SetTitle(const char *str)
{
  title = conv.Convert(str);
  striphtml(title);
}

void cItem::SetLink(const char *str)
{
  link = conv.Convert(str);
  striphtml(link);
}

void cItem::SetDescription(const char *str)
{
  description = conv.Convert(str);
  striphtml(description);
}


// --- Parse RSS  -------------------------------------------------------


cParser::cParser()
{
  data.memory = NULL;
  data.size = 0;
}

cParser::~cParser()
{
  if (data.memory) {
     free(data.memory);
     data.memory = NULL;
     data.size = 0;
     }
}


int cParser::DownloadAndParse(const char *url)
{

  // Clear Items list 
  debug ("cParser::DownloadAndParse: entrando  %s",url);
  Items.Clear();
  debug ("cParser::DownloadAndParse: borrando  %s",url);
  if (data.memory)
     free(data.memory);
  data.memory = NULL;
  data.size = 0;

  debug ("cParser::DownloadAndParse: Init download %s",url);

  // Some servers don't like requests that are made without a user-agent field
  http_setUserAgent(RSSREADER_USERAGENT);
  // Set timeout to 30 seconds  
  http_setTimeout(30);

  data.size = http_fetch(url, &(data.memory));	/* Downloads page */
  debug("End download %s +%d",url,(int)data.size);
  if( (int)data.size == -1) {				/* All HTTP Fetcher functions return */
	http_perror("http_fetch");		/*	-1 on error. */
	error("cParser::DownloadAndParse(): couldn't download the stream: %s",http_strerror());
	if (data.memory) {
	        free(data.memory);
	        data.memory = NULL;
	        data.size = 0;
        }
     	return (RSS_DOWNLOAD_ERROR);
  }
	


  if (data.size) {
#ifdef DEBUG
     // Only for debug dump
     FILE *fp = fopen("/tmp/rssreader.vdr", "w");
     if (fp) {
        fwrite(data.memory, 1, data.size, fp);
        fclose(fp);
        }
#endif
  //Get encoding
  SetEncoding(data.memory);
  debug("cParser::DownloadAndParse(): enconding [%s]",encoding);

	ezxml_t f1 = ezxml_parse_str(data.memory,data.size);

	if ( f1 == NULL ) {
		// Free allocated memory
		if (data.memory) {
		   free(data.memory);
		   data.memory = NULL;
		   data.size = 0;
		   }
		error("cParser::DownloadAndParse(): Parse error ");
        	return (RSS_PARSING_ERROR);
	}else {
		int rss_rdf=1; //rss - 1 , rdf - 0
		debug("cParser::DownloadAndParse(): Parse ok ");
    
    

		ezxml_t node = NULL;
		ezxml_t child = NULL;
		debug("cParser::DownloadAndParse(): root node [%s]",ezxml_name(f1));
		if ( !strcmp(ezxml_name(f1),"rdf:RDF") ) {				
			rss_rdf=0;
		} 
		if ( rss_rdf ) {
			node = ezxml_get(f1,"channel",0,"item",-1);
		}else {
			node = ezxml_get(f1,"item",-1);
		}
		if (node == NULL ) {
			// Free allocated memory
			if (data.memory) {
			   free(data.memory);
			   data.memory = NULL;
			   data.size = 0;
			   }
			error("cParser::DownloadAndParse(): Parse error file");
			return (RSS_PARSING_ERROR);
		}else {
		}
		
		for (; node != NULL; node = node->next) {
			debug("cParser::DownloadAndParse(): item?");
			child=ezxml_child(node,"title");
			if (child !=NULL ) {
				debug("cParser::DownloadAndParse(): Read item [%s]",ezxml_txt(child));
				cItem *pitem = new cItem(encoding); 			
        
				Parser.Items.Add(pitem);			

				pitem->SetTitle(ezxml_txt(child)) ;			

				if ( rss_rdf ) {
					child = ezxml_child(node,"pubDate");
				} else {
					child = ezxml_child(node,"dc:date");
				}
				pitem->SetDate(ezxml_txt(child));		
				child = ezxml_child(node,"description");				
				pitem->SetDescription(ezxml_txt(child));

				child = ezxml_child(node,"content:encoded");
				if ( child) pitem->SetDescription(ezxml_txt(child)); // overdrive description with content:encoded !
		
			}
		
		}
		// Free memory
		ezxml_free (f1);
	}
  }
	
  // Free allocated memory
  if (data.memory) {
     free(data.memory);	
     data.memory = NULL;
     data.size = 0;
     }
  debug("cParser::DownloadAndParse(): Parsing OK");
  return (RSS_PARSING_OK);
}

void cParser::SetEncoding(const char *str) {
  char *aux;
  if ((aux=strstr(str, "encoding")))  {
     int len=0,ini=0;
     ini= strspn(aux + 8," ='\"") + 8;
     len= strcspn(aux + ini , "'\"");
     debug("[%s] %d %d\n",aux+ini,ini,len);
     strn0cpy(encoding, aux+ini, sizeof(char)*(len+1));
  } else {
     strn0cpy(encoding,"UTF-8", sizeof(char)*(6+1));
  }
  debug("Encoding [%s]\n",encoding);
}  
