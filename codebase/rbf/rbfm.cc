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
    
    return -1;
}

RC RecordBasedFileManager::readRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const RID &rid, void *data) {
    return -1;
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

