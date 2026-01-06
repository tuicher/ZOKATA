#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include "ZokataEngine/systems/scene/Component.h"
#include "ZokataEngine/systems/scene/components/TransformComponent.h"

namespace ZKT
{
namespace ENGINE
{
/**
 * @brief Scene graph node that owns components and parent/child hierarchy.
 *
 * Provides utilities to add/query components and drive lifecycle recursively.
 */
class Entity
{
public:
    /**
     * @brief Constructs an entity with an id and optional name; ensures a Transform component exists.
     */
    explicit Entity(int64_t id, std::string name = "");
    virtual ~Entity();

    /**
     * @brief Unique identifier for this entity.
     */
    int64_t Id() const;
    /**
     * @brief Human-readable name of the entity.
     */
    const std::string& Name() const;

    /**
     * @brief Returns the parent entity (nullptr if root).
     */
    Entity* Parent() const;
    /**
     * @brief Child list accessors.
     */
    const std::vector<std::unique_ptr<Entity>>& Children() const;
    std::vector<std::unique_ptr<Entity>>& Children();

    /**
     * @brief Sets the parent pointer (does not rewire collections).
     */
    void SetParent(Entity* parent);

    /**
     * @brief Access the Transform component (guaranteed to exist).
     */
    TransformComponent& Transform();
    const TransformComponent& Transform() const;

    template <typename T, typename... Args>
    /**
     * @brief Adds a new component of type T, forwarding constructor args.
     * @note T must derive from Component; ownership stays inside the entity.
     */
    T& AddComponent(Args&&... args)
    {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
        auto comp = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *comp;
        comp->SetOwner(this);
        components_.push_back(std::move(comp));

        if constexpr (std::is_same_v<std::decay_t<T>, TransformComponent>)
        {
            transform_ = &ref;
        }
        return ref;
    }

    /**
     * @brief Adds an already-created component instance.
     */
    void AddComponent(std::unique_ptr<Component> comp);

    /**
     * @brief Adds a child entity, rewiring its parent pointer.
     * @return Raw pointer to the inserted child.
     */
    Entity* AddChild(std::unique_ptr<Entity> child);

    template <typename T>
    T* GetComponent()
    {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
        for (auto& comp : components_)
        {
            if (auto* concrete = dynamic_cast<T*>(comp.get()))
            {
                return concrete;
            }
        }
        return nullptr;
    }

    template <typename T>
    const T* GetComponent() const
    {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
        for (const auto& comp : components_)
        {
            if (const auto* concrete = dynamic_cast<const T*>(comp.get()))
            {
                return concrete;
            }
        }
        return nullptr;
    }

    template <typename T>
    std::vector<T*> GetComponents()
    {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
        std::vector<T*> matches;
        for (auto& comp : components_)
        {
            if (auto* concrete = dynamic_cast<T*>(comp.get()))
            {
                matches.push_back(concrete);
            }
        }
        return matches;
    }

    template <typename T>
    std::vector<const T*> GetComponents() const
    {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
        std::vector<const T*> matches;
        for (const auto& comp : components_)
        {
            if (const auto* concrete = dynamic_cast<const T*>(comp.get()))
            {
                matches.push_back(concrete);
            }
        }
        return matches;
    }

    // Depth-first search through descendants (optionally including this entity).
    template <typename T>
    T* GetComponentInChildren(bool include_self = false)
    {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
        if (include_self)
        {
            if (auto* own = GetComponent<T>())
            {
                return own;
            }
        }
        for (auto& child : children_)
        {
            if (auto* direct = child->template GetComponent<T>())
            {
                return direct;
            }
            if (auto* nested = child->template GetComponentInChildren<T>(false))
            {
                return nested;
            }
        }
        return nullptr;
    }

    template <typename T>
    const T* GetComponentInChildren(bool include_self = false) const
    {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
        if (include_self)
        {
            if (const auto* own = GetComponent<T>())
            {
                return own;
            }
        }
        for (const auto& child : children_)
        {
            const Entity* child_ptr = child.get();
            if (const auto* direct = child_ptr->template GetComponent<T>())
            {
                return direct;
            }
            if (const auto* nested = child_ptr->template GetComponentInChildren<T>(false))
            {
                return nested;
            }
        }
        return nullptr;
    }

    template <typename T>
    std::vector<T*> GetComponentsInChildren(bool include_self = false)
    {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
        std::vector<T*> matches;
        if (include_self)
        {
            auto own = GetComponents<T>();
            matches.insert(matches.end(), own.begin(), own.end());
        }
        for (auto& child : children_)
        {
            auto child_matches = child->template GetComponentsInChildren<T>(true);
            matches.insert(matches.end(), child_matches.begin(), child_matches.end());
        }
        return matches;
    }

    template <typename T>
    std::vector<const T*> GetComponentsInChildren(bool include_self = false) const
    {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
        std::vector<const T*> matches;
        if (include_self)
        {
            auto own = GetComponents<T>();
            matches.insert(matches.end(), own.begin(), own.end());
        }
        for (const auto& child : children_)
        {
            const Entity* child_ptr = child.get();
            auto child_matches = child_ptr->template GetComponentsInChildren<T>(true);
            matches.insert(matches.end(), child_matches.begin(), child_matches.end());
        }
        return matches;
    }

    // Lifecycle orchestration called by SceneManager/Scene.
    void OnEnableRecursive();
    void StartRecursive();
    void UpdateRecursive(float delta_seconds);
    void FixedUpdateRecursive(float fixed_seconds);

    // Lifecycle for this entity only (no children) driven by SceneManager/Scene.
    void OnEnableSelf();
    void StartSelf();
    void UpdateSelf(float delta_seconds);
    void FixedUpdateSelf(float fixed_seconds);

protected:
    // Hook for custom per-entity logic if subclasses need it.
    virtual void OnEnableEntity() {}
    virtual void StartEntity() {}
    virtual void UpdateEntity(float /*delta_seconds*/) {}
    virtual void FixedUpdateEntity(float /*fixed_seconds*/) {}

private:
    int64_t id_ = -1;
    std::string name_;
    Entity* parent_ = nullptr;
    std::vector<std::unique_ptr<Component>> components_;
    std::vector<std::unique_ptr<Entity>> children_;
    TransformComponent* transform_ = nullptr;

    void EnsureTransform();
};
}  // namespace ENGINE
}  // namespace ZKT
