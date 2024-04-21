#ifndef COMPAT_H
#define COMPAT_H

#if __cplusplus >= 201103L
    #define Q_NULLPTR nullptr
#else
    // not NULL to stay consistent with Qt’s convention
    #define Q_NULLPTR 0
#endif

#endif // COMPAT_H
