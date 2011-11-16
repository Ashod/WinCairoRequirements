/*
 * Copyright (c) 2008-2009 Brent Fulgham.  All rights reserved.
 *
 * This source code is a modified version of the CoreFoundation sources released by Apple Inc. under
 * the terms of the APSL version 2.0 (see below).
 *
 * For information about changes from the original Apple source release can be found by reviewing the
 * source control system for the project at https://sourceforge.net/svn/?group_id=246198.
 *
 */

//
// Apple's "Write a PList" example program.
// Taken from http://developer.apple.com/opensource/cflite.html
//
#include <CoreFoundation/CoreFoundation.h>

void propertyListExample (void);
void writePropertyListToFile (CFDataRef data);

const char* kFilename = "./schema.xml";

int main (int argc, const char * argv[]) {
    // Create and save the plist.
    propertyListExample ();

    return 0;
}

// This function will print the provided arguments (printf style varargs) out to the console.
// Note that the CFString formatting function accepts "%@" as a way to display CF types.
// For types other than CFString and CFNumber, the result of %@ is mostly for debugging
// and can differ between releases and different platforms.
void show(CFStringRef formatString, ...) {
    CFStringRef resultString;
    CFDataRef data;
    va_list argList;

    va_start(argList, formatString);
    resultString = CFStringCreateWithFormatAndArguments(NULL, NULL, formatString, argList);
    va_end(argList);

    data = CFStringCreateExternalRepresentation(NULL, resultString, CFStringGetSystemEncoding(), '?');

    if (data != NULL) {
        printf ("%.*s\n\n", (int)CFDataGetLength(data), CFDataGetBytePtr(data));
        CFRelease(data);
    }

    CFRelease(resultString);
}

void propertyListExample (void) {
    CFMutableDictionaryRef dict;
    CFNumberRef num;
    CFArrayRef array;
    CFDataRef data;
    #define NumKids 2
    CFStringRef kidsNames[] = { CFSTR ("John"), CFSTR ("Kyra") };
    #define NumPets 0
    int yearOfBirth = 1965;
    #define NumBytesInPic 10
    const unsigned char pic[ NumBytesInPic ] = { 0x3c, 0x42, 0x81, 0xa5, 0x81, 0xa5, 0x99, 0x81, 0x42, 0x3c };
    CFDataRef xmlPropertyListData;
    CFStringRef xmlAsString;

    // Create and populate a pretty standard mutable dictionary: CFString keys, CF type values.
    // To be written out as a "propertyList", the tree of CF types can contain only:
    //   CFDictionary, CFArray, CFString, CFData, CFNumber, and CFDate.
    // In addition, the keys of the dictionaries should be CFStrings.

    dict = CFDictionaryCreateMutable (NULL, 0, &kCFCopyStringDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks );

    CFDictionarySetValue (dict, CFSTR ("Name"), CFSTR ("John Doe"));

    CFDictionarySetValue (dict, CFSTR ("City of Birth"), CFSTR ("Springfield"));

    num = CFNumberCreate (NULL, kCFNumberIntType, &yearOfBirth);
    CFDictionarySetValue (dict, CFSTR ("Year Of Birth"), num);
    CFRelease (num);

    array = CFArrayCreate (NULL, (const void **)kidsNames, NumKids, &kCFTypeArrayCallBacks); 
    CFDictionarySetValue (dict, CFSTR ("Kids Names"), array);
    CFRelease (array);

    array = CFArrayCreate (NULL, NULL, 0, &kCFTypeArrayCallBacks);
    CFDictionarySetValue (dict, CFSTR ("Pets Names"), array );
    CFRelease (array);

    data = CFDataCreate (NULL, pic, NumBytesInPic);
    CFDictionarySetValue (dict, CFSTR ("Picture"), data);
    CFRelease (data);

    // We now have a dictionary which contains everything we want to know about
    // John Doe; let's show it first:
    show (CFSTR ("John Doe info dictionary:\n%@"), dict);

    // Now create a "property list", which is a flattened, XML version of the
    // dictionary:
    xmlPropertyListData = CFPropertyListCreateXMLData (NULL, dict);

   // The return value is a CFData containing the XML file; show the data

    show (CFSTR ("Shown as XML property list (bytes):\n%@"), xmlPropertyListData);

    // Given CFDatas are shown as ASCII versions of their hex contents, we can also
    // attempt to show the contents of the XML, assuming it was encoded in UTF8
    // (This is the case for XML property lists generated by CoreFoundation currently)

    xmlAsString = CFStringCreateFromExternalRepresentation (NULL, xmlPropertyListData, kCFStringEncodingUTF8);

    show (CFSTR ("The XML property list contents:\n%@"), xmlAsString);

    writePropertyListToFile (xmlPropertyListData);

    CFRelease (dict);
    CFRelease (xmlAsString);
    CFRelease (xmlPropertyListData);

    CFStringRef name = CFSTR("Brent");
    if (CFBundleRef bundle = CFBundleGetMainBundle ())
       if (CFTypeRef bundleExecutable = CFBundleGetValueForInfoDictionaryKey(bundle, kCFBundleExecutableKey))
          if (CFGetTypeID(bundleExecutable) == CFStringGetTypeID())
             name = reinterpret_cast<CFStringRef>(bundleExecutable);

    int value = 1;
    CFNumberRef numRef = CFNumberCreate(0, kCFNumberSInt8Type, &value);
    show (CFSTR ("The number was: %@"), numRef);
    CFRelease (numRef);
}

void writePropertyListToFile (CFDataRef data) {
    CFStringRef errorString;

    CFPropertyListRef propertyList = CFPropertyListCreateFromXMLData (NULL, data, kCFPropertyListMutableContainersAndLeaves, &errorString);

    if (errorString == NULL) {
        CFStringRef urlString = CFStringCreateWithCString (NULL, kFilename, CFStringGetSystemEncoding ());

        CFURLRef fileURL = CFURLCreateWithFileSystemPath (NULL, urlString, kCFURLPOSIXPathStyle, FALSE);

        CFWriteStreamRef stream = CFWriteStreamCreateWithFile (NULL, fileURL);

        Boolean isOpen = CFWriteStreamOpen (stream);

        show (CFSTR ("Property list (as written to file):\n%@"), propertyList);
  
        CFIndex bytesWritten = CFPropertyListWriteToStream (propertyList, stream, kCFPropertyListXMLFormat_v1_0, NULL);

        CFWriteStreamClose (stream);
    }
    else {
        CFShow (errorString);
        CFRelease (errorString);
    }

    CFRelease (propertyList);
}