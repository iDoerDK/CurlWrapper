/* 
 * File:   curlwrapper.h
 * Author: jarnt
 *
 *See CHANGELOG.md for history
 */


#ifndef CURLWRAPPER_H
#define CURLWRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <curl/curl.h>
//#include "curlwrapper.h"

#define CURLWRAPPER_VERSION "0.1.6"
    
typedef unsigned long NotesCurlHandle ;
pthread_mutex_t lock_malloc, lock_free;

/** NotesCurlVersion
    * .
    *  Returns the current NotesCurlWrapper as a string;
    *  @param none
    * @return char pointer to the returned string (char *)
    */
char * get_curlwrapper_version ();

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

