#pragma once

#ifdef _DEBUG
#include <string>
#include <iostream>

typedef std::string debug_string;
#define debug_cout std::cout 
#define debug_endl std::endl


#define DEBUG_COUNT_SIZE \
    static int s_debug_count_size_;\
    static debug_string s_class_name;
#define INIT_DEBUG_COUNT_SIZE(class_name) \
int class_name::s_debug_count_size_ = 0;\
    debug_string class_name::s_class_name = (""#class_name);

#define DEBUG_OUTPUT_CONSTRUCTOR_COUNT \
    debug_cout << (" ") << s_class_name << ("£º") << ++s_debug_count_size_ << debug_endl;
#define DEBUG_OUTPUT_DESTRUCTOR_COUNT \
    debug_cout << ("~") << s_class_name << ("£º") << --s_debug_count_size_ << debug_endl;


#else

#define DEBUG_COUNT_SIZE
#define INIT_DEBUG_COUNT_SIZE(class_name)
#define DEBUG_OUTPUT_CONSTRUCTOR_COUNT
#define DEBUG_OUTPUT_DESTRUCTOR_COUNT


#endif // _DEBUG
