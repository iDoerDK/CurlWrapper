/* 
 * File:   curlwrapper.h
 * Author: jarnt
 *
 * Created on February 13, 2018, 8:48 PM
 * Version 1.0 February 13, 2018
 * Version 1.1 March 20, 2018
 * Version 1.2 March 22, 2018
 * Version 1.3 April 6th 2018
 * Version 1.31 April 10th 2018
 */


#ifndef CURLWRAPPER_H
#define CURLWRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <curl/curl.h>
//#include "curlwrapper.h"

typedef unsigned long NotesCurlHandle ;

void PrintNotesCurlStruct (char * text);

unsigned char isNotesCurlHandleValid (NotesCurlHandle handle);

 extern void notes_curl_easy_cleanup(NotesCurlHandle notesHandle);

 extern void notes_curl_global_cleanup();
 
NotesCurlHandle create_notes_curl(CURL *curlhandle);

 extern NotesCurlHandle  notes_curl_easy_init() ;
 

extern  CURL *notes_get_native_curl_handle ( NotesCurlHandle entry);

/** notes_easy_curl_perform
    * 
    * Gets native CURL from CurlWrapper memory structure based on notes_curl_handle
    * Calls native easy_curl_perfom with provided url;
    * expands CurlMemory strucuture for notes_curl_handle) to hold returned data
    * returns pointer to returned data
    *  @param url_to_fetch - pointer to string with valid url
    * @return char pointer to the returned string (raw)
    */
 extern char * notes_easy_curl_perform(NotesCurlHandle notes_curl_handle);
 
 
#ifdef __cplusplus
}
#endif

#endif /* CURLWRAPPER_H */

