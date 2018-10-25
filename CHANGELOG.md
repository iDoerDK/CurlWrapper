# ChangeLog file for CurlWrapper

# History
## [0.1.9] - 2018-10-25
### Added
 - Added debug statements if compiled with -DDEBUG
- Added syslog if #define CURLWRAPPER_DEBUG
### Changed
 - Fixed logic error in notes_curl_perform where buffer_size was never reset if the 
     notes_curl_handle was reused. Resulted in every call to notes_curl_perform to use 
     additional memory equal to to the size of the response message;
### Removed
- Removed old debug print statements

## [0.1.5] - 2018-06-06
### Added
 - Added debug statements if compiled with -DDEBUG
-  Added get_curlwrapper_version() to get verion of library in use
### Changed
 - Modified init of internal curl structure to ensure initial return buffer size is 0;
 
## [0.1.4] - 2018-04-10
### Added
 - Testing release for initial use
 - Released to Github

## [0.1.3] - 2018-04-06 - Unreleased
## [0.1.2] - 2018-03-22 - Unreleased
## [0.1.1] - 2018-03-20 - Unreleased
## [0.1.0] - 2018-02-13 - Unreleased
## [Initial version] - 2018-02-13 - Unreleased
