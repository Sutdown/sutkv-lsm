#include "../../include/iterator/iterator.h"
#include <vector>

/*searchitem*/
bool operator<(const SearchItem &a, const SearchItem &b)
{
  if (a.key != b.key) {
    return a.key < b.key;
  }
  return a.idx < b.idx;
}

bool operator>(const SearchItem &a, const SearchItem &b)
{
  if (a.key != b.key){
    return a.key > b.key;
  }
  return a.idx > b.idx;
}

bool operator==(const SearchItem &a, const SearchItem &b)
{
  return a.idx == b.idx && a.key == b.key;
}

/*heap iterator*/
HeapIterator::HeapIterator(std::vector<SearchItem> item_vec)
{
  for (auto &item : item_vec)
  {
    items.push(item);
  }

  while (!items.empty() && items.top().value.empty())
  {
    // 如果当前元素的value为空，则说明该元素已经被删除，需要从优先队列中删除
    auto del_key = items.top().key;
    while (!items.empty() && items.top().key == del_key)
    {
      items.pop();
    }
  }
}

std::pair<std::string, std::string> HeapIterator::operator*() const
{
  return std::make_pair(items.top().key, items.top().value);
}

HeapIterator &HeapIterator::operator++()
{
  if (items.empty())
  {
    return *this; // 处理空队列情况
  }

  auto old_item = items.top();
  items.pop();

  // 删除与旧元素key相同的元素
  while (!items.empty() && items.top().key == old_item.key)
  {
    items.pop();
  }

  // 处理被删除的元素
  while (!items.empty() && items.top().value.empty())
  {
    auto del_key = items.top().key;
    while (!items.empty() && items.top().key == del_key)
    {
      items.pop();
    }
  }

  return *this;
}

bool HeapIterator::operator==(const HeapIterator &other) const
{
  if (items.empty() && other.items.empty())
  {
    return true;
  }
  if (items.empty() || other.items.empty())
  {
    return false;
  }
  return items.top().key == other.items.top().key &&
         items.top().value == other.items.top().value;
}

bool HeapIterator::operator!=(const HeapIterator &other) const
{
  return !(*this == other);
}

bool HeapIterator::is_end() const { return items.empty(); }
