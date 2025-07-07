#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <vector>
#include <map>
#include <random>
#include <iostream>

#include <wx/string.h>
#include <wx/dir.h>
#include <wx/log.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>

#include "Md5.h"

struct FileItem
{
    std::string name;
    //QIcon icon;
    bool isFile;
    std::string extension;
};

class FileManager
{
    public:
        FileManager();
        virtual ~FileManager();

        std::string getCurrentDirPath();
        void checkAssetsChange(wxString path, bool recursive = true);
        std::string fileChecksum(const wxString& fileName);
        std::string getRandomString(int stringLength);

        bool newProject(wxString name, wxString path);
        bool openProject(const wxString& path);

        // Editor path and directory
        wxString exePath;
        wxString baseDir;

        // Project path and directory
        std::string projectPath;
        std::vector<std::string> currentDir;

    protected:

    private:
        int initialResizeCount = 0;

        // Check project files
        std::map<std::string, int> fileGUID;
        std::map<int, std::string> fileMD5;
        std::map<int, std::string> fileCache;
        int latestFileGUID = 0;

        // Project config
        std::string projectName = "New Game";
        std::string developerName = "My Company";
        std::string projectVersion = "0.0.1";
};

#endif // FILEMANAGER_H
