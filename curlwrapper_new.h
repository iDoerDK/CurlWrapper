/* 
 * File:   curlwrapper.h
 * Author: Jens Arnt, jens@idoer.dk, iDoer Aps
 *
 * Version 1.2 March 22. 2018
 * Version 1.1 March 20, 2018
 * Version 1.0 February 13, 2018
 * Created on March 11, 2018, 2:08 PM
 */

#ifndef CURLWRAPPER_H
#define CURLWRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

     /** Notes_curl_global_init
     * Initializes the CURL library by calling the native curl_global_init()
     * Initializes the CurlWrapper memory structure and initializes everything with NULL
     * Does not allocate anything if CURLcode from native curl_global_init != 0
     * @returns CURLcode 
     */
    extern CURLcode notes_curl_global_init();
   
    /** notes_curl_easy_init
    * Calls native curl_easy_init
     * expands the CurlWrapper memory structure to hold a another struct with 
     * -- native CURL  pointer (from calling the native curl_easy_init) 
     * -- memory array for the return string 
    * @returns NotesCurlHandle
    */
    extern NotesCurlHandle  notes_curl_easy_init();
   
   /** notes_easy_curl_perform
    * 
    * Gets native CURL from CurlWrapper memory structure based on notes_curl_handle
    * Calls native easy_curl_perfom with provided url;
    * expands CurlMemory strucuture for notes_curl_handle) to hold returned data
    * returns pointer to returned data
    *  @param url_to_fetch - pointer to string with valid url
    * @return char pointer to the returned string (raw)
    */
    extern char * notes_easy_curl_perform(NotesCurlHandle notes_curl_handle, char * url_to_fetch);
    
    /** notes_get_native_curl_handle 
     * 
     * Note: use in case you want to do something with stuff like the native curl_easy_setopt()
     * @param notes_curl_handle
     * @returns the native CURL from the Curl memory strucure based n the notes_curl_handle
     */
    extern CURL *notes_get_native_curl_handle ( NotesCurlHandle notes_curl_handle);
    
    /** notes_easy_curl_cleanup
     * 
     * Calls the native easy_curl_cleanup
     * Frees the curl wrapper memory for the return data for the notes_curl_handle
     * invalidates the notes_curl_handle
     * @param notes_curl_handle
     * @returns nothing
     */
    extern void notes_easy_curl_cleanup ( NotesCurlHandle notes_curl_handle);
   
    /** notescurl_global_cleanup
     * 
     * Iterates over the Curl wrapper memory structure and frees any memory not yet free'd
     * Calls the native curl_global_cleanup
     */
    extern void notes_curl_global_cleanup();

#ifdef __cplusplus
}
#endif

#endif /* CURLWRAPPER_H */

