#include "ZokataEngine/systems/scene/SceneManager.h"

#include <filesystem>
#include <sstream>
#include <string>
#include <utility>

#include "ZokataEngine/systems/scene/SceneLoader.h"
#include "ZokataLog/Log.h"

namespace fs = std::filesystem;

namespace ZKT
{
namespace ENGINE
{
namespace
{
constexpr const char* kScenesFolder = "scenes";
}

Scene& SceneManager::CreateScene(const std::string& name)
{
    auto scene = std::make_unique<Scene>(name);
    return AddScene(std::move(scene));
}

Scene& SceneManager::AddScene(std::unique_ptr<Scene> scene)
{
    Scene& ref = *scene;
    scenes_.push_back(std::move(scene));
    if (active_scene_ == nullptr)
    {
        active_scene_ = &ref;
    }
    return ref;
}

void SceneManager::SetActiveScene(Scene* scene)
{
    active_scene_ = scene;
}

Scene* SceneManager::ActiveScene()
{
    return active_scene_;
}

const Scene* SceneManager::ActiveScene() const
{
    return active_scene_;
}

const std::vector<std::unique_ptr<Scene>>& SceneManager::Scenes() const
{
    return scenes_;
}

const std::vector<SceneMetadata>& SceneManager::SceneFiles() const
{
    return scenes_metadata_;
}

void SceneManager::OnEnableActive()
{
    if (active_scene_ != nullptr)
    {
        active_scene_->OnEnable();
    }
}

void SceneManager::StartActive()
{
    if (active_scene_ != nullptr)
    {
        active_scene_->Start();
    }
}

void SceneManager::UpdateActive(float delta_seconds)
{
    if (active_scene_ != nullptr)
    {
        active_scene_->Update(delta_seconds);
    }
}

void SceneManager::FixedUpdateActive(float fixed_seconds)
{
    if (active_scene_ != nullptr)
    {
        active_scene_->FixedUpdate(fixed_seconds);
    }
}

void SceneManager::DiscoverAndLoadScenes(const fs::path& scenes_root)
{
    scenes_metadata_.clear();
    scenes_root_ = scenes_root.empty() ? FindScenesRoot(fs::current_path()) : scenes_root;

    if (scenes_root_.empty())
    {
        ZLOG_WARN("Scenes directory not found after probing from: " + fs::current_path().string());
        return;
    }
    if (!fs::exists(scenes_root_) || !fs::is_directory(scenes_root_))
    {
        ZLOG_WARN("Scenes directory not found: " + scenes_root_.string());
        return;
    }

    for (const auto& entry : fs::directory_iterator(scenes_root_))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }
        const auto& path = entry.path();
        if (path.extension() == ".yaml" || path.extension() == ".yml")
        {
            scenes_metadata_.push_back(SceneMetadata{
                .name = path.stem().string(),
                .file_path = path,
            });
            ZLOG_INFO("Found scene file: " + path.filename().string());
            LoadSceneFromFile(path);
        }
    }

    ZKT::LOG::LogMessage msg {
        .tag = "SCENE",
        .text = "Loaded " + std::to_string(scenes_.size()) + " scene(s) from " + scenes_root_.string(),
        .color_code = "\033[35m",  // magenta tag for scene discovery
    };
    ZLOG_CUSTOM(msg);
}

void SceneManager::LoadSceneFromFile(const fs::path& scene_path)
{
    try
    {
        SceneLoader loader;
        auto loaded_scene = loader.LoadFromFile(scene_path);
        if (loaded_scene)
        {
            Scene& scene_ref = AddScene(std::move(loaded_scene));
            if (active_scene_ == nullptr)
            {
                active_scene_ = &scene_ref;
            }
            ZLOG_INFO("Loaded scene '" + scene_ref.Name() + "' from " + scene_path.filename().string());
        }
    }
    catch (const std::exception& e)
    {
        ZLOG_ERROR("Failed to load scene '" + scene_path.string() + "': " + e.what());
    }
}

fs::path SceneManager::FindScenesRoot(const fs::path& hint)
{
    fs::path path = hint;
    for (int i = 0; i < 6; ++i)
    {
        const auto candidate = path / kScenesFolder;
        if (fs::exists(candidate) && fs::is_directory(candidate))
        {
            return candidate;
        }
        if (!path.has_parent_path())
        {
            break;
        }
        path = path.parent_path();
    }
    return {};
}
}  // namespace ENGINE
}  // namespace ZKT
