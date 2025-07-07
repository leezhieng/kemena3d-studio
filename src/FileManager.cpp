#include "FileManager.h"

FileManager::FileManager()
{
    // Determine all the standard paths for later use
    exePath = wxStandardPaths::Get().GetExecutablePath();
    wxFileName exeFile(exePath);
    baseDir = exeFile.GetPath();
}

FileManager::~FileManager()
{
}

std::string FileManager::getCurrentDirPath()
{
    std::string path = projectPath;

    if (currentDir.size() > 0)
    {
        for (size_t i = 0; i < currentDir.size(); ++i)
        {
            path += wxFILE_SEP_PATH + currentDir.at(i);
        }
    }

    return path;
}

void FileManager::checkAssetsChange(wxString path, bool recursive)
{
    wxDir dir(path);
    if (!dir.IsOpened())
    {
        std::cout << "Cannot open directory: " << path << std::endl;
        return;
    }

    wxString filename;
    bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES | wxDIR_DIRS);

    while (cont)
    {
        /*
        Examples:
        path = "/home/user/projects"
        fullPath = "/home/user/projects/assets/image.png"
        relativePath = "assets/image.png"
        */

        wxString fullPath = path + wxFILE_SEP_PATH + filename;

        wxFileName fn(fullPath);
        fn.MakeRelativeTo(path);

        wxString relativePath = fn.GetFullPath();

        if (wxDirExists(fullPath))
        {
            //wxLogMessage("Directory: %s", fullPath);

            if (recursive)
            {
                checkAssetsChange(fullPath);
            }
        }
        else
        {
            //wxLogMessage("File: %s", fullPath);
        }

        cont = dir.GetNext(&filename);
    }
}

std::string fileChecksum(const wxString& fileName)
{
    wxFile file(fileName);
    if (!file.IsOpened())
        return {};

    const size_t bufferSize = 8192;
    std::vector<uint8_t> buffer(bufferSize);

    MD5 md5;

    ssize_t bytesRead;
    while ((bytesRead = file.Read(buffer.data(), bufferSize)) > 0)
    {
        md5.update(buffer.data(), static_cast<size_t>(bytesRead));
    }

    return md5.final();
}

std::string FileManager::getRandomString(int stringLength)
{
    const std::string possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

    std::string randomString;
    for(int i = 0; i < stringLength; ++i)
    {
        std::random_device rd;  // seed
        std::mt19937 gen(rd()); // Mersenne Twister RNG
        std::uniform_int_distribution<> dist(1, 100); // range [1, 100]

        int index = dist(gen) % possibleCharacters.length();

        char nextChar = possibleCharacters.at(index);
        randomString.append(&nextChar);
    }
    return randomString;
}

bool FileManager::newProject(wxString name, wxString path)
{
    wxString fullPath = path + wxFileName::GetPathSeparator() + name;

    // Create main project directory
    if (!wxFileName::Mkdir(fullPath, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
    {
        wxMessageBox("Failed to create project directory:\n" + fullPath, "Error", wxICON_ERROR);
        return false;
    }

    // Create subfolders
    wxArrayString subfolders = { "Assets", "Library", "Config" };
    for (const auto& folder : subfolders)
    {
        wxString subfolderPath = fullPath + wxFileName::GetPathSeparator() + folder;
        if (!wxFileName::Mkdir(subfolderPath, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
        {
            wxMessageBox("Failed to create subfolder:\n" + subfolderPath, "Error", wxICON_ERROR);
            return false;
        }
    }

    projectPath = fullPath;
    currentDir.push_back("Assets");

    // Create project config file

    return true;
}

bool FileManager::openProject(const wxString& path)
{
    // Check main project directory
    wxString assetsPath = path + wxFILE_SEP_PATH + "Assets";
    wxString libraryPath = path + wxFILE_SEP_PATH + "Library";
    wxString configPath = path + wxFILE_SEP_PATH + "Config";

    bool foldersExist = wxFileName::DirExists(assetsPath) && wxFileName::DirExists(libraryPath) && wxFileName::DirExists(configPath);

    if (!foldersExist)
    {
        wxMessageBox("Failed to open project. Invalid directory structure.", "Error", wxICON_ERROR);
        return false;
    }

    projectPath = path;
    currentDir.push_back("Assets");

    // Check project config file


    return true;
}
