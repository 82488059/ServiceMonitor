#pragma once
#include <string>
#include <ms/Sington.hpp>
#include <ms/tstring.h>
/////////////////////////////////////////////////////////////////////
// ����: 
// ����: zt
// ����: ����������־
// ��Ҫ������
// ����: 2015.03.10
// �汾: 1.0
// �޸�:
/////////////////////////////////////////////////////////////////////
namespace ms{
namespace mslog{

    class CLogToFile
    {
    private:
        tstring m_szFilePath;
        tstring m_szLogFileName;

    public:
        bool Init();
        CLogToFile();
        CLogToFile(const tstring& szFilePath, const tstring& szLogFileName = _T("AppError.Log"));
        ~CLogToFile(void);
        enum log{ ErrorLog = 0, GeneralLog = 1, StartLog = 2 };
        bool ToWriteLog(const tstring& szLog, const std::string& szFileName, int line, int logType = GeneralLog);
        bool operator()(const tstring& szLog, const std::string& szFileName, int line, int logType = ErrorLog);
    private:
        bool Wrtie(const tstring& strLOGFilePath, const tstring& strTime, const tstring& szLogTitle, const tstring& szLog);

    private:
        bool init_{false};
        CLogToFile(const CLogToFile&) = delete;
        void operator = (const CLogToFile&) = delete;
    };
}
}

typedef ms::Sington<ms::mslog::CLogToFile> LogToFile;
#define LogInstance (*(LogToFile::GetInstance()))

#define AppErrorLogFormat(format, ...)\
    {\
         TCHAR log_format_buf_1024[1024];\
         _stprintf_s (log_format_buf_1024, format, __VA_ARGS__);\
         AppErrorLog(log_format_buf_1024);\
    }
#define AppLogFormat(T, format, ...)\
    {\
         TCHAR log_format_buf_1024[1024];\
         _stprintf_s (log_format_buf_1024, format, __VA_ARGS__);\
         AppLog(log_format_buf_1024, T);\
    }

#ifdef _DEBUG


#include <iostream>
#ifdef _UNICODE
#define logtofile_debug_out std::wcout
#else
#define logtofile_debug_out std::cout
#endif // _UNICODE


// ��־
#define AppLog(T1, T2) std::cout << __FILE__<< " line: " << __LINE__ << " " ; logtofile_debug_out << (T1) << std::endl;
// ������־
#define AppErrorLog(T)  std::cout << __FILE__<< " line: " << __LINE__ << " " ; logtofile_debug_out <<(T) << std::endl;
// ������־
#define AppGeneralLog(T)  std::cout << __FILE__<< " line: " << __LINE__ << " " ; logtofile_debug_out << (T) << std::endl;
// �������
#define DebugPrint(format, ...)\
    {\
         TCHAR log_format_buf_1024[1024];\
         _stprintf_s (log_format_buf_1024, format, __VA_ARGS__);\
         logtofile_debug_out << (log_format_buf_1024) << std::endl;\
    }

#else
// ��־
#define AppLog(T1, T2) (*(LogToFile::GetInstance()))((T1), __FILE__, __LINE__, T2)
// ������־
#define AppErrorLog(T) (*(LogToFile::GetInstance()))((T), __FILE__, __LINE__, ms::mslog::CLogToFile::ErrorLog)
// ������־
#define AppGeneralLog(T) (*(LogToFile::GetInstance()))((T), __FILE__, __LINE__, ms::mslog::CLogToFile::GeneralLog)

#define DebugPrint(format, ...) 

#endif
