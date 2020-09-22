#ifndef CEXCEPTION_H
#define CEXCEPTION_H

enum
{
    STDEX_FILE_NOT_FOUND,
    STDEX_PATH_NOT_FOUND
};

class CException
{
    public:

        CException()
        {
            mErrnum = 0;
            mErrstr = NULL;
        }


        CException(int n) : mErrnum(n)
        {
            mErrstr = NULL;
        }


        CException(int n, const char* et) : mErrnum(n), mErrstr(et)
        { }


        int mErrnum;
        const char* mErrstr;
};

#endif
