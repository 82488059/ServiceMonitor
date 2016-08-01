#pragma once
namespace ms{

    template<class T>
    class Sington{
        Sington() = delete;
        Sington(const Sington&) = delete;
        void operator=(Sington const&) = delete;
    public:

        static T* GetInstance()
        {
            static T __instance;
            return &__instance;
        }


    };
}