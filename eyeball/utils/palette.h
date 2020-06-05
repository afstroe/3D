#ifndef __PALETTE_H__
#define __PALETTE_H__

#include <unordered_map>
#include <memory>

template <class Type>
class Palette 
{
public:
  Type* get(int objectId)
  {
    auto ite = m_container.find(objectId);
    if(ite != m_container.end())
    { 
      return (*ite).second.get();
    }
    else
    {
      return nullptr;
    }
  }

  void add(int key, std::unique_ptr<Type> object)
  {
    m_container[key] = std::move(object);
  }

protected:
  std::unordered_map<int, std::unique_ptr<Type>> m_container;
};

#endif // !__PALETTE_H__
