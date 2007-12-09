/*
 * news.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: news.c 0.0.1 2003/11/22 ew $
 */
 
#include "news.h"

int ifindchar(int start,char* str,char c)
{
	int len = strlen(str);
	for(int i = start+1; i < len; i++)
	{
		if(str[i] == c)
		{
			return i;
		}
	}

	return -1;
}
// left copy
void leftcopy(char* input,char* output,int pos)
{
	int index = 0;
	for(int i = 0; i < pos; i++)
	{
		output[index] = input[i];
		index++;
	}
	output[index] = 0;
}
// right copy
void rightcopy(char* input,char* output,int pos)
{
	int index = 0;
	int len = strlen(input);
	for(int i = pos; i < len; i++)
	{
		output[index] = input[i];
		index++;
	}
	output[index] = 0;

}
bool strloadfile(char* file,char* output)
{
	//int count = 0;
	ifstream fin(file);
	if(fin)
	{
		char ch;
		int str_i = 0;
		while(fin.get(ch))
		{
			output[str_i] = ch;
			str_i++;
		}

		output[str_i] = 0;
		//dsyslog("output %s", output);
		return true;
	}
	else
	{
		return false;
	}

	return false;
}
int getfilestringlength(char* filename)
{
	int count = 0;
	ifstream fin(filename);
	if(fin)
	{
		char ch;
		while(fin.get(ch))
		{
			count++;
		}

		fin.close();
		return count;

	} else { return -1; } // failure

	return -1;
}
int findstr(char* body,char* search)
{
	int len = strlen(body);
	int len2 = strlen(search); // search len

	for(int i = 0; i < len; i++)
	{
		if(body[i] == search[0])
		{
			bool ichk = true;

			for(int z = 0; z < len2; z++)
			{
				if(body[i+z] == search[z])
				{
				}
				else
				{
					ichk = false;
				}
			}

			if(ichk == true)
			{
				return i;
			}
		}
	}

	return -1; // failure
}

cNews::cNews()
{
	buffer = NULL;
	scrolltext = NULL;
	descriptiontext = NULL;
}

cNews::~cNews()
{
	if(buffer)
		free(buffer);
	if(scrolltext)
		free(scrolltext);
	if(descriptiontext)
		free(descriptiontext);	
}

char* cNews::getScrolltext()
{
	return scrolltext;
}

char* cNews::getDescriptiontext()
{
	return descriptiontext;
}

char* cNews::getBuffer()
{
	return buffer;
}

bool cNews::downloadRDF(char* url, const char *plugin_Name, char *option_savePath)
{
	if(!url)
		return false;
		
	char* URL_host = new char[100];
	char* URL_file = new char[100];
	memset(URL_host, 0, 100);
	memset(URL_file, 0, 100);
	
	char URL_http_start[] = "http://";
	char URL_file_start[] = "file://";
	
	int iFileSize = 0;
	
	if (findstr(url, URL_http_start) == 0)  //download it from the web
	{
		dsyslog("Newsticker downloading %s", url);
		char tmpSystem [300];
		//dsyslog("option_savePath = %s", option_savePath);
		const char *MyConfigDir;
		if(option_savePath)
			MyConfigDir = option_savePath;
		else
			MyConfigDir = cPlugin::ConfigDirectory(plugin_Name);
		if(!MyConfigDir)
			dsyslog("Error getting the config directory!");
		sprintf(tmpSystem, "wget -O %s/newsticker.rdf -o %s/newsticker.log -T 5 %s",MyConfigDir, MyConfigDir, url);
		//dsyslog("newsticker: %s" , tmpSystem);
		 
		int dow_i = system (tmpSystem);
		if (dow_i == -1)
			return false;

		int start_i =  ifindchar(strlen(URL_http_start), url, '/');
		if(start_i == -1)
			return false;
		
		//int i = 0;
		
		sprintf(tmpSystem, "%s/newsticker.rdf", MyConfigDir);
		//dsyslog("newsticker: %s" , tmpSystem);
		iFileSize = getfilestringlength(tmpSystem);
		
		//clean the buffer
		if(buffer)
			free(buffer);
			
		buffer = (char *) malloc(iFileSize + 1);
		memset(buffer, 0, iFileSize + 1);
		bool b_loadfile = strloadfile(tmpSystem, buffer);
		
		if(!b_loadfile)
		{
			delete URL_host;
			delete URL_file;
			return false;
		}
	}
	else if(findstr(url, URL_file_start) == 0) // load it from a file
	{
		//int i = 0;
		strncpy(URL_file, url + strlen(URL_file_start), strlen(url) - strlen(URL_file_start));
		//dsyslog("Newsticker loading file %s", URL_file);
		
		iFileSize = getfilestringlength(URL_file);
		
		//clean the buffer
		if(buffer)
			free(buffer);
			
		buffer = (char *) malloc(iFileSize + 1);
		memset(buffer, 0, iFileSize + 1);
		bool b_loadfile = strloadfile(URL_file, buffer);
		
		if(!b_loadfile)
		{
			delete URL_host;
			delete URL_file;
			return false;
		}
	}
	else
		return false;	
	
	char *startitem, *stopitem, *starttitle, *stoptitle;
	//char *startdescription, *stopdescription;
	int messagefound = false;
	//int loop;

	buffersize = 0;
	
	
	
	//alloc buffers for scroll- & descriptiontext
	
	//Here could be aproblem if th number of items is greater then 40
		buffersize = iFileSize + 200;

		scrolltext      = (char*)malloc(buffersize);
		descriptiontext = (char*)malloc(buffersize);
		memset(scrolltext, 0, buffersize);
		memset(descriptiontext, 0, buffersize);

	//channel titel

		if((starttitle = strstr(buffer, "<title>")))
		{
			//dsyslog("Newsticker channel title");
			starttitle += 7;

			stoptitle  = strstr(starttitle, "</title>");
			*stoptitle = 0;

			strcpy(scrolltext, starttitle);
			strcat(scrolltext, " +++ ");
			
			//dsyslog("scrolltext=%s",scrolltext);

			*stoptitle = '<';
		}
		else strcpy(scrolltext, "News +++ ");

	//news title & description

		startitem = buffer;

		strcpy(descriptiontext, " +++ ");
		descriptionfound = false;

		while((startitem = strstr(startitem, "<item")))
		{			
			startitem += 6;

			stopitem  = strstr(startitem, "</item>");
			*stopitem = 0;
			
			starttitle = strstr(startitem, "<title>");
			

			if(starttitle)
			{				
				starttitle += 7;

				stoptitle  = strstr(starttitle, "</title>");
				*stoptitle = 0;
				strcat(scrolltext, starttitle);
				strcat(scrolltext, " +++ ");
				*stoptitle = '<';

				messagefound = true;
			}/*

			if((startdescription = strstr(startitem, "<description>")))
			{
				startdescription += 13;

				stopdescription  = strstr(startdescription, "</description>");
				*stopdescription = 0;

				strcat(descriptiontext, startdescription);
				strcat(descriptiontext, " +++ ");

				descriptionfound = true;
			}*/

			startitem = stopitem + 7;
		}

		if(!messagefound)
			strcat(scrolltext, "Keine Nachrichten auf der angegebenen Seite gefunden +++");

		delete URL_host;
		delete URL_file;
	
		return true;
}


