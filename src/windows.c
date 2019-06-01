#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <iup.h>
#include <iuplua.h>
#include "gui-lua.h"
#include "gifwrapper.h"

int lua_gifwrapper(lua_State *L) {
    unsigned int delay;
    const char *output;
    const char *folder;

    delay = (unsigned int)lua_tointeger(L,1);
    output = lua_tostring(L,2);
    folder = lua_tostring(L,3);

    int done = gifwrapper_doit(delay,output,folder);
    lua_pushboolean(L,done == 0);

    return 1;
}


int main(void) {
  lua_State *L = luaL_newstate();
  luaL_openlibs(L);

  iuplua_open(L);

  lua_pushcfunction(L,lua_gifwrapper);
  lua_setglobal(L,"gifwrapper");

#ifdef _DEBUG
  iuplua_dofile(L,"gui.lua");
#else
  if(luaL_loadbuffer(L,(const char *)gui,sizeof(gui),"gui.lua") != 0) {
      fprintf(stderr,"error: %s",lua_tostring(L,-1));
      return 1;
  }

  if(lua_pcall(L, 0, 0, 0) != 0) {
      fprintf(stderr,"error: %s",lua_tostring(L,-1));
      return 1;
  }
#endif

  lua_close(L);
  return 0;
}
