/*
 * @file
 * @author  Jens Arnt <jens@idoer.dk>, iDoer ApS
 * @version 
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
#include <pthread.h>
#include <syslog.h>
#include "curlwrapper.h"

#define NO_ENT 10

struct NotesCurlHandlestruct {
    // In normal operation bot curlhand and return_buffer must have a valid addres or both have null,
    NotesCurlHandle notesCurlHandle; // just a copy of the index in the array so we know what 
    CURL *curlhandle;  // null if not init'ed 
    char * return_buffer; // null if not allocated
    size_t return_buffer_size;
    unsigned int active ; // 0 for inactive, 1 for active, both curlhandle and return buffer must have a valid address if active == 1
} ;

struct NotesCurlHandleListstruct {
    long number_of_entries; // size of the array - will be populated at init time. 
    struct NotesCurlHandlestruct  NotesCurlHandles[NO_ENT]; // 0 indexed so first entry is 0 (Actually should be NotesCurlHandles[] but due to flexible array member issues declared as "pure" pointers)
};

struct NotesCurlHandleListstruct globalNotesCurlHandleList = 
{
    //.number_of_entries= -1,
    .number_of_entries=NO_ENT
   // .NotesCurlHandles = ((void *) 0)

};

char * get_curlwrapper_version(){
#ifdef DEBUG
    char * notescurlversion =   CURLWRAPPER_VERSION;
#else
     char * notescurlversion =  CURLWRAPPER_VERSION;
#endif
    return notescurlversion;
}

void PrintNotesCurlStruct (long max_entries, char * text){
    //printf("> Printing NotesCurlStruct:%s\n",text);
    //printf("> globalNotesCurlHandleList:%p\n",globalNotesCurlHandleList);
    long no_entries = globalNotesCurlHandleList.number_of_entries;
   
    if(no_entries>=0)
    { 
        switch (no_entries)     
        {
        case -1:
            // Never reach this ????
            syslog(LOG_DEBUG,"%s NotesCurlStruct has been initialized with %ld entries",text,no_entries);    
            syslog(LOG_DEBUG, "%s NotesCurlHandles at %p",text,globalNotesCurlHandleList.NotesCurlHandles);
            break;
        default:
            syslog(LOG_DEBUG,"%s NotesCurlStruct last entry is %ld",text,no_entries);
            syslog(LOG_DEBUG, "%s NotesCurlHandles at %p",text,globalNotesCurlHandleList.NotesCurlHandles);
            for( int i = 0; i<max_entries;i++)
            {
                if (globalNotesCurlHandleList.NotesCurlHandles[i].active) {
                    syslog(LOG_DEBUG,"%s    %i, Active=%i, CURLpointer at %p, Returnbuffer at %p, size=%zu",text,i,globalNotesCurlHandleList.NotesCurlHandles[i].active, globalNotesCurlHandleList.NotesCurlHandles[i].curlhandle, globalNotesCurlHandleList.NotesCurlHandles[i].return_buffer,globalNotesCurlHandleList.NotesCurlHandles[i].return_buffer_size);
                }
            }
            break;
        }
    } else {
        syslog(LOG_DEBUG,"%s NotesCurlStruct has not yet been initialized",text);
        1==1;
    }
    
    //printf("< Printing NotesCurlStruct:%s\n",text);
}
unsigned char isNotesCurlHandleValid (NotesCurlHandle handle){
    /* The handle is valid if all of the below are true:
        1) the handle is >= 0 (default is -1)
        2) The handle points to a structure that is active
    */
    return (  handle >=0 
                && (handle < globalNotesCurlHandleList.number_of_entries) 
                && (globalNotesCurlHandleList.NotesCurlHandles[handle].active )
            );
    // long a = globalNotesCurlHandleList.index_last_entry;
}
 void notes_curl_easy_cleanup(NotesCurlHandle notesHandle) {
    if(isNotesCurlHandleValid(notesHandle)) {
        pthread_mutex_lock(&lock_free);
#ifdef CURLWRAPPER_DEBUG
        syslog(LOG_DEBUG, "[notes_curl_easy_cleanup] Mutex Lock lock_free");
#endif
#ifdef CURLWRAPPER_DEBUG
        syslog(LOG_DEBUG, "[notes_curl_easy_cleanup] Cleanup noteshandle=%li",notesHandle);
#endif

	// Cleanup/free any return_buffer allocations
        if(globalNotesCurlHandleList.NotesCurlHandles[notesHandle].return_buffer != NULL) {
            my_free(globalNotesCurlHandleList.NotesCurlHandles[notesHandle].return_buffer);
            globalNotesCurlHandleList.NotesCurlHandles[notesHandle].return_buffer=NULL;
        }
	// Deregister any active Curl handles.
        if(globalNotesCurlHandleList.NotesCurlHandles[notesHandle].curlhandle != NULL){
            curl_easy_cleanup(globalNotesCurlHandleList.NotesCurlHandles[notesHandle].curlhandle);
           globalNotesCurlHandleList.NotesCurlHandles[notesHandle].curlhandle =NULL;
        }
	
        globalNotesCurlHandleList.NotesCurlHandles[notesHandle].active = 0;
        pthread_mutex_unlock(&lock_free);
        #ifdef CURLWRAPPER_DEBUG
        syslog(LOG_DEBUG, "[notes_curl_easy_cleanup] Mutex Unlock lock_free");
#endif
    }
}
 void notes_curl_global_cleanup() {
     unsigned long i=0;
     
      #ifdef CURLWRAPPER_DEBUG
          syslog(LOG_NOTICE, ">notes_curl_global_cleanup");
#endif
     for (i=0;i<globalNotesCurlHandleList.number_of_entries; i++)
     {
         notes_curl_easy_cleanup(i);
     }
     /*
     if(globalNotesCurlHandleList.NotesCurlHandles != NULL) 
     {
         my_free(globalNotesCurlHandleList.NotesCurlHandles);
         globalNotesCurlHandleList.NotesCurlHandles = NULL;
         globalNotesCurlHandleList.number_of_entries = -1;
     }
      */
     #ifdef CURLWRAPPER_DEBUG
          syslog(LOG_NOTICE, "<notes_curl_global_cleanup");
     #endif
           
          closelog();
 }
NotesCurlHandle create_notes_curl(CURL *curlhandle){
    //long local_index_last_entry = globalNotesCurlHandleList.index_last_entry +1 ;
    long available_entry = -1;
    struct NotesCurlHandlestruct *localHandles = globalNotesCurlHandleList.NotesCurlHandles;
    struct NotesCurlHandlestruct *localHandle = &localHandles[0];
    NotesCurlHandle rc_NotesCurlHandle = -1;
   pthread_mutex_lock(&lock_my_malloc);
   #ifdef CURLWRAPPER_DEBUG
   syslog(LOG_DEBUG, "[create_notes_curl] Mutex Lock my_malloc");
#endif
    if (localHandles != NULL) {
        //printf("ENTERING create_notes_curl\n\n"); 
        for ( long i=0; i<globalNotesCurlHandleList.number_of_entries && available_entry<0;i++) {
            if(localHandles[i].active == 0) {
                available_entry=i;
            }
         }
#ifdef CURLWRAPPER_DEBUG
    //syslog(LOG_DEBUG, "[create_notes_curl] available_index=%l", available_entry);
#endif
    if(available_entry >=0) {
        localHandle = &localHandles[available_entry];
        localHandle->curlhandle = curlhandle;
        localHandle->return_buffer =NULL;
        localHandle->return_buffer_size=0;
        localHandle->active = 1;
  }   
        rc_NotesCurlHandle =available_entry; // Everything successfull until now so update the globalNotesCurlHandleList
    }
      pthread_mutex_unlock(&lock_my_malloc);
      #ifdef CURLWRAPPER_DEBUG
      syslog(LOG_DEBUG, "[create_notes_curl] Mutex Unlock my_malloc");
#endif

    //printf("LEAVING create_notes_curl\n\n");                         
    return rc_NotesCurlHandle;
}
 extern NotesCurlHandle  notes_curl_easy_init()  {
     // Consider providing the number of concurrent slots to make it available at init time
     // unsigned int NumberOfNotesCurlHandles = NO_ENT;
     NotesCurlHandle rc_handle = -1;
     CURLcode rccode = -1;
     CURL *localCURLhandle = NULL;
     
     openlog("CurlWrapper", LOG_PID|LOG_CONS, LOG_USER);
     syslog(LOG_NOTICE, "CurlWrapper version '%s'",get_curlwrapper_version());
     
       if (pthread_mutex_init(&lock_my_malloc, NULL) != 0) {
          
            syslog(LOG_ERR,"[notes_curl_easy_init] mutex init lock_my_malloc failed");
            return 0;
        }
        if (pthread_mutex_init(&lock_free, NULL) != 0) {
           syslog(LOG_ERR,"[notes_curl_easy_init] mutex init lock_free failed");
            return 0;
        }
        if (pthread_mutex_init(&lock_callback, NULL) != 0) {
            syslog(LOG_ERR,"[notes_curl_easy_init] mutex init lock_callback failed");
            return 0;
        }
     #ifdef CURLWRAPPER_DEBUG
     PrintNotesCurlStruct(10, "[>notes_curl_easy_init]");
#endif
     /*
       if(globalNotesCurlHandleList.NotesCurlHandles == NULL) {
            globalNotesCurlHandleList.NotesCurlHandles = my_malloc( sizeof(struct NotesCurlHandlestruct)*NumberOfNotesCurlHandles);
      }
     */
     
     if (globalNotesCurlHandleList.NotesCurlHandles != NULL ){
         //globalNotesCurlHandleList.number_of_entries = NumberOfNotesCurlHandles;
        for(unsigned long i=0; i<NO_ENT;i++) {
            #ifdef CURLWRAPPER_DEBUG
                syslog(LOG_ERR,"[notes_curl_easy_init] init=%li",i);
            #endif
            globalNotesCurlHandleList.NotesCurlHandles[i].active = 0; // Set all entries not "not active"
            globalNotesCurlHandleList.NotesCurlHandles[i].curlhandle = NULL;
            globalNotesCurlHandleList.NotesCurlHandles[i].return_buffer = NULL;
            globalNotesCurlHandleList.NotesCurlHandles[i].return_buffer_size=0;
        }
        localCURLhandle = curl_easy_init();
        if (localCURLhandle != NULL) 
        {
             rc_handle = create_notes_curl( localCURLhandle);    
        }
     } else {
        syslog(LOG_ERR,"[notes_curl_easy_init] Unable to malloc NotesCurlHandles");
        rc_handle=-1;
     }
    #ifdef CURLWRAPPER_DEBUG
          PrintNotesCurlStruct(10, "[<notes_curl_easy_init]");
    #endif
    return  rc_handle;
 }

 // Takes a pointer to NotesCurlHandlList and returns the NoteCurlHandle entry identified by the entry parameter,
// Returns NULL if entry is out of bounds.
 CURL *notes_get_native_curl_handle ( NotesCurlHandle entry){
    // struct NotesCurlHandleList * nCurlList = hlist; // use our own internal pointer so we do not risk misusing the original    
    CURL *rc_nativeCurlHandle = NULL; // our return value in with default of null in case we mess up.
    if (isNotesCurlHandleValid(entry) )
    {
        rc_nativeCurlHandle = globalNotesCurlHandleList.NotesCurlHandles[entry].curlhandle;
    }
    return rc_nativeCurlHandle;
}

 // userp is pointer to a NotesCURL.. entry. 
 /*
  content is the pointer to the chunk of content returned from the perform call
  size is the size of the specific sub chunk elements.
  nmemb is the size of the specific chunk expressed in size sizes.
  */
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp){
//    char * cp = (char *) contents; // Only for debugging
    pthread_mutex_lock(&lock_callback);
  #ifdef CURLWRAPPER_DEBUG
  syslog(LOG_DEBUG, "[WriteMemoryCallback] Mutex Lock callback");
#endif
  size_t realsize = size * nmemb;
  struct NotesCurlHandlestruct *mem = (struct NotesCurlHandlestruct *)userp;
  #ifdef CURLWRAPPER_DEBUG
    syslog(LOG_DEBUG, "[>WriteMemoryCallback] Content =%p, size=%zu, nmemb=%zu, userp=%p", contents, size, nmemb, userp);
#endif
  //char * rb1 =mem->return_buffer; //debug
  
  mem->return_buffer = my_realloc (mem->return_buffer,  mem->return_buffer_size+realsize + 1);
  //char * rb2 =mem->return_buffer; //debug
  
  #ifdef CURLWRAPPER_DEBUG
  syslog(LOG_DEBUG, "[WriteMemoryCallback] New buffer=%p", mem->return_buffer);
#endif
  
  if(mem->return_buffer == NULL) { /* out of memory! */ 
	syslog(LOG_ERR,"not enough memory (my_realloc returned NULL)");
	pthread_mutex_unlock(&lock_callback);
	syslog(LOG_DEBUG, "[WriteMemoryCallback] Mutex Unlock callback");
	return 0;
  }
  //Get the pointer to the return_buffer "return_buffer_size" into the buffer
  char * mm = &mem->return_buffer[mem->return_buffer_size]; //debug
  memcpy(&mem->return_buffer[mem->return_buffer_size], contents, realsize);
  mem->return_buffer_size += realsize;
  mem->return_buffer[mem->return_buffer_size] = 0; 
   
  

 // mem->return_buffer = &mem->return_buffer[mem->return_buffer_size];
  //printf("realsize=%zd\n",mem->return_buffer_size);
  //printf("Buffer=%s\n",mem->return_buffer);
pthread_mutex_unlock(&lock_callback);
 #ifdef CURLWRAPPER_DEBUG
  syslog(LOG_DEBUG, "[WriteMemoryCallback] Mutex Unlock callback");
#endif  
#ifdef CURLWRAPPER_DEBUG
  syslog(LOG_INFO,"[<WriteMemoryCallback] returning size=%zu",realsize);
#endif
  
  return realsize;
}

void PrintChunk( struct NotesCurlHandlestruct *content){  // Obs not used - only for debugging
    for (long i = 0; i < content->return_buffer_size ; i++)
    {
        printf("%c", content->return_buffer[i]);
    }
    printf("\n");
}
 
 extern char * notes_easy_curl_perform(NotesCurlHandle notes_curl_handle){
     char * rc_buffer = NULL;
     CURLcode curl_error;
#ifdef CURLWRAPPER_DEBUG
      syslog(LOG_NOTICE, "[>notes_easy_curl_perform]");
#endif
     //PrintNotesCurlStruct("easy_perform");
     
      //curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);/* send all data to this function  */ 
      // curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);/* we pass our 'chunk' struct to the callback function */ 
     if (isNotesCurlHandleValid(notes_curl_handle))
     { 
         if(globalNotesCurlHandleList.NotesCurlHandles[notes_curl_handle].return_buffer == NULL)
         { // If the buffer is not allocated at all, do an intial allocation of one byte
             globalNotesCurlHandleList.NotesCurlHandles[notes_curl_handle].return_buffer = my_malloc(1);
         }	 else {
	     globalNotesCurlHandleList.NotesCurlHandles[notes_curl_handle].return_buffer = my_realloc(globalNotesCurlHandleList.NotesCurlHandles[notes_curl_handle].return_buffer,1);
	     globalNotesCurlHandleList.NotesCurlHandles[notes_curl_handle].return_buffer_size=0;
         } 
	     
#ifdef CURLWRAPPER_DEBUG
	 syslog(LOG_DEBUG,"Pointer to result buffer: %p\n", (void *) &globalNotesCurlHandleList.NotesCurlHandles[notes_curl_handle] );
#endif
         curl_easy_setopt(globalNotesCurlHandleList.NotesCurlHandles[notes_curl_handle].curlhandle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
         curl_easy_setopt(globalNotesCurlHandleList.NotesCurlHandles[notes_curl_handle].curlhandle  , 
                             CURLOPT_WRITEDATA, 
                             (void *) &globalNotesCurlHandleList.NotesCurlHandles[notes_curl_handle]);
        char *url = NULL;
#ifdef CURLWRAPPER_DEBUG
        curl_easy_getinfo(globalNotesCurlHandleList.NotesCurlHandles[notes_curl_handle].curlhandle, CURLINFO_EFFECTIVE_URL, &url);
        if(url)
            syslog(LOG_DEBUG,"[<notes_easy_curl_perform] URL=%s", url); 
#endif
        
       curl_error = curl_easy_perform(globalNotesCurlHandleList.NotesCurlHandles[notes_curl_handle].curlhandle);
        if (curl_error == CURLE_OK) 
        {
            rc_buffer = globalNotesCurlHandleList.NotesCurlHandles[notes_curl_handle].return_buffer;
        }
     }
     //printf("rc_buffer:%s\n",rc_buffer);
 #ifdef CURLWRAPPER_DEBUG
     PrintNotesCurlStruct(10,"[notes_easy_curl_perform]");

     syslog(LOG_NOTICE, "[<notes_easy_curl_perform]");
#endif
     return rc_buffer;
 }

 
 void *my_malloc ( size_t numbytes ) {
	void *r_addr = NULL;
        #ifdef CURLWRAPPER_DEBUG
	syslog(LOG_DEBUG,"[my_malloc] request size=%zu", numbytes);
#endif
      
        r_addr = malloc( numbytes );
         #ifdef CURLWRAPPER_DEBUG
	syslog(LOG_DEBUG,"[my_malloc] new pointer at %p,",r_addr);
#endif
        return (r_addr); 
 }
 
 void *my_realloc( void *current, size_t size ) {
	void *r_addr = NULL;
#ifdef CURLWRAPPER_DEBUG
	syslog(LOG_DEBUG,"[my_realloc] old pointer at %p, size=%zu",current, size);
#endif
	r_addr = realloc( current,  size );
#ifdef CURLWRAPPER_DEBUG
	syslog(LOG_DEBUG,"[my_realloc] new pointer at %p,",r_addr);
#endif
	return(r_addr);
 }

 void my_free( void *region ){
	int rc = -1;
#ifdef CURLWRAPPER_DEBUG
        syslog(LOG_DEBUG,"[my_free] free pointer at %p,",region);
#endif
        free( region );
 }


// Only ever used if configuration is static 
 int main(int argc, char *argv[])
{
	//char * url = "www.google.com";
	char * url = argv[1];
	char * payload;
	
	NotesCurlHandle nch1 =-1;
            NotesCurlHandle nch2 =-1;
            CURL *nativeCURLhandle = NULL;
            char* rc_buffer = NULL;
#ifdef CURLWRAPPER_DEBUG
	printf("Running in debug mode\n");
#else
	printf("Running in normal mode\n");
#endif
	nch1 = notes_curl_easy_init();
            PrintNotesCurlStruct(2, "easy_perform0");
        	//nch2 = notes_curl_easy_init();
            //PrintNotesCurlStruct("easy_perform1");
	printf("libCurlWrapper version: %s\n", get_curlwrapper_version());    
	nativeCURLhandle = notes_get_native_curl_handle(nch1);
	for (int i=0; i++<1;){
            
		curl_easy_setopt(nativeCURLhandle, CURLOPT_URL, url);// specify URL to get  
		curl_easy_setopt(nativeCURLhandle, CURLOPT_FOLLOWLOCATION, 1);// Follow redirects 
		curl_easy_setopt(nativeCURLhandle, CURLOPT_HEADER, 1);// Include Header in result  
		rc_buffer = notes_easy_curl_perform(nch1 );
            }
            notes_curl_global_cleanup();
            PrintNotesCurlStruct(2, "easy_perform3");
            //notes_curl_easy_cleanup(nch2);
            //PrintNotesCurlStruct("easy_perform3");

	//printf("Asking for URL:%s\n", url );
//	payload = GetNumberOfBytes(url);
	//printf("Returned payload %s\n", rc_buffer);

  return 0;
}