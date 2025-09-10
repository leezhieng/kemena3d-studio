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
		auto result = pfd::message(
						  "Unsaved Changes",
						  "You have unsaved changes. Do you want to create a new project without saving?",
						  pfd::choice::yes_no,
						  pfd::icon::warning
					  ).result();

		if (result == pfd::button::no)
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

		pfd::message(
			"Invalid Directory",     // title
			msg,                     // message
			pfd::choice::ok,         // only an OK button
			pfd::icon::warning       // warning icon
		).result();

		return false;
	}

	// Create project directory
	fs::path fullPath = fs::path(path);

	std::error_code ec;

	// Create required subfolders
	fs::create_directories(fullPath / "Assets", ec);
	fs::create_directories(fullPath / "Library", ec);
	fs::create_directories(fullPath / "Library" / "Metadata", ec);
	fs::create_directories(fullPath / "Library" / "Thumbnails", ec);
	fs::create_directories(fullPath / "Library" / "ImportedAssets", ec);
	fs::create_directories(fullPath / "Config", ec);

	std::string msg = "Project created at: " + fullPath.string();

	pfd::message(
		"Success",     // title
		msg,                     // message
		pfd::choice::ok,         // only an OK button
		pfd::icon::warning       // warning icon
	).result();

	// Extract the folder name
	projectName = fullPath.filename().string();
	projectOpened = true;
	projectSaved = false;
	refreshWindowTitle();

	projectPath = path;
	currentDir.clear();
	currentDir.push_back("Assets");

	// TODO: Create project config file
	checkAssetJson();

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
		auto result = pfd::message(
						  "Unsaved Changes",
						  "You have unsaved changes. Do you want to open a new project without saving?",
						  pfd::choice::yes_no,
						  pfd::icon::warning
					  ).result();

		if (result == pfd::button::no)
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

		pfd::message(
			"Invalid Directory",     // title
			msg,                     // message
			pfd::choice::ok,         // only an OK button
			pfd::icon::warning       // warning icon
		).result();

		return false;
	}

	fs::path fullPath = fs::path(path);

	fs::path assetsPath  = fullPath / "Assets";
	fs::path libraryPath = fullPath / "Library";
	fs::path configPath  = fullPath / "Config";

	if (!(fs::exists(assetsPath) && fs::exists(libraryPath) && fs::exists(configPath)))
	{
		std::string msg = "Failed to open project. Invalid directory structure.\n";

		pfd::message(
			"Invalid Directory",     // title
			msg,                     // message
			pfd::choice::ok,         // only an OK button
			pfd::icon::warning       // warning icon
		).result();

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
	checkAssetJson();

	// Create other essential folders if don't exist
	std::error_code ec;

	fs::path metadataPath = fullPath / "Library" / "Metadata";
	if (!(fs::exists(metadataPath)))
        fs::create_directories(fullPath / "Library" / "Metadata", ec);

	fs::path thumbnailsPath = fullPath / "Library" / "Thumbnails";
	if (!(fs::exists(thumbnailsPath)))
        fs::create_directories(fullPath / "Library" / "Thumbnails", ec);

	fs::path importedAssetsPath = fullPath / "Library" / "ImportedAssets";
	if (!(fs::exists(importedAssetsPath)))
        fs::create_directories(fullPath / "Library" / "ImportedAssets", ec);

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

void Manager::checkAssetJson()
{
	fs::path libraryFolder = projectPath / "Library";
	fs::path assetsJsonFile = libraryFolder / "assets.json";
	fs::path assetPath = projectPath / "Assets";

	// Check whether assets.json exist or not
	try
	{
		if (!fs::exists(libraryFolder))
		{
			fs::create_directories(libraryFolder);
			std::cout << "Created Library folder: " << libraryFolder << "\n";
		}

		if (!fs::exists(assetsJsonFile))
		{
			json j;
			j["files"] = json::array();

			std::ofstream ofs(assetsJsonFile);
			ofs << j.dump(4); // pretty print
			ofs.close();

			std::cout << "Created assets.json at: " << assetsJsonFile << "\n";
		}
		else
		{
			std::cout << "assets.json already exists.\n";
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << "\n";
		return;
	}

	// Read assets.json
	// Check whether the files exist or not
	json j;
	{
		std::ifstream ifs(assetsJsonFile);
		ifs >> j;
	}

	if (!j.contains("files") || !j["files"].is_array())
	{
		std::cerr << "Invalid assets.json format\n";
		return;
	}

	if (!j["files"].empty())
    {
        for (auto it = j["files"].begin(); it != j["files"].end();)
        {
            std::string uuid = (*it)["uuid"].get<std::string>();
            std::string relPath = (*it)["name"].get<std::string>();
            std::string checksum = (*it).value("checksum", "");
            std::string type = (*it)["type"].get<std::string>();

            fs::path filePath = assetPath / relPath;

            if (!fs::exists(filePath))
            {
                std::cout << "Missing: " << relPath << " (removed from list)\n";
                it = j["files"].erase(it);

                // Delete metadata, thumbnail and imported asset?
                // Delete metadata
                fs::path metadataFile = libraryFolder / "Metadata" / (uuid + ".json");
                if (fs::exists(metadataFile))
                {
                    try
                    {
                        if (fs::remove(metadataFile))
                        {
                            std::cout << "Deleted file: " << metadataFile << "\n";
                        }
                        else
                        {
                            std::cout << "Failed to delete file (unknown reason): " << metadataFile << "\n";
                        }
                    }
                    catch (const fs::filesystem_error& e)
                    {
                        std::cerr << "Error deleting file: " << e.what() << "\n";
                    }
                }
                else
                {
                    std::cout << "File does not exist: " << metadataFile << "\n";
                }

                continue;
            }

            // Fill struct and store in map
            FileInfo info{ relPath, checksum, type };
            fileMap[uuid] = info;

            ++it;
        }
    }

	// Build a reverse lookup from path -> uuid for convenience
    std::unordered_map<std::string, std::string> relPathToUuid;
    for (const auto& [uuid, info] : fileMap)
        relPathToUuid[info.path] = uuid;

    // Check all files in the asset folder
    for (auto &p : fs::recursive_directory_iterator(assetPath))
    {
        if (!p.is_regular_file()) continue;

        std::string relPath = fs::relative(p.path(), assetPath).generic_string();
        std::string checksum = generateFileChecksum(p.path().string());

        std::string fileUuid;
        std::string fileType;

        // Check with assets.json
        auto it = relPathToUuid.find(relPath);
        if (it == relPathToUuid.end())
        {
            // New file
            std::string uuid = generateUuid();
            std::string type = checkAssetType(p.path());

            fileUuid = uuid;
            fileType = type;

            FileInfo info{ relPath, checksum, type };
            fileMap[uuid] = info;
            relPathToUuid[relPath] = uuid;

            json newEntry = {
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
            std::string uuid = it->second;
            FileInfo& info = fileMap[uuid];

            fileUuid = uuid;
            fileType = info.type;

            if (info.checksum != checksum)
            {
                std::cout << "File changed: " << relPath << "\n";
                info.checksum = checksum;

                // Update JSON as well
                for (auto& entry : j["files"])
                {
                    if (entry["uuid"] == uuid)
                    {
                        entry["checksum"] = checksum;
                        break;
                    }
                }
            }
        }

        // Check whether metadata exist or not
        if (!fileUuid.empty() && !fileType.empty())
        {
            fs::path metaPath = libraryFolder / "Metadata" / (fileUuid + ".json");

            if (!fs::exists(metaPath))
            {
                std::cout << "Missing meta file for UUID " << fileUuid << std::endl;

                // Create new meta file
                nlohmann::json metaJson;

                // Populate the JSON with engine version and type, ignore properties for now
                metaJson["version"] = kemena::engineVersion;
                metaJson["type"] = fileType;

                // Write JSON to file
                std::ofstream file(metaPath);
                if (!file)
                {
                    std::cerr << "Failed to create metadata file: " << metaPath << "\n";
                    return;
                }

                file << metaJson.dump(4); // Pretty print with 4-space indent
                file.close();
            }
            else
            {
                // Meta exists, you can optionally read it
                // e.g., json metaJson = loadJson(metaPath);
            }
        }
    }

	// Save back
	std::ofstream out(assetsJsonFile);
	out << j.dump(4);
	std::cout << "assets.json updated.\n";
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

std::string Manager::checkAssetType(const fs::path &p)
{
	auto ext = p.extension().string();

	if (ext == ".txt" || ext == ".ini" || ext == ".xml" || ext == ".json")
		return "text";
	else if (ext == ".jpg" || ext == ".jpeg" || ext == ".bmp" || ext == ".png" || ext == ".gif" || ext == ".tiff" || ext == ".tga")
		return "image";
	else if (ext == ".as")
		return "script";
	else if (ext == ".mp3" || ext == ".wav" || ext == ".ogg")
		return "audio";
	else if (ext == ".mp4" || ext == ".mov" || ext == ".avi" || ext == ".webm")
		return "video";
	else if (ext == ".obj" || ext == ".fbx" || ext == ".gltf" || ext == ".glb" || ext == ".dae" || ext == ".stl")
		return "mesh";
	else if (ext == ".pfb")
		return "prefab";
	else if (ext == ".world")
		return "world";
	else if (ext == ".mat")
		return "material";

	return "unknown";     // Unknown
}
