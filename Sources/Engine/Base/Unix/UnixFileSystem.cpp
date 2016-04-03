/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

/* rcg10142001 Implemented. */


// !!! FIXME: rcg10142001 This should really be using CTStrings...


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/param.h>
#include <unistd.h>

#include <Engine/Engine.h>
#include <Engine/Base/FileSystem.h>

ENGINE_API CFileSystem *_pFileSystem = NULL;


class CUnixFileSystem : public CFileSystem
{
public:
    CUnixFileSystem(const char *argv0, const char *gamename);
    virtual ~CUnixFileSystem(void);
    virtual void GetExecutablePath(char *buf, ULONG bufSize);
    virtual void GetUserDirectory(char *buf, ULONG bufSize);
    virtual CDynamicArray<CTString> *FindFiles(const char *dir,
                                               const char *wildcard);
protected:
    char *exePath;
    char *userDir;
};


const char *CFileSystem::GetDirSeparator(void)
{
    return("/");
}


BOOL CFileSystem::IsDummyFile(const char *fname)
{
    return( (strcmp(fname, ".") == 0) || (strcmp(fname, "..") == 0) );
}


BOOL CFileSystem::Exists(const char *fname)
{
    struct stat s;
    if (stat(fname, &s) == -1)
        return(FALSE);

    return(TRUE);
}


BOOL CFileSystem::IsDirectory(const char *fname)
{
    struct stat s;
    if (stat(fname, &s) == -1)
        return(FALSE);

    return(S_ISDIR(s.st_mode) ? TRUE : FALSE);
}


CFileSystem *CFileSystem::GetInstance(const char *argv0, const char *gamename)
{
    return(new CUnixFileSystem(argv0, gamename));
}


static char *copyEnvironmentVariable(const char *varname)
{
    const char *envr = getenv(varname);
    char *retval = NULL;

    if (envr != NULL)
    {
        retval = new char[strlen(envr) + 1];
        strcpy(retval, envr);
    }

    return(retval);
}


static char *getUserDirByUID(void)
{
    uid_t uid = getuid();
    struct passwd *pw;
    char *retval = NULL;

    pw = getpwuid(uid);
    if ((pw != NULL) && (pw->pw_dir != NULL))
    {
        retval = new char[strlen(pw->pw_dir) + 1];
        strcpy(retval, pw->pw_dir);
    }
    
    return(retval);
}

// !!! FIXME: This could stand to use CTFileNames ...
static char *calcExePath(const char *_argv0)
{
    char *ptr;
    char *retval = NULL;
    char *argv0 = new char[strlen(_argv0) + 1];
    strcpy(argv0, _argv0); /* _argv0 may be read-only... */

    ptr = strrchr(argv0, '/');
    if (ptr != NULL) // explicit path specified? We're done.
    {
        retval = new char[strlen(argv0) + 1];
        strcpy(retval, argv0);
    }

    // If there isn't a path on argv0, then look through the $PATH for it...
    else
    {
        char *envr;
        char *start;
        char *exe;

        envr = copyEnvironmentVariable("PATH");
        if (!envr)
        {
            delete[] argv0;
            return(NULL);
        }

        start = envr;
        do
        {
            ptr = strchr(start, ':');
            if (ptr)
                *ptr = '\0';

            exe = new char[strlen(start) + strlen(argv0) + 2];
            strcpy(exe, start);
            if (exe[strlen(exe) - 1] != '/') // make sure there's a dir sep...
                strcat(exe, "/");

            strcat(exe, argv0);  // add on the binary name...

            if (access(exe, X_OK) != 0)   // Not our binary?
                delete[] exe;

            else  // matching executable file found on path...this is it.
            {
                retval = new char[strlen(exe) + 1];
                strcpy(retval, exe);
                delete[] exe;
                break;
            }

            start = ptr + 1;
        } while (ptr != NULL);

        delete[] envr;
    }

    delete[] argv0;

    if (retval != NULL)
    {
        char full[MAXPATHLEN];
        realpath(retval, full);
        delete[] retval;
        retval = new char[strlen(full) + 1];
        strcpy(retval, full);
    }

    return(retval);
}


// !!! FIXME: cut and paste from Engine.cpp !  --ryan.
// reverses string
static void StrRev( char *str) {
  char ctmp;
  char *pch0 = str;
  char *pch1 = str+strlen(str)-1;
  while( pch1>pch0) {
    ctmp  = *pch0;
    *pch0 = *pch1;
    *pch1 = ctmp;
    pch0++;
    pch1--;
  }
}


// FIXME: This is such a lame hack.  --ryan.
static void calcModExt(char *full, const char *exePath, const char *gamename)
{
    strcat(full, gamename);

    // bah...duplication from Engine.cpp ...
    char strDirPath[MAX_PATH] = "";
    char strTmpPath[MAX_PATH] = "";
    strncpy(strTmpPath, exePath, sizeof(strTmpPath)-1);
    strDirPath[sizeof(strTmpPath)-1] = 0;
    // remove name from application path
    StrRev(strTmpPath);  
    // find last backslash
    char *pstr = strchr( strTmpPath, '/');
    if( pstr==NULL) {
        // not found - path is just "\"
        strcpy( strTmpPath, "/");
        pstr = strTmpPath;
    } 
    // remove 'debug' from app path if needed
    if( strnicmp( pstr, "/gubed", 6)==0) pstr += 6;
    if( *pstr == '/') pstr++;
    char *pstrFin = strchr( pstr, '/');
    if( pstrFin==NULL) {
      strcpy( pstr, "/");
      pstrFin = pstr;
    }
    // copy that to the path
    StrRev(pstrFin);
    strncpy( strDirPath, pstrFin, sizeof(strDirPath)-1);
    strDirPath[sizeof(strDirPath)-1] = 0;
    strcat(strDirPath, "/ModEXT.txt");

    if (access(strDirPath, F_OK) == 0)
    {
        FILE *in = fopen(strDirPath, "rb");
        int rc = fread(strTmpPath, 1, 30, in);
        if (rc > 0)
        {
            strTmpPath[rc] = '\0';
            strcat(full, strTmpPath);
        }
    }

    strcat(full, "/");
}


CUnixFileSystem::CUnixFileSystem(const char *argv0, const char *gamename)
    : exePath(NULL),
      userDir(NULL)
{
    exePath = calcExePath(argv0);
    if (exePath == NULL)
    {
        exePath = new char[strlen(argv0) + 3];
        strcpy(exePath, "./");  // (*shrug*)
        strcat(exePath, argv0);
    }


    userDir = copyEnvironmentVariable("HOME");
    if (userDir == NULL)
        userDir = getUserDirByUID();

    char full[MAXPATHLEN];
    realpath(userDir, full);
    delete[] userDir;

    if (full[strlen(full) - 1] != '/')
        strcat(full, "/");

    // make sure it ends with a dir separator!
    #if PLATFORM_MACOSX
    strcat(full, "Library/");
    mkdir(full, S_IRWXU);  // don't care if this fails. We'll catch it later.
    strcat(full, "Application Support/");
    mkdir(full, S_IRWXU);  // don't care if this fails. We'll catch it later.
    strcat(full, "Serious Sam/");
    #else
    strcat(full, ".serious/");
    #endif

    if (!Exists(full)) {
        if (mkdir(full, S_IRWXU) == -1) {
            FatalError("User dir creation failed! (%s)\n", strerror(errno));
        }
    }

    calcModExt(full, exePath, gamename);
    if (!Exists(full)) {
        if (mkdir(full, S_IRWXU) == -1) {
            FatalError("User dir creation failed! (%s)\n", strerror(errno));
        }
    }

    userDir = new char[strlen(full) + 1];
    strcpy(userDir, full);
}


CUnixFileSystem::~CUnixFileSystem(void)
{
    delete[] exePath;
    delete[] userDir;
}


void CUnixFileSystem::GetExecutablePath(char *buf, ULONG bufSize)
{
    buf[bufSize - 1] = '\0';  // just in case.
    strncpy(buf, exePath, bufSize);
}


void CUnixFileSystem::GetUserDirectory(char *buf, ULONG bufSize)
{
    buf[bufSize - 1] = '\0';  // just in case.
    strncpy(buf, userDir, bufSize);
}


CDynamicArray<CTString> *CUnixFileSystem::FindFiles(const char *dir,
                                                   const char *wildcard)
{
    CDynamicArray<CTString> *retval = new CDynamicArray<CTString>;
    DIR *d = opendir(dir);

    if (d != NULL)
    {
        struct dirent *dent;
        while ((dent = readdir(d)) != NULL)
        {
            CTString str(dent->d_name);
            if (str.Matches(wildcard))
                *retval->New() = str;
        }
        closedir(d);
    }

    return(retval);
}

// end of UnixFileSystem.cpp ...



