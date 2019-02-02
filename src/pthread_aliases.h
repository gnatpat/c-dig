inline void lockMutex(Mutex* mutex) {
  pthread_mutex_lock(mutex);
}

inline void unlockMutex(Mutex* mutex) {
  pthread_mutex_unlock(mutex);
}

inline void waitForCondition(MutexCondition* condition, Mutex* mutex) {
  pthread_cond_wait(condition, mutex);
}

inline void signalCondition(MutexCondition* condition) {
  pthread_cond_signal(condition);
}
