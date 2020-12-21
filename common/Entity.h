#pragma once
#pragma comment(lib, "rpcrt4.lib")  // UuidCreate
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
    EntityManager();
    ~EntityManager();
    Entity create();
    void destroy(Entity entity);

private:
    int lastEntity = 0;
};
