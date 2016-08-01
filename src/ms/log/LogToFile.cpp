#include "stdAfx.h"
#include "ms/log/LogToFile.h"
// #include <boost/log/common.hpp>
#include <ms/path/path.h>
#include <windows.h>
#include <fstream>
#include <iostream>
namespace ms{
namespace mslog{
    using namespace std;
    CLogToFile::CLogToFile(const tstring& szFilePath, const tstring& szLogFileName)
            : m_szFilePath()
            , m_szLogFileName(szLogFileName)
            , init_(false)
        {
            Init();
        }
        CLogToFile::CLogToFile()
            : m_szFilePath()
            , m_szLogFileName()
            , init_(false)
        {
            Init();
        }

        CLogToFile::~CLogToFile(void)
        {
        }

        bool CLogToFile::Init()
        {
            if (!init_)
            {
                if (m_szLogFileName.empty())
                {
                    m_szLogFileName = _T("AppError.Log");
                }

                if (m_szFilePath.empty ())
                {
                    m_szFilePath = ms::path::Path::GetInstance()->GetExecutePath();;
                }
                init_ = true;
            }

            return init_;
        }

        bool CLogToFile::operator()(const tstring& szLog, const std::string& szFileName, int line, int logType)
        {
            if (ErrorLog != logType) // 强制记录错误日志
            {
                //ChechLog();
            }
            TCHAR filename[MAX_PATH] = { 0 };
            size_t index = szFileName.find_last_of("\\");
            for (size_t i = index + 1, j = 0; i < szFileName.size(); ++j, ++i)
            {
                filename[j] = szFileName[i];
            }
            tstring strLOGPath, strLOGFilePath, strLogCov;
            SYSTEMTIME time;
            GetLocalTime(&time);
            TCHAR BUF[256];
            _stprintf_s(BUF, _T("%04d-%02d-%02d"), time.wYear, time.wMonth, time.wDay);
            tstring TimePath = BUF;
            _stprintf_s(BUF, _T("[%02d:%02d:%02d]"), time.wHour, time.wMinute, time.wSecond);
            tstring strTime = BUF;
            tstring szLogTitle;
            _stprintf_s(BUF, _T("[FileName: %s][Line: %d]"), filename, line);
            szLogTitle = BUF;

            tstring FileName;

            strLogCov = m_szFilePath + _T("LOG");


            strLOGPath = strLogCov + _T("\\") + TimePath;
            strLOGFilePath = strLOGPath + _T("\\") + m_szLogFileName;

            if (GetFileAttributes((LPCTSTR)strLogCov.c_str()) == 0xffffffff)
            {
                CreateDirectory((LPCTSTR)strLogCov.c_str(), NULL);
            }
            if (GetFileAttributes((LPCTSTR)strLOGPath.c_str()) == 0xffffffff)
            {
                CreateDirectory((LPCTSTR)strLOGPath.c_str(), NULL);
            }

            return Wrtie (strLOGFilePath, strTime, szLogTitle, szLog);
        }

        bool CLogToFile::Wrtie(const tstring& strLOGFilePath, const tstring& strTime, const tstring& szLogTitle, const tstring& szLog)
        {
            HANDLE hFile = ::CreateFile(strLOGFilePath.c_str()
                , GENERIC_WRITE
                , 0
                , NULL
                , OPEN_ALWAYS
                , FILE_ATTRIBUTE_ARCHIVE
                , NULL
                );
            if (hFile == INVALID_HANDLE_VALUE)
            {
                return false;
            }
            BOOL bRet = FALSE;
            DWORD leng = 0;
            leng = GetFileSize(hFile, NULL);
            SetFilePointer(hFile, leng, NULL, FILE_BEGIN);
            DWORD dw = 0;
            static ms::tstring space = ms::tstring(_T(" "));
            static ms::tstring end = ms::tstring(_T("\r\n"));
            bRet = ::WriteFile(hFile, strTime.c_str(), strTime.length()*sizeof(TCHAR), &dw, NULL);
            bRet = ::WriteFile(hFile, szLogTitle.c_str(), szLogTitle.length()*sizeof(TCHAR), &dw, NULL);
            bRet = ::WriteFile(hFile, space.c_str(), space.length()*sizeof(TCHAR), &dw, NULL);
            bRet = ::WriteFile(hFile, szLog.c_str(), szLog.length()*sizeof(TCHAR), &dw, NULL);
            bRet = ::WriteFile(hFile, end.c_str(), end.length()*sizeof(TCHAR), &dw, NULL);
            ::CloseHandle(hFile);

            return !!bRet;
        }

        bool CLogToFile::ToWriteLog(const tstring& szLog, const std::string& szFileName, int line, int logType)
        {
            return (*this)(szLog, szFileName, line, logType);
        }
}
}