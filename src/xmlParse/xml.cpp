//
//  xml.cpp
//  pathTrace
//
//  Created by HENRY BERGIN on 6/3/17.
//
//

#include "xml.h"

using namespace std;


void xmlParse::parseFile(string file_path)
{
    m_file_path = file_path;
    xmlFile.open(file_path);
    
}