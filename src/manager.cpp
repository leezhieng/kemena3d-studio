#include "manager.h"

namespace fs = std::filesystem;

Manager::Manager(kWindow* setWindow, kWorld* setWorld)
{
	window = setWindow;
	world = setWorld;
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

Manager::~Manager() = default;

std::string Manager::getCurrentDirPath()
{
	fs::path path = projectPath;

	for (const auto& dir : currentDir)
	{
		path /= dir;
	}

	return path.string();
}

/*void Manager::checkAssetsChange(const std::string& path, bool recursive)
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
}*/

void Manager::openFolder(string name)
{
	currentDir.push_back(name);
}

void Manager::closeFolder()
{
	if (currentDir.size() > 1)
	{
		currentDir.pop_back();
	}
}

bool Manager::newProject()
{
	// Check if project is saved
	if (!projectSaved)
	{
		const SDL_MessageBoxButtonData buttons[] =
		{
			{ SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "No"  },
			{ SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Yes" }
		};

		const SDL_MessageBoxData messageboxdata =
		{
			SDL_MESSAGEBOX_WARNING,
			window->getSdlWindow(),
			"Unsaved Project",
			"You have unsaved changes. Do you want to create a new project without saving?",
			SDL_arraysize(buttons),
			buttons,
			nullptr // no custom colors
		};

		int buttonid = -1;
		if (!SDL_ShowMessageBox(&messageboxdata, &buttonid))
		{
			SDL_Log("Error showing message box: %s", SDL_GetError());
			return false;
		}

		if (buttonid != 1)
			return false;
	}

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
	checkDirJson();

	if (panelProject != nullptr)
    {
		panelProject->refreshTreeList();
		panelProject->refreshThumbnailList();
    }

	// WIP: Load scenes of the world

	if (panelHierarchy != nullptr)
		panelHierarchy->refreshList();

	return true;
}

bool Manager::openProject()
{
	// Check if project is saved
	if (!projectSaved)
	{
		const SDL_MessageBoxButtonData buttons[] =
		{
			{ SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, "No"  },
			{ SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Yes" }
		};

		const SDL_MessageBoxData messageboxdata =
		{
			SDL_MESSAGEBOX_WARNING,
			window->getSdlWindow(),
			"Unsaved Project",
			"You have unsaved changes. Do you want to open a new project without saving?",
			SDL_arraysize(buttons),
			buttons,
			nullptr // no custom colors
		};

		int buttonid = -1;
		if (!SDL_ShowMessageBox(&messageboxdata, &buttonid))
		{
			SDL_Log("Error showing message box: %s", SDL_GetError());
			return false;
		}

		if (buttonid != 1)
			return false;
	}

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
	checkDirJson();

	if (panelProject != nullptr)
    {
		panelProject->refreshTreeList();
		panelProject->refreshThumbnailList();
    }

	// WIP: Load scenes of the world

	if (panelHierarchy != nullptr)
		panelHierarchy->refreshList();

	return true;
}

void Manager::refreshWindowTitle()
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

// Internal function
void Manager::checkDirJson()
{
	fs::path libraryFolder = projectPath / "Library";
	fs::path dirJsonFile = libraryFolder / "dir.json";
	fs::path assetPath = projectPath / "Assets";

	// Check whether file exist
	try
	{
		if (!fs::exists(libraryFolder))
		{
			fs::create_directories(libraryFolder);
			std::cout << "Created Library folder: " << libraryFolder << "\n";
		}

		if (!fs::exists(dirJsonFile))
		{
			json j;
			j["files"] = json::array();

			std::ofstream ofs(dirJsonFile);
			ofs << j.dump(4); // pretty print
			ofs.close();

			std::cout << "Created dir.json at: " << dirJsonFile << "\n";
		}
		else
		{
			std::cout << "dir.json already exists.\n";
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << "\n";
		return;
	}

	// Read dir.json
	// Check whether the files exist or not
	json j;
	{
		std::ifstream ifs(dirJsonFile);
		ifs >> j;
	}

	if (!j.contains("files") || !j["files"].is_array())
	{
		std::cerr << "Invalid dir.json format\n";
		return;
	}

	if (!j["files"].empty())
	{
		for (auto it = j["files"].begin(); it != j["files"].end();)
		{
			std::string uuid = (*it)["uuid"].get<std::string>();
			std::string relPath = (*it)["name"].get<std::string>();   // path/name.ext
			std::string checksum = (*it).value("checksum", "");
			int type = (*it).value("type", 0);

			fs::path filePath = assetPath / relPath;

			if (!fs::exists(filePath))
			{
				// File missing -> remove from JSON
				std::cout << "Missing: " << relPath << " (removed from list)\n";
				it = j["files"].erase(it);
				continue;
			}

			fileChecksum[relPath] = checksum;
			fileUuid[uuid] = relPath;
			fileType[relPath] = type;

			++it;
		}
	}

	// Check all the files and compare with
	for (auto &p : fs::recursive_directory_iterator(assetPath))
	{
		if (!p.is_regular_file()) continue;

		std::string relPath = fs::relative(p.path(), assetPath).generic_string();
		std::string checksum = generateFileChecksum(p.path().string());

		if (fileUuid.find(relPath) == fileUuid.end())
		{
			// New file
			std::string uuid = generateUuid();
			int type = checkAssetType(p.path());

			fileUuid[relPath] = uuid;
			fileChecksum[relPath] = checksum;
			fileType[relPath] = type;

			json newEntry =
			{
				{"name", relPath},
				{"uuid", uuid},
				{"checksum", checksum},
				{"type", type}
			};
			j["files"].push_back(newEntry);

			std::cout << "New file added: " << relPath << "\n";
		}
		else
		{
			// Existing file, check checksum
			if (fileChecksum[relPath] != checksum)
			{
				std::cout << "File changed: " << relPath << "\n";
				fileChecksum[relPath] = checksum;
			}
		}
	}

	// Save back
	std::ofstream out(dirJsonFile);
	out << j.dump(4);
	std::cout << "dir.json updated.\n";
}

int Manager::checkAssetType(const fs::path &p)
{
	auto ext = p.extension().string();

	if (ext == ".txt" || ext == ".ini" || ext == ".xml" || ext == ".json")
		return 1;
	else if (ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".png" || ext == ".gif" || ext == ".tiff" || ext == ".tga")
		return 2;
	else if (ext == ".as")
		return 3;
	else if (ext == ".mp3" || ext == ".wav" || ext == ".ogg")
		return 4;
	else if (ext == ".mp4" || ext == ".mov" || ext == ".avi" || ext == ".webm")
		return 5;
	else if (ext == ".obj" || ext == ".fbx" || ext == ".gltf" || ext == ".glb" || ext == ".dae" || ext == ".stl")
		return 6;
	else if (ext == ".pfb")
		return 7;
	else if (ext == ".world")
		return 8;
	else if (ext == ".mat")
		return 9;

	return 0;     // Unknown
}
