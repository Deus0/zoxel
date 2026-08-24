byte locks_enabled = 1;

#ifdef zox_windows

    typedef SRWLOCK spinlock;

    static inline void spinlock_init(spinlock* lock) {
        if (locks_enabled) {
            InitializeSRWLock(lock);
        }
    }

    static inline void spin_lock(spinlock* lock) {
        if (locks_enabled) {
            AcquireSRWLockExclusive(lock);
        }
    }

    static inline void spin_unlock(spinlock* lock) {
        if (locks_enabled) {
            ReleaseSRWLockExclusive(lock);
        }
    }

#else

    #include <stdatomic.h>

    typedef struct {
        atomic_flag flag;
    } spinlock;

    static inline void spinlock_init(spinlock* lock) {
        if (locks_enabled) {
            atomic_flag_clear(&lock->flag);
        }
    }

    static inline void spin_lock(spinlock* lock) {
        if (locks_enabled) {
            while (atomic_flag_test_and_set_explicit(&lock->flag, memory_order_acquire)) {
                // optionally: sched_yield();
            }
        }
    }

    static inline void spin_unlock(spinlock* lock) {
        if (locks_enabled) {
            atomic_flag_clear_explicit(&lock->flag, memory_order_release);
        }
    }

#endif
