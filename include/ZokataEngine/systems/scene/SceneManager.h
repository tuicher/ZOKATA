#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "ZokataEngine/systems/scene/Scene.h"

namespace ZKT
{
namespace ENGINE
{
struct SceneMetadata
{
    std::string name;
    std::filesystem::path file_path;
};

// SceneManager supervises one or many loaded scenes.
/**
 * @brief Orchestrates scene creation, loading, and activation at runtime.
 */
class SceneManager
{
public:
    SceneManager() = default;

    /**
     * @brief Creates and registers a new empty scene.
     */
    Scene& CreateScene(const std::string& name = "");
    /**
     * @brief Adds an existing scene and makes it active if none is set.
     */
    Scene& AddScene(std::unique_ptr<Scene> scene);

    /**
     * @brief Sets the currently active scene.
     */
    void SetActiveScene(Scene* scene);
    /**
     * @brief Returns the active scene (mutable/const).
     */
    Scene* ActiveScene();
    const Scene* ActiveScene() const;

    const std::vector<std::unique_ptr<Scene>>& Scenes() const;
    const std::vector<SceneMetadata>& SceneFiles() const;

    // Drive lifecycle on the active scene (no-op if none).
    /**
     * @brief Drives OnEnable on the active scene.
     */
    void OnEnableActive();
    /**
     * @brief Drives Start on the active scene.
     */
    void StartActive();
    /**
     * @brief Drives Update on the active scene.
     */
    void UpdateActive(float delta_seconds);
    /**
     * @brief Drives FixedUpdate on the active scene.
     */
    void FixedUpdateActive(float fixed_seconds);

    // Discover and load .yaml/.yml scenes from a folder (searches default if empty).
    /**
     * @brief Discovers scene files and loads them into managed scenes.
     * @param scenes_root Optional folder hint; auto-discovers if empty.
     */
    void DiscoverAndLoadScenes(const std::filesystem::path& scenes_root = {});

private:
    std::vector<std::unique_ptr<Scene>> scenes_;
    Scene* active_scene_ = nullptr;
    std::vector<SceneMetadata> scenes_metadata_;
    std::filesystem::path scenes_root_;

    void LoadSceneFromFile(const std::filesystem::path& scene_path);
    static std::filesystem::path FindScenesRoot(const std::filesystem::path& hint);
};
}  // namespace ENGINE
}  // namespace ZKT
