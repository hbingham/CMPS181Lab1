#include "pfm.h"
#include <iostream>
#include <fstream>
#include <unistd.h>


PagedFileManager* PagedFileManager::_pf_manager = 0;

PagedFileManager* PagedFileManager::instance()
{
    if(!_pf_manager)
        _pf_manager = new PagedFileManager();

    return _pf_manager;
}


PagedFileManager::PagedFileManager()
{
}


PagedFileManager::~PagedFileManager()
{
}


RC PagedFileManager::createFile(const string &fileName)
{
    if(access(fileName.c_str(), F_OK ) == -1)
    {
       FILE *fp = fopen(fileName.c_str(), "w");
       fclose(fp);
       return 0;
    }
    return -1; //file already exists, so return error number
}


RC PagedFileManager::destroyFile(const string &fileName)
{
    if (remove(fileName.c_str()) == 0)
    {
       return 0;
    }
    return -1; //file already exists, so return error number
}


RC PagedFileManager::openFile(const string &fileName, FileHandle &fileHandle)
{
    if(access(fileName.c_str(), F_OK ) == 0)
    {
       FILE *fp = fopen(fileName.c_str(), "rw");
       fileHandle.fd = fp;
       return 0;
    }
    return -1;
}


RC PagedFileManager::closeFile(FileHandle &fileHandle)
{
    fclose(fileHandle.fd);
    return 0;
}


FileHandle::FileHandle()
{
	readPageCounter = 0;
	writePageCounter = 0;
	appendPageCounter = 0;
}


FileHandle::~FileHandle()
{
}


RC FileHandle::readPage(PageNum pageNum, void *data)
{
    return -1;
}


RC FileHandle::writePage(PageNum pageNum, const void *data)
{
    return -1;
}


RC FileHandle::appendPage(const void *data)
{
    return -1;
}


unsigned FileHandle::getNumberOfPages()
{
    return 0;
}


RC FileHandle::collectCounterValues(unsigned &readPageCount, unsigned &writePageCount, unsigned &appendPageCount)
{
	return -1;
}
