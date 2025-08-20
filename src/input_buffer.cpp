#include <pimento/input_buffer.hpp>
#include <pimento/utils.hpp>

namespace pimento::tokenization {

InputBuffer::InputBuffer(std::istream &istream) : m_stream(istream) {
  m_stream.read(m_buffer.data(), static_cast<std::streamsize>(m_buffer.size()));
  m_num_chars = static_cast<size_t>(m_stream.gcount());

  if (m_num_chars == 0) {
    m_done = true;
    return;
  }

  m_total_bytes += m_num_chars;
}

[[nodiscard]] std::optional<char> InputBuffer::peek() noexcept {
  if (m_index >= m_num_chars) {
    read_chunk();
    if (m_done) { return {}; }
  }

  return m_buffer[m_index];
}

[[nodiscard]] std::optional<char> InputBuffer::consume() {
  if (m_index >= m_num_chars) {
    read_chunk();
    if (m_done) { return {}; }
    return m_buffer[m_index];
  }

  char curr = m_buffer[m_index];
  advance();

  return curr;
}

void InputBuffer::advance() {
  // End of chunk or EoF
  if (m_index >= m_num_chars) {
    read_chunk();
    return;
  }

  char current = m_buffer[m_index++];
  if (current == '\n') {
    auto &logger = utils::get_logger();
    logger.trace("Finished line: {} with {} columns", m_line, m_column);
    ++m_line;
    m_column = 0;
  } else {
    ++m_column;
  }

  ++m_offset;
}

[[nodiscard]] std::string InputBuffer::get(std::streamsize offset,
                                           size_t span) {
  std::string out;
  m_stream.clear();

  m_stream.seekg(offset, std::ios::beg);
  out.resize(span);
  m_stream.read(&out[0], span);

  return out;
}

void InputBuffer::read_chunk() {
  m_stream.read(m_buffer.data(), static_cast<std::streamsize>(m_buffer.size()));
  m_num_chars = static_cast<size_t>(m_stream.gcount());

  if (m_num_chars == 0) {
    m_done = true;
    return;
  }

  m_total_bytes += m_num_chars;
  ++m_total_chunks;
  m_index = 0;
}

} // namespace pimento::tokenization