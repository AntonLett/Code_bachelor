//------------------------------------------------------------------------------
// File:        example3.cpp
//
// Description: Example to read or write metadata from files
//
// Syntax:      example3 [OPTIONS] FILE [FILE...]
//
// Options:     Any single-argument options supported by the exiftool
//              application, including options of the form -TAG=VALUE to write
//              information to the specified file(s).
//
// License:     Copyright 2013-2019, Phil Harvey (philharvey66 at gmail.com)
//
//              This is software, in whole or part, is free for use in
//              non-commercial applications, provided that this copyright notice
//              is retained.  A licensing fee may be required for use in a
//              commercial application.
//
// Created:     2013-11-23 - Phil Harvey
//------------------------------------------------------------------------------

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ExifTool.h"

using namespace std;

int main(int argc, char **argv)
{
    // if (argc < 2) {
    //     cout << "Example1: Read metadata from an image." << endl;
    //     cout << "Please specify input file name" << endl;
    //     return 1;
    // }

    ExifTool *et = new ExifTool();
    int cmdNum = et->ExtractInfo("/home/lettowh/Downloads/DSCN0010.jpg", "-a\n-s");
    
    TagInfo *info = et->GetInfo(cmdNum, 0.5); 

     if (info) {
        // print returned information
        for (TagInfo *i=info; i; i=i->next) {
            cout << i->name << " = " << i->value << endl;
        }
        // we are responsible for deleting the information when done
        delete info;
    } else if (et->LastComplete() <= 0) {
        cerr << "Error executing exiftool!" << endl;
    }
    // print exiftool stderr messages
    char *err = et->GetError();
    if (err) cout << err;
    delete et;      // delete our ExifTool objec
}

// end
