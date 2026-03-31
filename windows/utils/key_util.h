#ifndef KEY_UTIL_H
#define KEY_UTIL_H

class QKeySequence;

QKeySequence virtualKeyToKeySequence(int vk);
int keySequenceToVirtualKey(const QKeySequence &ks);

#endif // KEY_UTIL_H