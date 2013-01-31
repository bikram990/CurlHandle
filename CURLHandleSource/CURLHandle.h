//
//  CURLHandle.h
//
//  Created by Dan Wood <dwood@karelia.com> on Fri Jun 22 2001.
//  This is in the public domain, but please report any improvements back to the author.

#import <Foundation/Foundation.h>
#import <curl/curl.h>


#ifndef CURLHandleLog
#define CURLHandleLog(...) // no logging by default - to enable it, add something like this to the prefix: #define CURLHandleLog NSLog
#endif

@class CURLMulti;

@protocol CURLHandleDelegate;

extern NSString * const CURLcodeErrorDomain;
extern NSString * const CURLMcodeErrorDomain;
extern NSString * const CURLSHcodeErrorDomain;

@interface CURLHandle : NSObject
{
	CURL                    *_curl;                         /*" Pointer to the actual CURL object that does all the hard work "*/
    NSURL                   *_URL;
	id <CURLHandleDelegate> _delegate;
	char                    _errorBuffer[CURL_ERROR_SIZE];	/*" Buffer to hold string generated by CURL; this is then converted to an NSString. "*/
    BOOL                    _executing;                     // debugging
	NSMutableData           *_headerBuffer;                 /*" The buffer that is filled with data from the header as the download progresses; it's appended to one line at a time. "*/
    struct curl_slist       *_httpHeaders;
    struct curl_slist       *_preQuoteCommands;
    struct curl_slist       *_postQuoteCommands;
	NSDictionary            *_proxies;                      /*" Dictionary of proxy information; it's released when the handle is deallocated since it's needed for the transfer."*/
    NSInputStream           *_uploadStream;
}

//  Loading respects as many of NSURLRequest's built-in features as possible, including:
//  
//    * An HTTP method of @"HEAD" turns on the CURLOPT_NOBODY option, regardless of protocol (e.g. handy for FTP)
//    * Similarly, @"PUT" turns on the CURLOPT_UPLOAD option (again handy for FTP uploads)
//  
//    * Supply -HTTPBody or -HTTPBodyStream to switch Curl into uploading mode, regardless of protocol
//  
//    * Custom Range: HTTP headers are specially handled to set the CURLOPT_RANGE option, regardless of protocol in use
//      (you should still construct the header as though it were HTTP, e.g. bytes=500-999)
//  
//    * Custom Accept-Encoding: HTTP headers are specially handled to set the CURLOPT_ENCODING option
//
//  Delegate messages are delivered on an arbitrary thread; you should bounce over a specific thread if required for thread safety, or doing any significant work
//
//  Redirects are *not* automatically followed. If you want that behaviour, NSURLConnection is likely a better match for your needs
- (id)initWithRequest:(NSURLRequest *)request credential:(NSURLCredential *)credential delegate:(id <CURLHandleDelegate>)delegate multi:(CURLMulti*)multi;

// Can be called from any thread. Causes -loadRequest:err_delegateor: as soon as it can
- (void)cancel;

- (NSString *)initialFTPPath;    // CURLINFO_FTP_ENTRY_PATH. Only suitable once handle has finished
+ (NSString *)curlVersion;
- (NSString*)nameForType:(curl_infotype)type;

@end

#pragma mark - RunLoop Support

@interface CURLHandle(MultiSupport)
- (void)completeWithMultiCode:(CURLMcode)code;
- (void)completeWithCode:(CURLcode)code;
- (BOOL)hasCompleted;
- (BOOL)isCancelled;
@end

#pragma mark - Old API

@interface CURLHandle(OldAPI)

// Please don't use this unless you have to!
// To use, -init a handle, and then call this method, as many times as you like. Delegate messages will be delivered fairly normally during the request
- (void)sendSynchronousRequest:(NSURLRequest *)request credential:(NSURLCredential *)credential delegate:(id <CURLHandleDelegate>)delegate;

- (CURL *) curl;
+ (void) setProxyUserIDAndPassword:(NSString *)inString;
+ (void) setAllowsProxy:(BOOL) inBool;
@end

#pragma mark - Delegate

@protocol CURLHandleDelegate <NSObject>

- (void)handle:(CURLHandle *)handle didReceiveData:(NSData *)data;

@optional
- (void)handle:(CURLHandle *)handle didReceiveResponse:(NSURLResponse *)response;
- (void)handleDidFinish:(CURLHandle *)handle;

// Where possible errors are in NSURLErrorDomain or NSCocoaErrorDomain. There will generally be a CURLcodeErrorDomain error present; either directly, or as an underlying error (KSError <https://github.com/karelia/KSError> is handy for querying underlying errors)
// The key CURLINFO_RESPONSE_CODE (as an NSNumber) will be filled out with HTTP/FTP status code if appropriate
// At present all errors include NSURLErrorFailingURLErrorKey and NSURLErrorFailingURLStringErrorKey if applicable even though the docs say "This key is only present in the NSURLErrorDomain". Should we respect that?
- (void)handle:(CURLHandle*)handle didFailWithError:(NSError*)error;

// Reply to tell CURLHandle/libcurl how to handle the fingerprint
// If not implemented, only matching keys are accepted; all else is rejected
// I've found that CURLKHSTAT_FINE_ADD_TO_FILE only bothers appending to the file if not already present
- (enum curl_khstat)handle:(CURLHandle *)handle didFindHostFingerprint:(const struct curl_khkey *)foundKey knownFingerprint:(const struct curl_khkey *)knownkey match:(enum curl_khmatch)match;

// When sending data to the server, this reports just before it goes out on the wire. Reports a length of 0 when the end of the data is written so you can get a nice heads up that an upload is about to complete
- (void)handle:(CURLHandle *)handle willSendBodyDataOfLength:(NSUInteger)bytesWritten;

- (void)handle:(CURLHandle *)handle didReceiveDebugInformation:(NSString *)string ofType:(curl_infotype)type;

@end

#pragma mark - Error Domains

extern NSString * const CURLcodeErrorDomain;
extern NSString * const CURLMcodeErrorDomain;
extern NSString * const CURLSHcodeErrorDomain;



