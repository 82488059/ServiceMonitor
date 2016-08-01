#pragma once
namespace dev{

int GetMacAddress(char *mac);

BOOL GetDiskSerial(char* pBuf, size_t bufSize);
BOOL HexStrToStr(char* hexStr, char*pBuf, size_t bufSize);
// int GetDiskSerial();
}
