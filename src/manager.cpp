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
		showingMessageBox = true;

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
	checkAssetChange();

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
	showingMessageBox = true;

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

	// Open project successful

	// Extract the folder name
	projectName = fullPath.filename().string();
	projectOpened = true;
	projectSaved = false;
	refreshWindowTitle();

	projectPath = path;
	currentDir.clear();
	currentDir.push_back("Assets");

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

	// TODO: check project config file
	// Need this to false to check assets
	showingMessageBox = false;
	checkAssetChange();

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

void Manager::checkAssetChange()
{
	if (projectOpened && !showingMessageBox)
	{
		fs::path libraryFolder = projectPath / "Library";
		fs::path assetsJsonFile = libraryFolder / "assets.json";
		fs::path assetsPath = projectPath / "Assets";

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
		// Check if file is empty
		if (fs::is_empty(assetsJsonFile))
		{
			std::cout << "assets.json is empty, reinitializing...\n";
			j["files"] = json::array();

			std::ofstream ofs(assetsJsonFile);
			ofs << j.dump(4);
			ofs.close();
		}
		else
		{
			std::ifstream ifs(assetsJsonFile);
			try
			{
				ifs >> j;
			}
			catch (const std::exception& e)
			{
				std::cerr << "Failed to parse assets.json: " << e.what() << "\n";
				j["files"] = json::array(); // reset to valid state
			}
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
				std::string relativePath = (*it)["name"].get<std::string>();
				std::string checksum = (*it).value("checksum", "");
				std::string type = (*it)["type"].get<std::string>();

				fs::path filePath = assetsPath / relativePath;

				if (!fs::exists(filePath))
				{
					std::cout << "Missing: " << relativePath << " (removed from list)\n";
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

					// Delete imported assets
					std::string assetExt;

					if (type == "mesh")
						assetExt = ".glb";
					else if (type == "image")
						assetExt = ".ktx2";

					fs::path importedAssetsFile = libraryFolder / "ImportedAssets" / (uuid + assetExt);
					if (fs::exists(importedAssetsFile))
					{
						try
						{
							if (fs::remove(importedAssetsFile))
							{
								std::cout << "Deleted file: " << importedAssetsFile << "\n";
							}
							else
							{
								std::cout << "Failed to delete file (unknown reason): " << importedAssetsFile << "\n";
							}
						}
						catch (const fs::filesystem_error& e)
						{
							std::cerr << "Error deleting file: " << e.what() << "\n";
						}
					}
					else
					{
						std::cout << "File does not exist: " << importedAssetsFile << "\n";
					}

					continue;
				}

				// Fill struct and store in map
				FileInfo info{ relativePath, checksum, type };
				fileMap[uuid] = info;

				++it;
			}
		}

		// Build a reverse lookup from path -> uuid for convenience
		uuidMap.clear();
		for (const auto& [uuid, info] : fileMap)
			uuidMap[info.path] = uuid;

		// Check all files in the Assets folder
		for (auto &p : fs::recursive_directory_iterator(assetsPath))
		{
			if (!p.is_regular_file()) continue;

			std::string relativePath = fs::relative(p.path(), assetsPath).generic_string();
			std::string checksum = generateFileChecksum(p.path().string());

			std::string fileUuid;
			std::string fileType;
			bool needImport = false;

			// Check with assets.json
			auto it = uuidMap.find(relativePath);
			if (it == uuidMap.end())
			{
				// New file
				std::string uuid = generateUuid();
				std::string type = checkAssetType(p.path());

				fileUuid = uuid;
				fileType = type;
				needImport = true;  // Need import

				FileInfo info{ relativePath, checksum, type };
				fileMap[uuid] = info;
				uuidMap[relativePath] = uuid;

				json newEntry =
				{
					{"name", relativePath},
					{"uuid", uuid},
					{"checksum", checksum},
					{"type", type}
				};
				j["files"].push_back(newEntry);

				std::cout << "New file added: " << relativePath << "\n";
			}
			else
			{
				// Existing file, check checksum
				std::string uuid = it->second;
				FileInfo& info = fileMap[uuid];

				fileUuid = uuid;
				fileType = info.type;

				// Different checksum
				if (info.checksum != checksum)
				{
					std::cout << "File changed: " << relativePath << "\n";
					info.checksum = checksum;
					needImport = true;  // Need import

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

			if (!fileUuid.empty() && !fileType.empty())
			{
				// Check whether metadata exist or not
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

				// Fixed extension
				string uuidExt;

				if (fileType == "mesh")
					uuidExt = ".glb";
				else if (fileType == "image")
					uuidExt = ".ktx2";

				// Check whether imported asset exist or not
				fs::path importedAssetPath = libraryFolder / "ImportedAssets" / (fileUuid + uuidExt);
				if (!fs::exists(importedAssetPath))
					needImport = true;  // Need import

				// Check imported asset exist or not
				if (needImport)
				{
					// Only import if it is mesh, image, etc.
					if (fileType == "mesh" || fileType == "image")
					{
						fs::path from(assetsPath / relativePath);
						fs::path to(libraryFolder / "ImportedAssets" / (fileUuid + uuidExt));

						std:: cout << from << " -> " << to << std::endl;

						importTasks.push_back(
						{
							from,
							to,
							fileType,
							false,
							false
						});
					}
				}
			}
		}

		// Save back
		std::ofstream out(assetsJsonFile);
		out << j.dump(4);
		std::cout << "assets.json updated.\n";

		// Begin batch imports
		startBatchImport(importTasks);
	}

	// Reset so that it will check again
	showingMessageBox = false;
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

void Manager::closeEditor()
{
	if (!projectSaved)
	{
		showingMessageBox = true;

		auto result = pfd::message(
						  "Unsaved Changes",
						  "Project not saved. Do you really want to quit?",
						  pfd::choice::yes_no,
						  pfd::icon::warning
					  ).result();

		if (result == pfd::button::yes)
		{
			window->setRunning(false); // user confirmed quit
		}
	}
	else
	{
		window->setRunning(false);
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

void Manager::startBatchImport(const std::vector<ImportTask>& tasks)
{
	{
		std::scoped_lock lock(queueMutex);
		importQueue = tasks;
	}

	filesProcessed = 0;
	batchDone = false;

	importFuture = std::async(std::launch::async, [this]()
	{
		for (auto& task : importQueue)
		{
			if (task.type == "mesh")
			{
				task.success = convertMeshToGlb(task.inputPath, task.outputPath);
			}
			else
			{
				// Not handled yet -> mark as skipped
				task.success = false;
				std::cout << "Skipping: " << task.inputPath << " (type=" << task.type << " not supported yet)\n";
			}

			filesProcessed++;
		}
		batchDone = true;
	});
}

void Manager::drawImportPopup(PanelConsole* console)
{
	if (showImportPopup)
	{
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("Importing Assets...", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			int totalFiles = (int)importQueue.size();
			int processed  = filesProcessed.load();

			float progress = (totalFiles > 0) ? (float)processed / (float)totalFiles : 0.0f;

			if (!batchDone)
			{
				ImGui::Text("Converting files...");
				ImGui::ProgressBar(progress, ImVec2(250.f, 0.f));
				ImGui::Text("Processed %d / %d", processed, totalFiles);
				ImGui::EndPopup();
			}
			else
			{
				// Record the time conversion finished
				if (importEndTime.time_since_epoch().count() == 0)
				{
					importEndTime = std::chrono::steady_clock::now();
				}

				ImGui::Text("Import complete!");
				//ImGui::Separator();

				ImGui::ProgressBar(progress, ImVec2(250.f, 0.f));
				ImGui::Text("Processed %d / %d", processed, totalFiles);

				for (auto& task : importQueue)
				{
					/*ImGui::Text("%s -> %s [%s]",
								task.inputPath.string().c_str(),
								task.outputPath.string().c_str(),
								task.success ? "OK" : "FAIL");*/

					if (!task.success && !task.reported)
					{
						console->addLog(LogLevel::Error, (string("Failed to import asset: ") + task.inputPath.generic_string()).c_str());
						task.reported = true;
					}
				}

				// Auto-close after 2 seconds
				auto now = std::chrono::steady_clock::now();
				if (std::chrono::duration_cast<std::chrono::seconds>(now - importEndTime).count() >= 2)
				{
					ImGui::CloseCurrentPopup();
					showImportPopup = false;
					importEndTime = {}; // reset for next run
				}

				ImGui::EndPopup();

				importTasks.clear();
			}
		}
	}
}

void Manager::selectObject(const std::string uuid, bool clearList)
{
    if (clearList)
        selectedObjects.clear();

    auto it = std::find(selectedObjects.begin(), selectedObjects.end(), uuid);
    if (it == selectedObjects.end())
    {
        selectedObjects.push_back(uuid);
    }
}

void Manager::deselectObject(const std::string uuid)
{
    auto it = std::find(selectedObjects.begin(), selectedObjects.end(), uuid);
    if (it != selectedObjects.end())
    {
        selectedObjects.erase(it);
    }
}
