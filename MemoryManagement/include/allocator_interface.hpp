#pragma once

namespace memory {

class AllocatorInterface {
public:
    virtual ~AllocatorInterface() = 0;
    virtual void init() = 0;
    virtual bool is_init() = 0;
    virtual void destroy() = 0;
    virtual void* alloc(size_t) = 0;
    virtual void free(void*) = 0;
    virtual bool belongs(void*) = 0;
#if _DEBUG
    virtual void dumpStat() const = 0;
    virtual void dumpBlocks() const = 0;
#endif
};

} // namespace memory
