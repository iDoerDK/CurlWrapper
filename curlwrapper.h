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

#define CURLWRAPPER_VERSION "0.1.15 debug"
#define CURLWRAPPER_LOGLEVEL "debug" // not yet used
#define CURLWRAPPER_DEBUG 1
    
typedef long NotesCurlHandle ;
pthread_mutex_t lock_my_malloc, lock_free, lock_callback;

/** NotesCurlVersion
    * .
    *  Returns the current NotesCurlWrapper as a string;
    *  @param none
    * @return char pointer to the returned string (char *)
    */
char * get_curlwrapper_version ();

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
extern void notes_curl_global_cleanup();
extern NotesCurlHandle  notes_curl_easy_init() ;
extern void notes_curl_easy_cleanup(NotesCurlHandle notesHandle);
extern  CURL *notes_get_native_curl_handle ( NotesCurlHandle entry);

 /*
  * Private Functions
  */
 
 void *my_malloc ( size_t numbytes ) ;
 void *my_realloc( void *current, size_t size ) ;
 void my_free( void *region );
 
#ifdef __cplusplus
}
#endif

#endif /* CURLWRAPPER_H */

