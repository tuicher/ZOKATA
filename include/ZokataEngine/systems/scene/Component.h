#pragma once

namespace ZKT
{
namespace ENGINE
{
class Entity;

/**
 * @brief Base contract for any component attached to a scene entity.
 *
 * Declares minimal lifecycle hooks and shared state (owner and enabled flag).
 */
class Component
{
public:
    virtual ~Component() = default;

    virtual void OnEnable() = 0;
    /**
     * @brief Invoked when the component becomes enabled/active.
     */
    virtual void OnDisable() = 0;
    /**
     * @brief Called once before the first Update.
     */
    virtual void Start() = 0;
    /**
     * @brief Per-frame update driven by the owning entity.
     * @param delta_seconds Time elapsed since last frame.
     */
    virtual void Update(float delta_seconds) = 0;
    /**
     * @brief Fixed-timestep update for simulation.
     * @param fixed_seconds Step interval configured by the engine.
     */
    virtual void FixedUpdate(float fixed_seconds) = 0;

    /**
     * @brief Set the owning entity; normally handled internally by Entity.
     */
    void SetOwner(Entity* owner);
    /**
     * @brief Returns the owning entity.
     */
    Entity* Owner();
    /**
     * @brief Returns the owning entity (const view).
     */
    const Entity* Owner() const;

    /**
     * @brief Toggle whether this component participates in lifecycle calls.
     */
    void SetEnabled(bool enabled);
    /**
     * @brief Whether the component is enabled.
     */
    bool Enabled() const;

protected:
    Entity* owner_ = nullptr;
    bool enabled_ = true;
};
}  // namespace ENGINE
}  // namespace ZKT
