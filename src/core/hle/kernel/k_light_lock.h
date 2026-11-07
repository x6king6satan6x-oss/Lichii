// SPDX-FileCopyrightText: Copyright 2021 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <atomic>

#include "core/hle/kernel/k_scoped_lock.h"

namespace Kernel {

class KernelCore;

class KLightLock {
public:
    explicit KLightLock(KernelCore& kernel) : m_kernel{kernel} {}

    void Lock();

    /// Acquires the lock only when it is free. A host thread must not take the slow path: that
    /// parks the thread on the guest scheduler until the owner runs, and the owner can be a guest
    /// core blocked in the GPU thread (Ocarina of Time, build 93).
    bool TryLock();

    void Unlock();

    bool LockSlowPath(uintptr_t owner, uintptr_t cur_thread);

    void UnlockSlowPath(uintptr_t cur_thread);

    bool IsLocked() const {
        return m_tag.load() != 0;
    }

    bool IsLockedByCurrentThread() const;

private:
    std::atomic<uintptr_t> m_tag{};
    KernelCore& m_kernel;
};

using KScopedLightLock = KScopedLock<KLightLock>;

} // namespace Kernel
