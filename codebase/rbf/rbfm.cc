#include "rbfm.h"
#include "math.h"
#include "string.h"

RecordBasedFileManager* RecordBasedFileManager::_rbf_manager = 0;
PagedFileManager* pfm = PagedFileManager:: instance();

//Checks the null byte(s), returns 1 if null, 0 if not
int checkNull(char* data,int fieldNumber)
{
   int theByte = floor((double) fieldNumber /8);
   char nullByte = data[theByte*8];
   int nullVal = (nullByte >> fieldNumber%8) & 0x01;
//   printf("Nullbit: %d\n", nullVal);

   return nullVal;
}

int makePage(FileHandle &fileHandle)
{
    int zero = 0;
    char* prepData = (char*)malloc(PAGE_SIZE * sizeof(char));
    memcpy(prepData + PAGE_SIZE - sizeof(int), &zero, sizeof(int));
    memcpy(prepData + PAGE_SIZE - sizeof(int) * 2, &zero, sizeof(int));
    fileHandle.appendPage(prepData);
    return 0;
}

RecordBasedFileManager* RecordBasedFileManager::instance()
{
    if(!_rbf_manager)
        _rbf_manager = new RecordBasedFileManager();

    return _rbf_manager;
}

RecordBasedFileManager::RecordBasedFileManager()
{
}

RecordBasedFileManager::~RecordBasedFileManager()
{
}

RC RecordBasedFileManager::createFile(const string &fileName) {
    return pfm-> createFile(fileName);
}

RC RecordBasedFileManager::destroyFile(const string &fileName) {
    return pfm->destroyFile(fileName);
}

RC RecordBasedFileManager::openFile(const string &fileName, FileHandle &fileHandle) {
    return pfm->openFile(fileName, fileHandle);
}

RC RecordBasedFileManager::closeFile(FileHandle &fileHandle) {
    return pfm->closeFile(fileHandle);
}

RC RecordBasedFileManager::insertRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const void *data, RID &rid) {
    unsigned pageCount = fileHandle.getNumberOfPages();
    int slotCount = 0;
    int freeStart = 0;
    if (pageCount == 0) makePage(fileHandle);//if no pages exist, make a page
//read in page from file
    char* modPage = (char*)malloc(PAGE_SIZE * sizeof(char));
    fileHandle.readPage(pageCount-1, modPage);
//get slotcount and freespace from page
    memcpy(&slotCount, modPage + PAGE_SIZE - sizeof(int) * 2, sizeof(int));
    memcpy(&freeStart, modPage + PAGE_SIZE - sizeof(int), sizeof(int));

    int fieldCount = recordDescriptor.size();
    int nullOffset = ceil((double) fieldCount/8);
    char* dataCopy = (char*)data;

    char* record = (char*)malloc(PAGE_SIZE * sizeof(char));
    memcpy(record, &dataCopy[0], sizeof(char) * nullOffset);
    int recordLen = nullOffset;
    for(int i = 0; i < fieldCount; i++)
{
       if (checkNull(dataCopy, i) ==0)
       {
       switch(recordDescriptor[i].type)
       {
          case TypeInt: {
             memcpy(record + recordLen, &dataCopy[recordLen], sizeof(int));
             recordLen += sizeof(int);
             break;
          }
          case TypeReal: {
             memcpy(record + recordLen, &dataCopy[recordLen], sizeof(int));
             recordLen += sizeof(float);
             break;
          }
          case TypeVarChar: {
             int length;
             memcpy(&length, &dataCopy[recordLen], sizeof(int));
             memcpy(record + recordLen, &dataCopy[recordLen], sizeof(int));
             recordLen += sizeof(int);
             char* varChar = new char[length + 1];
             memcpy(record + recordLen, &dataCopy[recordLen], length);
             varChar[length] = '\0';
             recordLen += length;
             break;
          }
       }
       }
}


    int freeSpace = PAGE_SIZE - (sizeof(int)*2 + sizeof(int)*(slotCount + 1) + freeStart);
    if(recordLen > freeSpace)//if there is not enough space on the page, make a new one
{
       makePage(fileHandle);
       pageCount++;
       fileHandle.readPage(pageCount-1, modPage);
       slotCount = 0;
       freeStart = 0;
}
    slotCount++;
    memcpy(modPage + freeStart, record, recordLen); //copy data
//following line copies data offset to slot
    memcpy(modPage + PAGE_SIZE - ((sizeof(int)*2) + (sizeof(int)*slotCount)), &freeStart, sizeof(int));

    freeStart+= recordLen;
    memcpy(modPage + PAGE_SIZE - sizeof(int), &freeStart, sizeof(int));
    memcpy(modPage + PAGE_SIZE - sizeof(int) * 2, &slotCount, sizeof(int));
    fileHandle.writePage(pageCount -1, modPage);

    rid.pageNum = pageCount;
    rid.slotNum = slotCount;
    return 0;
}

RC RecordBasedFileManager::readRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const RID &rid, void *data) {
    int fieldCount = recordDescriptor.size();
    int offset = ceil((double) fieldCount/8);
    unsigned pageCount = fileHandle.getNumberOfPages();
    int recStart = 0;
    int recEnd = 0;
    int slotCount = 0;


    char* modPage = (char*)malloc(PAGE_SIZE * sizeof(char));
    fileHandle.readPage(rid.pageNum, modPage);
    memcpy(&recStart, modPage + PAGE_SIZE - (sizeof(int) * (2+rid.slotNum)), sizeof(int));
    memcpy(&slotCount, modPage + PAGE_SIZE - sizeof(int) * 2, sizeof(int));

    if(slotCount == rid.slotNum)
{
       memcpy(&recEnd, modPage + PAGE_SIZE - sizeof(int), sizeof(int));
}
    else
{
       memcpy(&recEnd, modPage + PAGE_SIZE - (sizeof(int) * (3+rid.slotNum)), sizeof(int));
}
    printf("Recstart: %d recend: %d\n", recStart, recEnd);
    memcpy(&data, modPage + recStart, recEnd-recStart);
    return 0;
}

RC RecordBasedFileManager::printRecord(const vector<Attribute> &recordDescriptor, const void *data) {
    int fieldCount = recordDescriptor.size();
    int offset = ceil((double) fieldCount/8);
    char* dataCopy = (char*)data;
    printf("Field Count: %d\n", fieldCount);
    for(int i = 0; i < fieldCount; i++)
{
       if (checkNull(dataCopy, i) ==0)
       {
       switch(recordDescriptor[i].type)
       {
          case TypeInt: {
             int fieldData;
             memcpy(&fieldData, &dataCopy[offset], sizeof(int));
             printf("%s: %d    ", recordDescriptor[i].name.c_str(), fieldData);
             offset += sizeof(int);
             break;
          }
          case TypeReal: {
             float fieldData;
             memcpy(&fieldData, &dataCopy[offset], sizeof(int));
             printf("%s: %f    ", recordDescriptor[i].name.c_str(), fieldData);
             offset += sizeof(float);
             break;
          }
          case TypeVarChar: {
             int length;
             memcpy(&length, &dataCopy[offset], sizeof(int));
             offset += sizeof(int);
             char* varChar = new char[length + 1];
             memcpy(varChar, &dataCopy[offset], length);
             varChar[length] = '\0';
             printf("%s: %s    ", recordDescriptor[i].name.c_str(), varChar);
             offset += length;
             break;
          }
       }
       }
       else
       {
          printf("%s: NULL    ", recordDescriptor[i].name.c_str());
       }
}
    printf("\n");
    return 0;
}

