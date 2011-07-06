file(GLOB LUA_C_FILES "*.c")


add_library(lua_static STATIC ${LUA_C_FILES})
add_library(lua SHARED ${LUA_C_FILES})


