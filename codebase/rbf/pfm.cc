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
       unsigned int pageCount = 0;
       FILE *fp = fopen(fileName.c_str(), "w+b");
       fwrite(&pageCount, sizeof(unsigned), 1, fp);
       fseek(fp, 0, SEEK_SET);
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
    return -1; //file doesnt exist, so return error number
}


RC PagedFileManager::openFile(const string &fileName, FileHandle &fileHandle)
{
    if(access(fileName.c_str(), F_OK ) == 0)
    {
       FILE *fp = fopen(fileName.c_str(), "r+b");
       fileHandle.fd = fp;
       return 0;
    }
    return -1; //couldnt open file for some reason
}


RC PagedFileManager::closeFile(FileHandle &fileHandle)
{
    if (fclose(fileHandle.fd) == 0)
{
        return 0;
}
    return -1;
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
    fseek(fd, 0, SEEK_END);
    fwrite(data, sizeof(char), PAGE_SIZE, fd);
    incPageCount();
    appendPageCounter++;
    return 0;
}


unsigned FileHandle::getNumberOfPages()
{
    fseek(fd, 0, SEEK_SET);
    unsigned int pageNum;
    fread(&pageNum, sizeof(unsigned), 1, fd);
    return pageNum;
}


RC FileHandle::collectCounterValues(unsigned &readPageCount, unsigned &writePageCount, unsigned &appendPageCount)
{
    readPageCount = readPageCounter;
    writePageCount = writePageCounter;
    appendPageCount = appendPageCounter;
    return 0;
}



RC FileHandle::incPageCount()
{
    fseek(fd, 0, SEEK_SET);
    unsigned int pageCount;
    fread(&pageCount, sizeof(unsigned), 1, fd);
    pageCount++;
    fseek(fd, 0, SEEK_SET);
    fwrite(&pageCount, sizeof(unsigned), 1, fd);
    return 0;
}
