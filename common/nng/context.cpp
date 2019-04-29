#include <mapf/common/context.h>

namespace mapf
{

Context& Context::Instance()
{
    static Context instance;
    return instance;
}

Context::~Context()
{
    Close();
}

Context::Context()
{
}

void Context::Close()
{
}

void *Context::get()
{
    return ctx_;
}

} /* namespace mapf */