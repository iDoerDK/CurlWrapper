# CurlWrapper

(C) iDoer Aps, Jens Arnt, jens@idoer.dk

This codes is released under the Apache License Version 2.0 (APLv2)

CURL Wrapper

Attribution and motivation

This code has been procured and co-sponsored by Semaphor (www.semaphor.dk). Origiginal motivation was to enable the use of the CURL api from IBM Lotus Domino (Lotus script)  on Linux

High level description

Most of the functions in this library are wrappers around the native CURL API easy_* functions.

This library hides the callback elements of using curl_easy_perform from the calling program. In effects this makes the call to the wrapped curl_easy_perform as syncroneus/blocking call returning the html response as a string (char *)

You will most likely use this code if the following is true
* You do not need/want to deal with the callback elements of the CURL API
* You do not want to deal with memory allocation and buffer management for the CURL API (curl_easy_* functions)
* You just want the html response from curl_easy_perfom as a string (char *)

Function description

This code is a wrapper around the CURL API for managing callbacks and memory allocation for html responses and the return string (char *).

curl_easy_init is being wrapped and initializes an internal memory structure to capture responses from the curl_easy_perfom call. 

curl_easy_perform is being wrapped and initializes (or reinitializes) the return bufffer for the html response.

curl_easy_cleanup is being wrapped and releases any memory being allocated during curl_easy_init and curl_easy_perform.

Known bugs and limitations

* Only some of the CURL API is being wrapped as needed by Sempahor
* Error checking is limited
* Error responses are going to stdout and will most likely not be visible from the calling program.

ToDo

Code is release as is and there are no further actions planned on it. 
