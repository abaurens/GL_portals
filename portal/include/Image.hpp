#pragma once

#include <string>
#include <string_view>

#include "stb_image.h"

class Image
{
public:
  Image() noexcept;
  Image(Image &&mve) noexcept;
  ~Image();

  bool load(const std::string_view path, int expected_channels = 0);
  void clear();

  int width() const { return m_width; }
  int height() const { return m_height; }
  int channel_count() const { return m_channels; }
  const void *data() const { return m_data; }

private:
  int m_width = 0;
  int m_height = 0;
  int m_channels = 0;
  void *m_data = nullptr;
};