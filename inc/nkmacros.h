#ifndef _Inkmacros
#define _Inkmacros

#define nk_stringify(x) #x
#define nk_tostring(x) nk_stringify(x)
#define nk_member_size(type, member) sizeof(((type *)0)->member)
#define nk_unused(x) ((void)(x))

#endif
