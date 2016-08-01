#pragma once

namespace ms {

    //  Private copy constructor and copy assignment ensure classes derived from  
    //  class noncopyable cannot be copied.  

    //  Contributed by Dave Abrahams  

    namespace noncopyable_  // protection from unintended ADL  
    {
        class noncopyable
        {
        protected:
            noncopyable() {}
            ~noncopyable() {}
        private:  // emphasize the following members are private  
            noncopyable(const noncopyable&) = delete;
            const noncopyable& operator=(const noncopyable&) = delete;
        };
    }

    typedef noncopyable_::noncopyable noncopyable;

} // namespace ms  
