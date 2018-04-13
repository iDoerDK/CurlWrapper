/**
 * @file
 * @author  Jens Arnt <jens@idoer.dk>, iDoer Aps
 * @version 1.3
 * 
 * @section HISTORY
 *  Created on February 13, 2018, 8:48 PM
 * Version 1.0 February 13, 2018
 * Version 1.1 March 20, 2018
 * Version 1.2 March 22, 2018
 * Version 1.3 April 6th 2018
 * 
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the Apache 2.0 Licence as
 * published by the Apache Software Foundation
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * Apache License for more details at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * @section DESCRIPTION
 *
 * Yada yada yada
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "curlwrapper.h"


struct NotesCurlHandlestruct {
    // In normal operation bot curlhand and return_buffer must have a valid addres or both have null,
    NotesCurlHandle notesCurlHandle; // just a copy of the indes in the array so we know what 
    CURL *curlhandle;  // null if not init'ed 
    char * return_buffer; // null if not allocated
    size_t return_buffer_size;
    unsigned int active; // 0 for inactive, 1 for active, both curlhandle and return buffer must have a valid addres if active == 1
} ;



struct NotesCurlHandleListstruct {
    long index_last_entry; // 0 if empty, >0 if it contains any entries 
    struct NotesCurlHandlestruct  *NotesCurlHandles; // 0 indexed so first entry is 0 (Actually should be NotesCurlHandles[] but due to flexible array member issues declared as "pure" pointers)
};

struct NotesCurlHandleListstruct globalNotesCurlHandleList = 
{
    .index_last_entry=-1,
    .NotesCurlHandles = ((void *) 0)
};

void PrintNotesCurlStruct (char * text)
{
    //printf("> Printing NotesCurlStruct:%s\n",text);
    //printf("> globalNotesCurlHandleList:%p\n",globalNotesCurlHandleList);
    long no_entries = globalNotesCurlHandleList.index_last_entry;
   
    if(no_entries>=0)
    { 
        switch (no_entries)     
        {
        case -1:
            //printf("  NotesCurlStruct has been initialized with %ld entries\n",no_entries);    
            //printf("  NotesCurlHandles at %p \n",globalNotesCurlHandleList.NotesCurlHandles);

            break;
        default:
            //printf("  NotesCurlStruct last entry is %ld\n",no_entries);
            //printf("  NotesCurlHandles at %p \n",globalNotesCurlHandleList.NotesCurlHandles);
            for( int i = 0; i<=no_entries;i++)
            {
                printf("    %i, Active=%i, CURLpointer at %p, Returnbuffer at %p\n",i,globalNotesCurlHandleList.NotesCurlHandles[i].active, globalNotesCurlHandleList.NotesCurlHandles[i].curlhandle, globalNotesCurlHandleList.NotesCurlHandles[i].return_buffer);
            }
            break;
        }
    } else {
        //printf("  NotesCurlStruct has not yet been initialized\n");
        1==1;
    }
    
    //printf("< Printing NotesCurlStruct:%s\n",text);
}
unsigned char isNotesCurlHandleValid (NotesCurlHandle handle)
{
    /* The handle is valid if all of the below are true:
        1) the handle is >= 0 (default is -1)
        2) The handle is lower than or equal to index_last_entry
        3) The handle points to a structure that is active
    */
    return (  handle >=0 
                && (handle <= globalNotesCurlHandleList.index_last_entry) 
                && (globalNotesCurlHandleList.NotesCurlHandles[handle].active )
            );
    // long a = globalNotesCurlHandleList.index_last_entry;
}
 void notes_curl_easy_cleanup(NotesCurlHandle notesHandle)
{
    if(isNotesCurlHandleValid(notesHandle)) {
        if(globalNotesCurlHandleList.NotesCurlHandles[notesHandle].return_buffer != NULL) {
            free(globalNotesCurlHandleList.NotesCurlHandles[notesHandle].return_buffer);
            globalNotesCurlHandleList.NotesCurlHandles[notesHandle].return_buffer=NULL;
        }
        if(globalNotesCurlHandleList.NotesCurlHandles[notesHandle].curlhandle != NULL){
            curl_easy_cleanup(globalNotesCurlHandleList.NotesCurlHandles[notesHandle].curlhandle);
            globalNotesCurlHandleList.NotesCurlHandles[notesHandle].curlhandle =NULL;
        }
        globalNotesCurlHandleList.NotesCurlHandles[notesHandle].active = 0;
    }
    void curl_easy_cleanup(CURL * handle );
}
 
 void notes_curl_global_cleanup()
 {
     unsigned long i=0;
     for (i=0;i<=globalNotesCurlHandleList.index_last_entry; i++)
     {
         notes_curl_easy_cleanup(i);
     }
     if(globalNotesCurlHandleList.NotesCurlHandles != NULL) 
     {
         free(globalNotesCurlHandleList.NotesCurlHandles);
         globalNotesCurlHandleList.NotesCurlHandles = NULL;
         globalNotesCurlHandleList.index_last_entry = -1;
     }
 }

NotesCurlHandle create_notes_curl(CURL *curlhandle)
{
    long local_index_last_entry = globalNotesCurlHandleList.index_last_entry +1 ;
    struct NotesCurlHandlestruct *localHandles = NULL;
    struct NotesCurlHandlestruct *localHandle = NULL;
    NotesCurlHandle rc_NotesCurlHandle = -1;
   
    if(globalNotesCurlHandleList.NotesCurlHandles == NULL) {
        localHandles = malloc( sizeof(struct NotesCurlHandlestruct));
    } else {
        localHandles = (struct NotesCurlHandlestruct *) realloc(globalNotesCurlHandleList.NotesCurlHandles, (local_index_last_entry+1) * sizeof(struct NotesCurlHandlestruct));
    }
       
    if (localHandles != NULL) {
        //printf("ENTERING create_notes_curl\n\n"); 
        localHandle = &localHandles[local_index_last_entry];
        localHandle->curlhandle = curlhandle;
        localHandle->return_buffer =NULL;
        localHandle->active = 1;
  
        rc_NotesCurlHandle = ++globalNotesCurlHandleList.index_last_entry; // Everything successfull until now so update the globalNotesCurlHandleList
        globalNotesCurlHandleList.NotesCurlHandles = localHandles;
             //printf("\nInside add_notes_curl5\nlocalhandle=%p\n", localHandle);
    }
    //printf("LEAVING create_notes_curl\n\n");                         
    return rc_NotesCurlHandle;
}

 extern NotesCurlHandle  notes_curl_easy_init() 
 {
     NotesCurlHandle rc_handle = -1;
     CURLcode rccode = -1;
     CURL *localCURLhandle = NULL;
     //PrintNotesCurlStruct("easy_init0");
     // rccode = notes_curl_global_init();
     //printf("ENTERING notes_curl_easy_init\n");
     //PrintNotesCurlStruct("easy_init1");
     
     localCURLhandle = curl_easy_init();
     //PrintNotesCurlStruct("easy_init2");
     //printf("lCURLhandle:%p\n", localCURLhandle);
     //printf("CURLcode:%u\n", rccode);

     if (localCURLhandle != NULL) 
     {
         //printf("Killroy .. again ...\n");
         rc_handle = create_notes_curl( localCURLhandle);
     }
    // printf("LEAVING notes_curl_easy_init\n\n");
     return  rc_handle;
 }

 CURL *notes_get_native_curl_handle ( NotesCurlHandle entry)
// Takes a pointer to NotesCurlHandlList and returns the NoteCurlHandle entry identified by the entry parameter,
// Returns NULL if entry is out of bounds.
{
    // struct NotesCurlHandleList * nCurlList = hlist; // use our own internal pointer so we do not risk misusing the original    
    CURL *rc_nativeCurlHandle = NULL; // our return value in with default of null in case we mess up.
    long noOfEntries = globalNotesCurlHandleList.index_last_entry;
    if (isNotesCurlHandleValid(entry) )
    {
        rc_nativeCurlHandle = globalNotesCurlHandleList.NotesCurlHandles[entry].curlhandle;
    }
    return rc_nativeCurlHandle;
}
 
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
// userp is pointer to a NotesCURL.. entry.
{
  size_t realsize = size * nmemb;
  struct NotesCurlHandlestruct *mem = (struct NotesCurlHandlestruct *)userp;
  //printf("WriteMemoryCallback: size=%zd, nmemb=%zd\n", size, nmemb);
  //int a; //????
  //char *mymem; // ??
  //mem->return_buffer = realloc( mem->return_buffer, 100);
  mem->return_buffer = realloc (mem->return_buffer,  mem->return_buffer_size+realsize + 1);
  if(mem->return_buffer == NULL) {
    /* out of memory! */ 
    //printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
  memcpy(&mem->return_buffer[mem->return_buffer_size], contents, realsize);
  mem->return_buffer_size += realsize;
  mem->return_buffer[mem->return_buffer_size] = 0; 
  //printf("realsize=%zd\n",mem->return_buffer_size);
  //printf("Buffer=%s\n",mem->return_buffer);
   return realsize;
}

void PrintChunk( struct NotesCurlHandlestruct *content)
{
    for (long i = 0; i < content->return_buffer_size ; i++)
    {
        printf("%c", content->return_buffer[i]);
    }
    printf("\n");
}
 
 extern char * notes_easy_curl_perform(NotesCurlHandle notes_curl_handle)
 {
     char * rc_buffer = NULL;
     CURLcode curl_error;
     //PrintNotesCurlStruct("easy_perform");
     
      //curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);/* send all data to this function  */ 
      // curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);/* we pass our 'chunk' struct to the callback function */ 
     if (isNotesCurlHandleValid(notes_curl_handle))
     { 
         if(globalNotesCurlHandleList.NotesCurlHandles[notes_curl_handle].return_buffer == NULL)
         {
             globalNotesCurlHandleList.NotesCurlHandles[notes_curl_handle].return_buffer = malloc(1);
         }
        curl_easy_setopt(globalNotesCurlHandleList.NotesCurlHandles[notes_curl_handle].curlhandle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(globalNotesCurlHandleList.NotesCurlHandles[notes_curl_handle].curlhandle  , 
                             CURLOPT_WRITEDATA, 
                             (void *) &globalNotesCurlHandleList.NotesCurlHandles[notes_curl_handle]);
        
        curl_error = curl_easy_perform(globalNotesCurlHandleList.NotesCurlHandles[notes_curl_handle].curlhandle);
        if (curl_error == CURLE_OK) 
        {
            rc_buffer = globalNotesCurlHandleList.NotesCurlHandles[notes_curl_handle].return_buffer;
        }
     }
     //printf("rc_buffer:%s\n",rc_buffer);
     return rc_buffer;
 }
/*
 int main(int argc, char *argv[])
{
	//char * url = "www.google.com";
	char * url = argv[1];
	char * payload;
	
	NotesCurlHandle nch1 =-1;
            NotesCurlHandle nch2 =-1;
            CURL *nativeCURLhandle = NULL;
            char* rc_buffer = NULL;
            
	nch1 = notes_curl_easy_init();
            //PrintNotesCurlStruct("easy_perform0");
        	nch2 = notes_curl_easy_init();
            //PrintNotesCurlStruct("easy_perform1");
            nativeCURLhandle = notes_get_native_curl_handle(nch1);
            curl_easy_setopt(nativeCURLhandle, CURLOPT_URL, url);// specify URL to get  
            curl_easy_setopt(nativeCURLhandle, CURLOPT_FOLLOWLOCATION, 1);// Follow redirects 
            curl_easy_setopt(nativeCURLhandle, CURLOPT_HEADER, 1);// Include Header in result  
            rc_buffer = notes_easy_curl_perform(nch1 );
            //printf("Printing Chunk:\n");
            // PrintChunk(&globalNotesCurlHandleList.NotesCurlHandles[nch1]);
            
            notes_curl_global_cleanup();
            //PrintNotesCurlStruct("easy_perform3");
            //notes_curl_easy_cleanup(nch2);
            //PrintNotesCurlStruct("easy_perform3");

	printf("Asking for URL:%s\n", url );
//	payload = GetNumberOfBytes(url);
	printf("Returned payload %s\n", rc_buffer);

  return 0;
}

*/