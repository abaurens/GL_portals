#define STB_IMAGE_IMPLEMENTATION
#include "Image.hpp"

#include <iostream>

Image::Image() noexcept : m_width(0), m_height(0), m_channels(0), m_data(nullptr)
{}

Image::Image(Image &&mve) noexcept : m_width(mve.m_width), m_height(mve.m_height), m_channels(mve.m_channels), m_data(mve.m_data)
{
  mve.m_width = 0;
  mve.m_height = 0;
  mve.m_channels = 0;
  mve.m_data = nullptr;
}

Image::~Image()
{
  clear();
}

bool Image::load(const std::string_view path, int expected_channels)
{
  clear();
  m_data = stbi_load(path.data(), &m_width, &m_height, &m_channels, expected_channels);

  if (!m_data)
  {
    std::cout << "Image loading error: " << stbi_failure_reason() << std::endl;
    return false;
  }

  if (m_channels < expected_channels)
  {
    clear();
    return false;
  }

  return true;
}

void Image::clear()
{
  if (m_data)
  {
    stbi_image_free(m_data);
    m_data = nullptr;
  }
  m_width = 0;
  m_height = 0;
  m_channels = 0;
}
