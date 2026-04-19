#include "manager.h"

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#define STB_IMAGE_RESIZE2_IMPLEMENTATION
#include <stb/stb_image_resize2.h>

#include <kemena/kmesh.h>
#include <kemena/klight.h>
#include <kemena/kcamera.h>
#include <kemena/kmeshgenerator.h>

#include <set>
#include <functional>
#include <ctime>

namespace fs = std::filesystem;

Manager::Manager(kWindow* setWindow, kWorld* setWorld, kRenderer* setRenderer)
{
	window = setWindow;
	world = setWorld;
	renderer = setRenderer;
	initialWindowTitle = window->getWindowTitle();

	try
	{
#ifdef _WIN32
		char buffer[MAX_PATH];
		DWORD len = GetModuleFileNameA(NULL, buffer, MAX_PATH);
		exePath = kString(buffer, len);
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

kString Manager::getCurrentDirPath()
{
	fs::path path = projectPath;

	for (const auto& dir : currentDir)
	{
		path /= dir;
	}

	return path.string();
}

void Manager::openFolder(kString name)
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
		kString msg = "Directory does not exist:\n" + path;

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

	kString msg = "Project created at: " + fullPath.string();

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

	if (!renderer->getEnableObjectPicking())
	    renderer->setEnableObjectPicking(true);

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
		kString msg = "Directory does not exist:\n" + path;

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
		kString msg = "Failed to open project. Invalid directory structure.\n";

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

	if (!renderer->getEnableObjectPicking())
	    renderer->setEnableObjectPicking(true);

	projectPath = path;
	currentDir.clear();
	currentDir.push_back("Assets");

	// Create other essential folders if don't exist
	std::error_code ec;

	fs::create_directories(fullPath / "Assets", ec);
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
		bool anyChanges = false;
		fileMap.clear();
		importTasks.clear();
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
				kString uuid = (*it)["uuid"].get<kString>();
				kString relativePath = (*it)["name"].get<kString>();
				kString checksum = (*it).value("checksum", "");
				kString type = (*it)["type"].get<kString>();

				fs::path filePath = assetsPath / relativePath;

				if (!fs::exists(filePath))
				{
					std::cout << "Missing: " << relativePath << " (removed from list)\n";
					it = j["files"].erase(it);
					anyChanges = true;

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

					// Delete imported assets and thumbnail
					kString assetExt;
					if (type == "mesh")  assetExt = ".glb";
					else if (type == "image") assetExt = ".dds";

					auto tryDelete = [](const fs::path &p) {
						if (!fs::exists(p)) return;
						try { fs::remove(p); std::cout << "Deleted: " << p << "\n"; }
						catch (const fs::filesystem_error &e) { std::cerr << "Error deleting: " << e.what() << "\n"; }
					};

					if (!assetExt.empty())
						tryDelete(libraryFolder / "ImportedAssets" / (uuid + assetExt));

					tryDelete(libraryFolder / "Thumbnails" / (uuid + ".png"));

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

			kString relativePath = fs::relative(p.path(), assetsPath).generic_string();
			kString checksum = generateFileChecksum(p.path().string());

			kString fileUuid;
			kString fileType;
			bool needImport = false;

			// Check with assets.json
			auto it = uuidMap.find(relativePath);
			if (it == uuidMap.end())
			{
				// New file
				kString uuid = generateUuid();
				kString type = checkAssetType(p.path());

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
				kString uuid = it->second;
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
				// Determine dest extension
				kString uuidExt;
				if (fileType == "mesh")       uuidExt = ".glb";
				else if (fileType == "image") uuidExt = ".dds";

				fs::path srcFullPath   = assetsPath / relativePath;
				fs::path destDir       = libraryFolder / "ImportedAssets";
				fs::path destFile      = destDir / (fileUuid + uuidExt);
				fs::path thumbnailPath = libraryFolder / "Thumbnails" / (fileUuid + ".png");
				fs::path metaPath      = libraryFolder / "Metadata"   / (fileUuid + ".json");

				// Write / overwrite metadata whenever it's missing or the file changed
				if (!fs::exists(metaPath) || needImport)
				{
					nlohmann::json meta;
					meta["type"]         = fileType;
					meta["last_change"]  = static_cast<int64_t>(std::time(nullptr));
					meta["src_checksum"] = checksum;
					meta["src_path"]     = fs::relative(srcFullPath.parent_path(), projectPath).generic_string();
					meta["src_file"]     = srcFullPath.filename().generic_string();
					meta["dest_path"]    = fs::relative(destDir, projectPath).generic_string();
					meta["dest_file"]    = fileUuid + uuidExt;

					std::ofstream mf(metaPath);
					if (mf) { mf << meta.dump(4); mf.close(); }
					else std::cerr << "Failed to write metadata: " << metaPath << "\n";
				}

				// Queue conversion if imported asset is missing or source changed
				if (!fs::exists(destFile))
					needImport = true;

				// If the thumbnail is missing, force a re-import/re-generation.
				if (!fs::exists(thumbnailPath))
				{
					if (fileType == "mesh" && !uuidExt.empty())
					{
						// For meshes, delete the imported asset and metadata to trigger full re-import.
						auto tryRemove = [](const fs::path &p) {
							if (!fs::exists(p)) return;
							std::error_code ec;
							fs::remove(p, ec);
							if (!ec) std::cout << "Removed stale Library file: " << p << "\n";
							else     std::cerr << "Failed to remove: " << p << " (" << ec.message() << ")\n";
						};
						tryRemove(destFile);
						tryRemove(metaPath);
						needImport = true;
					}
					else if (fileType == "image" && fs::exists(srcFullPath) && !needImport)
					{
						// For images, just queue a thumbnail generation from the source file.
						thumbnailQueue.push_back({ fileUuid, srcFullPath, thumbnailPath, "image" });
						anyChanges = true;
					}
				}

				if (needImport && (fileType == "mesh" || fileType == "image"))
				{
					std::cout << srcFullPath << " -> " << destFile << "\n";
					importTasks.push_back({ srcFullPath, destFile, fileType,
					                        fileUuid, thumbnailPath, false, false });
					anyChanges = true;
				}
			}
		}

		// Save back
		std::ofstream out(assetsJsonFile);
		out << j.dump(4);
		std::cout << "assets.json updated.\n";

		// Begin batch imports
		startBatchImport(importTasks);

		// Refresh project panel if anything changed
		if (anyChanges && panelProject != nullptr)
			panelProject->triggerRefresh();
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

void Manager::clearWorld(bool forced)
{
    if (!forced)
    {
        if (projectOpened && worldName != "" && !projectSaved)
        {
            showingMessageBox = true;

            auto result = pfd::message(
                              "Unsaved Changes",
                              "World not saved. Do you really want to close this world?",
                              pfd::choice::yes_no,
                              pfd::icon::warning
                          ).result();

            if (result == pfd::button::no)
            {
                return;
            }
        }
    }

    if (!world->getScenes().empty())
    {
        for (kScene* scene : world->getScenes())
        {
            if (scene && scene->getRootNode())
            {
                for (kObject* child : scene->getRootNode()->getChildren())
                {
                    deleteObjectRecursive(child);
                }
                scene->getRootNode()->getChildren().clear();
                delete scene->getRootNode();
                delete scene;
            }
        }
        world->getScenes().clear();
    }
}

void Manager::deleteObjectRecursive(kObject* node)
{
    if (!node) return;

    // Delete all children first
    for (kObject* child : node->getChildren())
    {
        deleteObjectRecursive(child);
    }

    // Clear the children list to avoid dangling pointers
    node->getChildren().clear();

    // Finally delete this node
    delete node;
}

kString Manager::checkAssetType(const fs::path &p)
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
	if (tasks.empty()) return;

	{
		std::scoped_lock lock(queueMutex);
		importQueue = tasks;
	}

	filesProcessed = 0;
	batchDone = false;
	showImportPopup = true;

	importFuture = std::async(std::launch::async, [this]()
	{
		for (auto& task : importQueue)
		{
			if (task.type == "mesh")
			{
				task.success = convertMeshToGlb(task.inputPath, task.outputPath);
			}
			else if (task.type == "image")
			{
				task.success = convertImageToDxt5(task.inputPath, task.outputPath);
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
		kGuiManager *gui = console->gui;
		kVec2 center = gui->getMainViewportCenter();
		gui->setNextWindowPos(center, ImGuiCond_Appearing, kVec2(0.5f, 0.5f));

		if (gui->popupModal("Importing Assets...", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			int totalFiles = (int)importQueue.size();
			int processed  = filesProcessed.load();

			float progress = (totalFiles > 0) ? (float)processed / (float)totalFiles : 0.0f;

			if (!batchDone)
			{
				gui->text("Converting files...");
				gui->progressBar(progress, kVec2(250.f, 0.f));
				gui->text("Processed " + std::to_string(processed) + " / " + std::to_string(totalFiles));
				gui->popupEnd();
			}
			else
			{
				if (importEndTime.time_since_epoch().count() == 0)
					importEndTime = std::chrono::steady_clock::now();

				gui->text("Import complete!");

				gui->progressBar(progress, kVec2(250.f, 0.f));
				gui->text("Processed " + std::to_string(processed) + " / " + std::to_string(totalFiles));

				for (auto& task : importQueue)
				{
					if (!task.success && !task.reported)
					{
						console->addLog(LogLevel::Error, (kString("[Error] Failed to convert: ") + task.inputPath.generic_string()).c_str());
						task.reported = true;
					}
					else if (task.success && !task.reported && task.type == "mesh" && !task.thumbnailPath.empty())
					{
						thumbnailQueue.push_back({ task.uuid, task.outputPath, task.thumbnailPath, "mesh" });
						task.reported = true;
					}
					else if (task.success && !task.reported && task.type == "image" && !task.thumbnailPath.empty())
					{
						thumbnailQueue.push_back({ task.uuid, task.inputPath, task.thumbnailPath, "image" });
						task.reported = true;
					}
				}

				auto now = std::chrono::steady_clock::now();
				if (std::chrono::duration_cast<std::chrono::seconds>(now - importEndTime).count() >= 2)
				{
					gui->closeCurrentPopup();
					showImportPopup = false;
					importEndTime = {};
				}

				gui->popupEnd();

				importTasks.clear();
			}
		}
	}
}

void Manager::processThumbnailQueue(PanelConsole *console)
{
	if (thumbnailQueue.empty()) return;

	ThumbnailTask task = thumbnailQueue.front();
	thumbnailQueue.erase(thumbnailQueue.begin());

	// Skip if thumbnail already exists
	if (fs::exists(task.thumbnailPath)) return;

	if (task.type == "image")
	{
		// Load source image, fit into 128x128 canvas, save as PNG
		const int THUMB = 128;
		const uint8_t BG = 42;

		int w, h, c;
		unsigned char *src = stbi_load(task.srcPath.string().c_str(), &w, &h, &c, 4);
		if (!src)
		{
			console->addLog(LogLevel::Error,
			    ("[Error] Thumbnail: failed to load image " + task.srcPath.generic_string()).c_str());
			return;
		}

		float scale = std::min((float)THUMB / w, (float)THUMB / h);
		int fitW = std::max(1, (int)(w * scale));
		int fitH = std::max(1, (int)(h * scale));

		std::vector<uint8_t> resized(fitW * fitH * 4);
		stbir_resize_uint8_linear(src, w, h, 0, resized.data(), fitW, fitH, 0, STBIR_RGBA);
		stbi_image_free(src);

		// Fill canvas with background, then blit the resized image centered
		std::vector<uint8_t> canvas(THUMB * THUMB * 4);
		for (int i = 0; i < THUMB * THUMB * 4; i += 4)
		{
			canvas[i]     = BG;
			canvas[i + 1] = BG;
			canvas[i + 2] = BG;
			canvas[i + 3] = 255;
		}
		int offX = (THUMB - fitW) / 2;
		int offY = (THUMB - fitH) / 2;
		for (int y = 0; y < fitH; y++)
			for (int x = 0; x < fitW; x++)
			{
				int s = (y * fitW + x) * 4;
				int d = ((offY + y) * THUMB + (offX + x)) * 4;
				canvas[d]     = resized[s];
				canvas[d + 1] = resized[s + 1];
				canvas[d + 2] = resized[s + 2];
				canvas[d + 3] = resized[s + 3];
			}

		bool saved = stbi_write_png(task.thumbnailPath.string().c_str(),
		                            THUMB, THUMB, 4, canvas.data(), THUMB * 4) != 0;
		if (!saved)
			console->addLog(LogLevel::Error,
			    ("[Error] Thumbnail: failed to save " + task.thumbnailPath.generic_string()).c_str());
		else if (panelProject != nullptr)
			panelProject->triggerRefresh();

		return;
	}

	// Mesh thumbnail — rendered via offscreen renderer
	kAssetManager *am = getAssetManager();
	if (!am) return;
	kMesh *mesh = am->loadMesh(task.srcPath.generic_string());
	if (!mesh)
	{
		console->addLog(LogLevel::Error,
		    ("[Error] Thumbnail: failed to load mesh " + task.srcPath.generic_string()).c_str());
		return;
	}

	mesh->calculateModelMatrix();

	bool saved = false;
	try
	{
		thumbnailRenderer.setBackgroundColor(kVec4(42/255.0f, 42/255.0f, 42/255.0f, 1.0f));
		thumbnailRenderer.renderMesh(mesh);
		saved = thumbnailRenderer.saveToFile(task.thumbnailPath.generic_string());
	}
	catch (...)
	{
		saved = false;
	}

	if (!saved)
		console->addLog(LogLevel::Error,
		    ("[Error] Thumbnail: failed to save " + task.thumbnailPath.generic_string()).c_str());
	else if (panelProject != nullptr)
		panelProject->triggerRefresh();

	// Cleanup loaded mesh tree
	std::function<void(kMesh*)> deleteMeshTree = [&](kMesh *m) {
		for (kObject *child : m->getChildren())
			if (child->getType() == kemena::NODE_TYPE_MESH)
				deleteMeshTree(static_cast<kMesh*>(child));
		delete m;
	};
	deleteMeshTree(mesh);
}

void Manager::selectObject(const kString uuid, bool clearList)
{
    selectedScene = nullptr;

    if (clearList)
        selectedObjects.clear();

    auto it = std::find(selectedObjects.begin(), selectedObjects.end(), uuid);
    if (it == selectedObjects.end())
    {
        selectedObjects.push_back(uuid);
    }
}

void Manager::deselectObject(const kString uuid)
{
    auto it = std::find(selectedObjects.begin(), selectedObjects.end(), uuid);
    if (it != selectedObjects.end())
    {
        selectedObjects.erase(it);
    }
}

kObject *Manager::findObjectByUuid(const kString &uuid)
{
    // Fast path: objectMap (populated by hierarchy panel)
    auto it = objectMap.find(uuid);
    if (it != objectMap.end() && it->second.object)
        return it->second.object;

    // Fallback: traverse scene graph directly
    if (!scene) return nullptr;
    for (kObject *child : scene->getRootNode()->getChildren())
        if (child->getUuid() == uuid) return child;

    return nullptr;
}

std::vector<TransformState> Manager::captureSelectedTransforms()
{
    std::vector<TransformState> states;
    for (const auto &uuid : selectedObjects)
    {
        kObject *obj = findObjectByUuid(uuid);
        if (!obj) continue;
        states.push_back({ uuid, obj->getPosition(), obj->getRotation(), obj->getScale() });
    }
    return states;
}

void Manager::deleteSelectedObjects()
{
    if (!scene || selectedObjects.empty()) return;

    std::vector<DeletedObjectInfo> deleted;

    for (const auto &uuid : selectedObjects)
    {
        kObject *obj = findObjectByUuid(uuid);
        if (!obj) continue;

        kNodeType type = obj->getType();

        if (type == NODE_TYPE_LIGHT)
            scene->removeLight(static_cast<kLight *>(obj));
        else
            scene->removeMesh(static_cast<kMesh *>(obj));

        deleted.push_back({ obj, type, scene });
    }

    if (deleted.empty()) return;

    auto cmd = std::make_unique<DeleteCommand>(
        this, std::move(deleted), selectedObjects, selectedObject);

    selectedObjects.clear();
    selectedObject = nullptr;

    if (panelHierarchy)
        panelHierarchy->refreshList();

    undoRedo.push(std::move(cmd));
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

// Recursively collect all UUIDs reachable under node (excluding the node itself).
static void collectUuids(kObject *node, std::vector<kString> &out, kObject **firstObj)
{
    for (kObject *child : node->getChildren())
    {
        out.push_back(child->getUuid());
        if (*firstObj == nullptr) *firstObj = child;
        collectUuids(child, out, firstObj);
    }
}

// Apply a Phong material to a mesh.
static void applyDefaultMaterial(kMesh *mesh, kAssetManager *am)
{
    kShader   *shader = am->loadShaderFromResource("SHADER_VERTEX_MESH", "SHADER_FRAGMENT_PHONG");
    kMaterial *mat    = am->createMaterial(shader);
    mat->setAmbientColor(kVec3(1.0f, 1.0f, 1.0f));
    mat->setDiffuseColor(kVec3(0.5f, 0.5f, 0.5f));
    mesh->setMaterial(mat);
}

// Apply a gizmo icon material to a light.
static void applyLightIcon(kLight *light, kAssetManager *am, const char *gizmoResource)
{
    kShader    *shader = am->loadShaderFromResource("SHADER_VERTEX_ICON", "SHADER_FRAGMENT_ICON");
    kMaterial  *mat    = am->createMaterial(shader);
    kTexture2D *tex    = am->loadTexture2DFromResource(gizmoResource, "albedoMap",
                                                        kTextureFormat::TEX_FORMAT_RGBA);
    mat->addTexture(tex);
    light->setMaterial(mat);
}

// ---------------------------------------------------------------------------
// Edit — selection helpers
// ---------------------------------------------------------------------------

void Manager::selectAll()
{
    if (!scene) return;

    std::vector<kString> newSel;
    kObject *newSelObj = nullptr;
    collectUuids(scene->getRootNode(), newSel, &newSelObj);

    if (newSel == selectedObjects) return;

    auto before    = selectedObjects;
    auto beforeObj = selectedObject;

    selectedObjects = newSel;
    selectedObject  = newSelObj;

    undoRedo.push(std::make_unique<SelectCommand>(this, before, beforeObj, newSel, newSelObj));
}

void Manager::deselectAll()
{
    if (selectedObjects.empty()) return;

    auto before    = selectedObjects;
    auto beforeObj = selectedObject;

    selectedObjects.clear();
    selectedObject = nullptr;

    undoRedo.push(std::make_unique<SelectCommand>(this, before, beforeObj, std::vector<kString>{}, nullptr));
}

void Manager::invertSelection()
{
    if (!scene) return;

    std::set<kString> currentSet(selectedObjects.begin(), selectedObjects.end());

    std::vector<kString> allUuids;
    kObject *dummy = nullptr;
    collectUuids(scene->getRootNode(), allUuids, &dummy);

    std::vector<kString> newSel;
    kObject *newSelObj = nullptr;
    for (const auto &uuid : allUuids)
    {
        if (currentSet.find(uuid) == currentSet.end())
        {
            newSel.push_back(uuid);
            if (newSelObj == nullptr)
                newSelObj = findObjectByUuid(uuid);
        }
    }

    auto before    = selectedObjects;
    auto beforeObj = selectedObject;

    selectedObjects = newSel;
    selectedObject  = newSelObj;

    undoRedo.push(std::make_unique<SelectCommand>(this, before, beforeObj, newSel, newSelObj));
}

// ---------------------------------------------------------------------------
// Object creation — shared post-creation logic
// ---------------------------------------------------------------------------

static void finishCreate(Manager *mgr, kObject *obj, kScene *scene,
                         std::function<void()> undoFn, std::function<void()> redoFn)
{
    kString uuid = obj->getUuid();
    mgr->selectedObject = obj;
    mgr->selectObject(uuid, true);

    if (mgr->panelHierarchy)
        mgr->panelHierarchy->refreshList();

    mgr->undoRedo.push(std::make_unique<PropertyCommand>(
        std::move(undoFn), std::move(redoFn)));
}

// ---------------------------------------------------------------------------
// Create Scene
// ---------------------------------------------------------------------------

void Manager::createSceneObject()
{
    kScene *newScene = world->createScene("New Scene");

    undoRedo.push(std::make_unique<PropertyCommand>(
        [this, newScene]() { world->removeScene(newScene); },
        [this, newScene]() { world->addScene(newScene);    }
    ));
}

// ---------------------------------------------------------------------------
// Create Empty
// ---------------------------------------------------------------------------

void Manager::createEmpty()
{
    if (!scene) return;

    kObject *obj = new kObject();
    obj->setName("Empty");
    scene->addObject(obj);
    kString uuid = obj->getUuid();

    finishCreate(this, obj, scene,
        [this, obj, uuid]()
        {
            scene->removeObject(obj);
            selectedObjects.erase(std::remove(selectedObjects.begin(), selectedObjects.end(), uuid),
                                  selectedObjects.end());
            if (selectedObject == obj) selectedObject = nullptr;
            if (panelHierarchy) panelHierarchy->refreshList();
        },
        [this, obj, uuid]()
        {
            scene->addObject(obj, uuid);
            selectedObject = obj;
            selectObject(uuid, true);
            if (panelHierarchy) panelHierarchy->refreshList();
        }
    );
}

// ---------------------------------------------------------------------------
// Create Mesh Primitive
// ---------------------------------------------------------------------------

void Manager::createMeshPrimitive(kMesh *mesh, const kString &name)
{
    if (!scene) return;

    kAssetManager *am = getAssetManager();
    if (am) applyDefaultMaterial(mesh, am);

    mesh->setName(name);
    scene->addMesh(mesh);
    kString uuid = mesh->getUuid();

    finishCreate(this, mesh, scene,
        [this, mesh, uuid]()
        {
            scene->removeMesh(mesh);
            selectedObjects.erase(std::remove(selectedObjects.begin(), selectedObjects.end(), uuid),
                                  selectedObjects.end());
            if (selectedObject == mesh) selectedObject = nullptr;
            if (panelHierarchy) panelHierarchy->refreshList();
        },
        [this, mesh, uuid]()
        {
            scene->addMesh(mesh, uuid);
            selectedObject = mesh;
            selectObject(uuid, true);
            if (panelHierarchy) panelHierarchy->refreshList();
        }
    );
}

// ---------------------------------------------------------------------------
// Create Mesh from file
// ---------------------------------------------------------------------------

void Manager::createMeshFromFile()
{
    if (!scene) return;

    auto result = pfd::open_file("Open Mesh", "",
        { "Mesh files", "*.obj *.fbx *.gltf *.glb",
          "All files",  "*" }).result();

    if (result.empty()) return;

    kString filePath = result[0];
    kAssetManager *am = getAssetManager();
    if (!am) return;

    kMesh *mesh = am->loadMesh(filePath);
    if (!mesh) return;

    mesh->setLoaded(true);
    mesh->setFileName(filePath);

    // Derive a display name from the filename
    fs::path p(filePath);
    mesh->setName(p.stem().string());

    if (am) applyDefaultMaterial(mesh, am);

    scene->addMesh(mesh);
    kString uuid = mesh->getUuid();

    finishCreate(this, mesh, scene,
        [this, mesh, uuid]()
        {
            scene->removeMesh(mesh);
            selectedObjects.erase(std::remove(selectedObjects.begin(), selectedObjects.end(), uuid),
                                  selectedObjects.end());
            if (selectedObject == mesh) selectedObject = nullptr;
            if (panelHierarchy) panelHierarchy->refreshList();
        },
        [this, mesh, uuid]()
        {
            scene->addMesh(mesh, uuid);
            selectedObject = mesh;
            selectObject(uuid, true);
            if (panelHierarchy) panelHierarchy->refreshList();
        }
    );
}

// ---------------------------------------------------------------------------
// Create Light
// ---------------------------------------------------------------------------

void Manager::createLight(kLightType type)
{
    if (!scene) return;

    kAssetManager *am = getAssetManager();

    kLight *light = nullptr;
    const char *gizmoRes = "GIZMO_SUN_LIGHT";

    if (type == LIGHT_TYPE_SUN)
    {
        light = scene->addSunLight(kVec3(0, 3, 0), kVec3(0,-1,0),
                                    kVec3(1.0f, 1.0f, 1.0f),
                                    kVec3(1.0f, 1.0f, 1.0f));
        light->setName("Sun Light");
        gizmoRes = "GIZMO_SUN_LIGHT";
    }
    else if (type == LIGHT_TYPE_POINT)
    {
        light = scene->addPointLight(kVec3(0, 2, 0),
                                      kVec3(1.0f, 1.0f, 1.0f),
                                      kVec3(1.0f, 1.0f, 1.0f));
        light->setName("Point Light");
        gizmoRes = "GIZMO_POINT_LIGHT";
    }
    else if (type == LIGHT_TYPE_SPOT)
    {
        light = scene->addSpotLight(kVec3(0, 3, 0),
                                     kVec3(1.0f, 1.0f, 1.0f),
                                     kVec3(1.0f, 1.0f, 1.0f));
        light->setName("Spot Light");
        gizmoRes = "GIZMO_SPOT_LIGHT";
    }
    else return;

    light->setPower(1.0f);
    if (am) applyLightIcon(light, am, gizmoRes);

    kString uuid = light->getUuid();

    finishCreate(this, light, scene,
        [this, light, uuid]()
        {
            scene->removeLight(light);
            selectedObjects.erase(std::remove(selectedObjects.begin(), selectedObjects.end(), uuid),
                                  selectedObjects.end());
            if (selectedObject == light) selectedObject = nullptr;
            if (panelHierarchy) panelHierarchy->refreshList();
        },
        [this, light, uuid]()
        {
            scene->addLight(light);
            selectedObject = light;
            selectObject(uuid, true);
            if (panelHierarchy) panelHierarchy->refreshList();
        }
    );
}

// ---------------------------------------------------------------------------
// Create Camera
// ---------------------------------------------------------------------------

void Manager::createCamera()
{
    if (!scene) return;

    kCamera *cam = new kCamera();
    cam->setName("Camera");
    cam->setPosition(kVec3(0.0f, 1.0f, -5.0f));
    cam->setLookAt(kVec3(0.0f, 0.0f, 0.0f));
    cam->setFOV(60.0f);

    scene->addObject(cam);
    world->addCamera(cam, cam->getUuid()); // also register in world camera list
    kString uuid = cam->getUuid();

    finishCreate(this, cam, scene,
        [this, cam, uuid]()
        {
            scene->removeObject(cam);
            world->removeCamera(cam);
            selectedObjects.erase(std::remove(selectedObjects.begin(), selectedObjects.end(), uuid),
                                  selectedObjects.end());
            if (selectedObject == cam) selectedObject = nullptr;
            if (panelHierarchy) panelHierarchy->refreshList();
        },
        [this, cam, uuid]()
        {
            scene->addObject(cam, uuid);
            world->addCamera(cam, uuid);
            selectedObject = cam;
            selectObject(uuid, true);
            if (panelHierarchy) panelHierarchy->refreshList();
        }
    );
}
