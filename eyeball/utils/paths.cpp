#include "paths.h"
#include "string.h"

const char* Paths::getFileName(const char* path)
{
  const char* fileName = path + strlen(path);
  while (fileName != path && *fileName != '\\' && *fileName != '/' && --fileName);
  
  return fileName != path ? ++fileName : fileName;
}