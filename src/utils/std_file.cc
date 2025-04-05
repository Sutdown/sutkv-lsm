#include "../../include/utils/std_file.h"

// 根据传入的 filename 和 create 标志打开文件
bool StdFile::open(const std::string &filename, bool create)
{
  filename_ = filename;

  if (create)
  {
    file_.open(filename, std::ios::in | std::ios::out | std::ios::binary |
                             std::ios::trunc);
  }
  else
  {
    file_.open(filename, std::ios::in | std::ios::out | std::ios::binary);
  }

  return file_.is_open();
}

// 调用 open 方法以创建模式打开文件，如果打开失败则抛出异常。然后将 buf 中的数据写入文件
bool StdFile::create(const std::string &filename, std::vector<uint8_t> &buf)
{
  if (!this->open(filename, true))
  {
    throw std::runtime_error("Failed to open file for writing");
  }
  write(0, buf.data(), buf.size());
  return true;
}

// 如果文件处于打开状态，先调用 sync 方法将文件内容同步到磁盘，然后关闭文件。
void StdFile::close()
{
  if (file_.is_open())
  {
    sync();
    file_.close();
  }
}

// 将文件指针移动到文件末尾，返回文件的大小
size_t StdFile::size()
{
  file_.seekg(0, std::ios::end);
  return file_.tellg();
}

// 从指定的 offset 位置开始读取 length 字节的数据到 std::vector<uint8_t> 中。如果读取失败，则抛出异常
std::vector<uint8_t> StdFile::read(size_t offset, size_t length)
{
  std::vector<uint8_t> buf(length);
  file_.seekg(offset, std::ios::beg);
  if (!file_.read(reinterpret_cast<char *>(buf.data()), length))
  {
    throw std::runtime_error("Failed to read from file");
  }
  return buf;
}

// 将指定 data 从 offset 位置开始写入文件，然后调用 sync 方法将数据同步到磁盘
bool StdFile::write(size_t offset, const void *data, size_t size)
{
  file_.seekg(offset, std::ios::beg);
  file_.write(static_cast<const char *>(data), size);
  this->sync();
  return true;
}

// 如果文件处于打开状态，调用 flush 方法将缓冲区的数据刷新到磁盘，然后返回操作是否成功的状态
bool StdFile::sync()
{
  if (!file_.is_open())
  {
    return false;
  }
  file_.flush();
  return file_.good();
}