#include "filemanager.h"

namespace fs = std::filesystem;

FileManager::FileManager(kWindow* setWindow)
{
    window = setWindow;
    initialWindowTitle = window->getWindowTitle();

	try
	{
#ifdef _WIN32
		char buffer[MAX_PATH];
		DWORD len = GetModuleFileNameA(NULL, buffer, MAX_PATH);
		exePath = std::string(buffer, len);
#elif __APPLE__
		char buffer[PATH_MAX];
		uint32_t size = sizeof(buffer);
		if (_NSGetExecutablePath(buffer, &size) == 0)
			exePath = fs::canonical(buffer).string();
		else
			exePath.clear();
#elif __linux__
		char buffer[PATH_MAX];
		ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
		if (len != -1)
		{
			buffer[len] = '\0';
			exePath = fs::canonical(buffer).string();
		}
		else
		{
			exePath.clear();
		}
#else
		exePath = fs::current_path().string(); // fallback for unknown platforms
#endif

		if (!exePath.empty())
			baseDir = fs::path(exePath).parent_path().string();
		else
			baseDir = fs::current_path().string(); // fallback

	}
	catch (const fs::filesystem_error& e)
	{
		std::cerr << "Error resolving executable path: " << e.what() << std::endl;
		exePath.clear();
		baseDir = fs::current_path().string(); // fallback
	}
}

FileManager::~FileManager() = default;

std::string FileManager::getCurrentDirPath()
{
	fs::path path = projectPath;

	for (const auto& dir : currentDir)
	{
		path /= dir;
	}

	return path.string();
}

void FileManager::checkAssetsChange(const std::string& path, bool recursive)
{
	if (!fs::exists(path) || !fs::is_directory(path))
	{
		std::cout << "Cannot open directory: " << path << std::endl;
		return;
	}

	if (recursive)
	{
		for (auto& entry : fs::recursive_directory_iterator(path))
		{
			auto relativePath = fs::relative(entry.path(), path);
			if (entry.is_directory())
			{
				// std::cout << "[DIR ] " << relativePath.string() << "\n";
			}
			else
			{
				// std::cout << "[FILE] " << relativePath.string() << "\n";
			}
		}
	}
	else
	{
		for (auto& entry : fs::directory_iterator(path))
		{
			auto relativePath = fs::relative(entry.path(), path);
			if (entry.is_directory())
			{
				// std::cout << "[DIR ] " << relativePath.string() << "\n";
			}
			else
			{
				// std::cout << "[FILE] " << relativePath.string() << "\n";
			}
		}
	}
}

std::string FileManager::fileChecksum(const std::string& fileName)
{
	std::ifstream file(fileName, std::ios::binary);
	if (!file.is_open())
		return {};

	const size_t bufferSize = 8192;
	std::vector<uint8_t> buffer(bufferSize);

	MD5 md5;

	while (file)
	{
		file.read(reinterpret_cast<char*>(buffer.data()), bufferSize);
		std::streamsize bytesRead = file.gcount();
		if (bytesRead > 0)
		{
			md5.update(buffer.data(), static_cast<size_t>(bytesRead));
		}
	}

	return md5.final();
}

std::string FileManager::getRandomString(int stringLength)
{
	const std::string possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

	std::string randomString;
	randomString.reserve(stringLength);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, (int)possibleCharacters.length() - 1);

	for (int i = 0; i < stringLength; ++i)
	{
		char nextChar = possibleCharacters.at(dist(gen));
		randomString.push_back(nextChar);
	}

	return randomString;
}

bool FileManager::newProject()
{
	auto path = pfd::select_folder("Select project folder").result();

	if (path.empty())
    {
        return false;
    }

	if (!fs::exists(path) || !fs::is_directory(path))
	{
		std::string msg = "Directory does not exist:\n" + path;
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING,
								 "Invalid Directory",
								 msg.c_str(),
								 nullptr);
		return false;
	}

	// Create project directory
	fs::path fullPath = fs::path(path);

	std::error_code ec;

	// Create required subfolders
	fs::create_directories(fullPath / "Assets", ec);
	fs::create_directories(fullPath / "Library", ec);
	fs::create_directories(fullPath / "Config", ec);

	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
							 "Success",
							 ("Project created at: " + fullPath.string()).c_str(),
							 nullptr);

    // Extract the folder name
    projectName = fullPath.filename().string();
	projectOpened = true;
	projectSaved = false;
	refreshWindowTitle();

	projectPath = path;
	currentDir.clear();
	currentDir.push_back("Assets");

	// TODO: Create project config file

	return true;
}

bool FileManager::openProject()
{
	auto path = pfd::select_folder("Select project folder").result();

	if (path.empty())
    {
        return false;
    }

	if (!fs::exists(path) || !fs::is_directory(path))
	{
		std::string msg = "Directory does not exist:\n" + path;
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Invalid Directory", msg.c_str(), nullptr);
		return false;
	}

	fs::path fullPath = fs::path(path);

	fs::path assetsPath  = fullPath / "Assets";
	fs::path libraryPath = fullPath / "Library";
	fs::path configPath  = fullPath / "Config";

	if (!(fs::exists(assetsPath) && fs::exists(libraryPath) && fs::exists(configPath)))
	{
		std::string msg = "Failed to open project. Invalid directory structure.\n";
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Invalid Directory", msg.c_str(), nullptr);
		return false;
	}

	// Extract the folder name
    projectName = fullPath.filename().string();
	projectOpened = true;
	projectSaved = false;
	refreshWindowTitle();

	projectPath = path;
	currentDir.clear();
	currentDir.push_back("Assets");

	// TODO: check project config file

	return true;
}

void FileManager::refreshWindowTitle()
{
    if (!projectOpened)
    {
        window->setWindowTitle(initialWindowTitle);
    }
    else
    {
        if (worldName == "")
            window->setWindowTitle(initialWindowTitle + " - " + projectName + " - Untitled");
        else
            window->setWindowTitle(initialWindowTitle + " - " + projectName + " - " + worldName);

        if (!projectSaved)
            window->setWindowTitle(window->getWindowTitle() + "*");
    }
}
