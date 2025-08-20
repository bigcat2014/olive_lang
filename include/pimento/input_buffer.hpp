//! @file input_buffer.hpp
//! @brief Pimento file buffer.
//! @author Logan Thomas

#pragma once

#include <array>
#include <istream>
#include <optional>
#include <string>

namespace pimento::tokenization {

class InputBuffer {
public:
  //! @brief The size in bytes of the chunks to read from the input stream.
  constexpr static size_t BUFFER_SIZE = 19; //4096;

public:
  //! @brief Constructor for the InputBuffer
  //! @param istream The stream of characters to buffer.
  explicit InputBuffer(std::istream &istream);

  //! @brief Peek at the next character in the buffer without consuming it.
  //! @return The next character in the buffer.
  [[nodiscard]] std::optional<char> peek() noexcept;

  //! @brief Get the current character and advance the buffer.
  //! @return The current character in the buffer.
  [[nodiscard]] std::optional<char> consume();

  //! @brief Advance the file buffer.
  void advance();

  //! @brief Get the current offset in the input stream.
  //! @return Current offset in the input stream.
  [[nodiscard]] inline std::streamsize get_offset() { return m_offset; }

  //! @brief Get the current line of the input stream.
  //! @return The current line.
  [[nodiscard]] inline size_t get_current_line() { return m_line; }

  //! @brief Get the current column in the current line.
  //! @return The current column in the current line.
  [[nodiscard]] inline size_t get_current_column() { return m_column; }

  //! @brief Whether or not we are done traversing the input stream.
  //! @return True if done, false if there is more data.
  [[nodiscard]] inline bool done() { return m_done; }

  //! @brief Get characters from the input stream.
  //! @param offset The offset of the input stream to start at.
  //! @param span The number of characters to get.
  //! @return String of characters from the input stream starting at `offset`
  //! and grabbing `span` number of characters.
  [[nodiscard]] std::string get(std::streamsize offset, size_t span = 1);

private:
  void read_chunk();

private:
  //! @brief The input stream to tokenize.
  std::istream &m_stream;
  //! @brief The buffer to read characters into.
  std::array<char, BUFFER_SIZE> m_buffer;
  //! @brief The number of characters read into the buffer.
  size_t m_num_chars{0};
  //! @brief Total bytes read from the input stream.
  size_t m_total_bytes{0};
  //! @brief Total chunks of size BUFFER_SIZE read from the input stream.
  size_t m_total_chunks{0};
  //! @brief Current index in the buffer.
  size_t m_index{0};
  //! @brief The current line of the input stream.
  size_t m_line{0};
  //! @brief The current column of the current line of the input stream.
  size_t m_column{0};
  //! @brief The current offset in the input stream.
  std::streamsize m_offset{-1};
  //! @brief Whether or not we are done traversing the input stream.
  bool m_done{false};
};

} // namespace pimento::tokenization