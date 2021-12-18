#include "actor_prop.h"
#include "actor.h"

ActorProp& ActorProp::operator=(const ActorProp& prop)
{
	if (type == PROP_TYPE_STR && prop.type != PROP_TYPE_STR) s.~CXString();
	if (type == PROP_TYPE_STR && prop.type == PROP_TYPE_STR)
		s = prop.s;
	else
		copyUnion(prop);
	type = prop.type;
	return *this;
}

ActorProp& ActorProp::operator=(bool v)
{
	if (type == PROP_TYPE_STR) s.~CXString();
	b = v;
	type = PROP_TYPE_BOOL;
	return *this;
}

ActorProp& ActorProp::operator=(int v)
{
	if (type == PROP_TYPE_STR) s.~basic_string();
	i = v;
	type = PROP_TYPE_INT;
	return *this;
}

ActorProp& ActorProp::operator=(float  v)
{
	if (type == PROP_TYPE_STR) s.~CXString();
	f = v;
	type = PROP_TYPE_FLOAT;
	return *this;
}

ActorProp& ActorProp::operator=(float v[2])
{
	if (type == PROP_TYPE_STR) s.~CXString();
	v2[0] = v[0];
	v2[1] = v[1];
	type = PROP_TYPE_VEC2;
	return *this;
}

ActorProp& ActorProp::operator=(uint64_t v)
{
	if (type == PROP_TYPE_STR) s.~CXString();
	l = v;
	type = PROP_TYPE_UINT64;
	return *this;
}

ActorProp& ActorProp::operator=(const CXString& v)
{
	if (type == PROP_TYPE_STR) {
		s = v;
	}
	else {
		new(&s) CXString(v);
	}
	type = PROP_TYPE_STR;
	return *this;
}

ActorProp& ActorProp::operator=(const char* v)
{
	if (type == PROP_TYPE_STR) {
		s = v;
	}
	else {
		new(&s) CXString(v);
	}
	type = PROP_TYPE_STR;
	return *this;
}



void ActorProp::copyUnion(const ActorProp& prop)
{
	switch (prop.type)
	{
	case PROP_TYPE_BOOL:
		b = prop.b;
		break;
	case PROP_TYPE_INT:
		i = prop.i;
		break;
	case PROP_TYPE_UINT64:
		l = prop.l;
		break;
	case PROP_TYPE_FLOAT:
		f = prop.f;
		break;
	case PROP_TYPE_STR:
		new(&s) CXString(prop.s);
		break;
	case PROP_TYPE_VEC2:
		v2[0] = prop.v2[0];
		v2[1] = prop.v2[1];
		break;
	default:
		break;
	}
}

int actor_set_prop(lua_State* L)
{
	Actor* actor = lua_check_actor(L, 1);
	int prop_index = (int)lua_tointeger(L, 2);
	ActorProp& prop = actor->GetProperty(prop_index);
	switch (prop.type)
	{
	case PROP_TYPE_BOOL:
		prop = (bool)lua_toboolean(L, 3);
		break;
	case PROP_TYPE_INT:
		prop = (int)lua_tointeger(L, 3);
		break;
	case PROP_TYPE_UINT64:
		prop = (uint64_t)lua_tointeger(L, 3);
		break;
	case PROP_TYPE_FLOAT:
		prop = (float)lua_tonumber(L, 3);
		break;
	case PROP_TYPE_STR:
		prop = CXString(lua_tostring(L, 3)) ;
		break;
	case PROP_TYPE_VEC2:
		float v[2];
		for (int i = 0; i < 2; i++) {
			lua_geti(L, 3, i+1);
			v[i] = (float)lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
		prop = v;
		break;
	default:
		break;
	}
	actor->DirtyProperty(prop_index);
	return 0;
}

int actor_get_prop(lua_State* L)
{
	Actor* actor = lua_check_actor(L, 1);
	int prop_index = (int)lua_tointeger(L, 2);
	ActorProp& prop = actor->GetProperty(prop_index);
	switch (prop.type)
	{
	case PROP_TYPE_BOOL:
		lua_pushboolean(L, prop.b);
		break;
	case PROP_TYPE_INT:
		lua_pushinteger(L, prop.i);
		break;
	case PROP_TYPE_UINT64:
		lua_pushinteger(L, prop.l);
		break;
	case PROP_TYPE_FLOAT:
		lua_pushnumber(L, prop.f);
		break;
	case PROP_TYPE_STR:
		lua_pushstring(L, prop.s.c_str());
		break;
	case PROP_TYPE_VEC2:
		lua_newtable(L);
		for (int i = 0; i < 2; i++) {
			lua_pushnumber(L, prop.v2[i]);
			lua_seti(L, -2, i+1);
		}
		break;
	default:
		break;
	}
	return 1;
}


int actor_reg_prop(lua_State* L)
{
	Actor* actor = lua_check_actor(L, 1);
	int prop_index = (int)lua_tointeger(L, 2);
	int type = (int)lua_tointeger(L, 3);
	ActorProp prop;
	switch (type)
	{
	case PROP_TYPE_BOOL:
		prop = (bool)lua_toboolean(L, 4);
		break;
	case PROP_TYPE_INT:
		prop = (int)lua_tointeger(L, 4);
		break;
	case PROP_TYPE_UINT64:
		prop = (uint64_t)lua_tointeger(L, 4);
		break;
	case PROP_TYPE_FLOAT:
		prop = (float)lua_tonumber(L, 4);
		break;
	case PROP_TYPE_STR:
		prop = CXString(lua_tostring(L, 4));
		break;
	case PROP_TYPE_VEC2:
		float v[2];
		for (int i = 0; i < 2; i++) {
			lua_geti(L, 4, i + 1);
			v[i] = (float)lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
		prop = v;
	default:
		break;
	}
	actor->RegProperty(prop_index, prop);
	return 0;
}
