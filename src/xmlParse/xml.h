//
//  xml.h
//  pathTrace
//
//  Created by HENRY BERGIN on 6/3/17.
//
//

#ifndef xml_h
#define xml_h

#include <iostream>

class xmlParse
{
public:
    void parseFile(string file_path);
private:
    string m_file_path;
};

#endif /* xml_h */
