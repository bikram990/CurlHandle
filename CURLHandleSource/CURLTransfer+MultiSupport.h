//
//  CURLTransfer+MultiSupport.h
//  CURLHandle
//
//  Created by Sam Deane on 27/03/2013.
//  Copyright (c) 2013 Karelia Software. All rights reserved.

#import "CURLTransfer.h"


/**
 Private API used by CURLMulti.
 Not exported in the framework, and not recommended for general use.
 */

@interface CURLTransfer(MultiSupport)

/** @name Internal Methods */

/**
 The CURL handle managed by this object.

 @warning Not intended for general use.

 @return The curl handle.

 */

- (CURL*)curlHandle;

/**
 Called by <CURLMulti> to tell the transfer that it has completed.
 
 @param code The completion code.
 
 @warning Not intended for general use.

 */

- (void)completeWithCode:(CURLcode)code;

/**
 Called by <CURLMulti> to tell the transfer that it has completed.
 
 @param error The failure error if there was one.
 
 @warning Not intended for general use.
 
 */

- (void)completeWithError:(NSError *)error;

/**
 Has the transfer completed?
 
 @return YES if the transfer has completed.
 
 @warning Not intended for general use.

 */

- (BOOL)hasCompleted;

@end

