#pragma once
#ifndef UNIX_USE
#  error "<unix/services/actuation.hpp> is included by <unix/module.hpp>; include that instead"
#endif

namespace UNIx {

/// <summary>Acting on the world as the local player. Unity main thread only; needs
/// unix_cap_actuation. unix_e_arg for a bad object, unix_e_state when the game refused.</summary>
class Actuation {
public:
    /// <summary>Use an interactable, as clicking it does.</summary>
    /// <param name="ignoreRange">Interact even when the player is out of reach.</param>
    template <class = void> Status Interact(void* gameObject, bool ignoreRange = false) {
        UNIX_USE(act, interact);
        return detail::g_d.act_interact(detail::g_self, gameObject, ignoreRange);
    }
    template <class = void> Status GrabPickup(void* gameObject, Hand hand = Hand::Right) {
        UNIX_USE(act, grab_pickup);
        return detail::g_d.act_grab_pickup(detail::g_self, gameObject, static_cast<uint32_t>(hand));
    }
    /// <summary>Drop a held pickup; null drops everything held.</summary>
    template <class = void> Status DropPickup(void* gameObject = nullptr) {
        UNIX_USE(act, drop_pickup);
        return detail::g_d.act_drop_pickup(detail::g_self, gameObject);
    }
    template <class = void> Status EnterStation(void* gameObject) {
        UNIX_USE(act, enter_station);
        return detail::g_d.act_enter_station(detail::g_self, gameObject);
    }
    template <class = void> Status ExitStation(void* gameObject) {
        UNIX_USE(act, exit_station);
        return detail::g_d.act_exit_station(detail::g_self, gameObject);
    }
    template <class = void> Status Jump() {
        UNIX_USE(act, jump);
        return detail::g_d.act_jump(detail::g_self);
    }
    /// <summary>Face the player along a compass yaw, in degrees.</summary>
    template <class = void> Status Look(float yawDegrees) {
        UNIX_USE(act, look);
        return detail::g_d.act_look(detail::g_self, yawDegrees);
    }
    /// <summary>Hold a movement input for a bounded time; returns at once. Both axes are -1 to 1.</summary>
    template <class = void> Status Move(float forward, float right, uint32_t durationMs,
                                        bool run = false) {
        UNIX_USE(act, move);
        return detail::g_d.act_move(detail::g_self, forward, right, durationMs, run);
    }
    template <class = void> Status StopMove() {
        UNIX_USE(act, stop_move);
        return detail::g_d.act_stop_move(detail::g_self);
    }
};

}   // namespace UNIx
