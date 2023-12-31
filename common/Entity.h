#pragma once
#pragma comment(lib, "rpcrt4.lib")  // UuidCreate
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

struct Entity
{
    int uuid;
};

/* Comparator required for unordered_map */
inline bool operator < (const Entity& a, const Entity& b)
{
    return a.uuid < b.uuid;
}

/* Equality comparator */
inline bool operator == (const Entity& a, const Entity& b)
{
    return a.uuid == b.uuid;
}

class EntityManager
{
public:
    EntityManager(bool client=false);
    ~EntityManager();
    Entity create();
    Entity create(int uuid);
    void destroy(Entity entity);

private:
    int lastEntity = 0;
    bool isClient = false;
};
