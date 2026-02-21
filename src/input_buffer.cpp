#include <pimento/input_buffer.hpp>
#include <pimento/utils.hpp>

namespace pimento::tokenization {

InputBuffer::InputBuffer(std::istream* istream)
    : mStream(istream)
{
    readChunk();
}

[[nodiscard]] char InputBuffer::peek() const noexcept
{
    return mBuffer[mIndex];
}

[[nodiscard]] char InputBuffer::consume()
{
    if (mIndex >= mNumChars) {
        readChunk();
        if (mDone) {
            return std::char_traits<char>::eof();
        }
    }

    const char curr = mBuffer[mIndex];
    if (curr == '\n') {
        auto& logger = utils::getLogger();
        logger.trace("Finished line: {} with {} columns", mLine, mColumn);
        ++mLine;
        mColumn = 0;
    }
    else {
        ++mColumn;
    }

    ++mIndex;
    ++mOffset;

    if (curr == std::char_traits<char>::eof()) {
        mDone = true;
    }
    return curr;
}

[[nodiscard]] std::string InputBuffer::get(size_t offset, size_t span)
{
    auto& logger = utils::getLogger();
    std::string out;

    // Check if the requested offset and span is within the current buffer before reading from the stream
    size_t chunkStart = (mTotalChunks - 1) * BUFFER_SIZE;
    size_t chunkEnd   = chunkStart + mNumChars;

    // The entire span is within the current buffer, no I/O needed
    if (offset >= chunkStart && (offset + span) <= chunkEnd) {
        logger.trace("Offset and span is in buffer, using cached value.");
        size_t localOffset = offset - chunkStart;
        out.assign(mBuffer.data() + localOffset, span);
        return out;
    }

    // Requested offset and span is not within our current buffer, we need to read from the stream
    logger.trace("Offset and span is not in buffer, reading from stream.");

    // Save state
    mStream->clear();
    std::streampos currentPos = mStream->tellg();

    mStream->seekg(static_cast<std::streamsize>(offset), std::ios::beg);
    out.resize(span);
    mStream->read(out.data(), static_cast<std::streamsize>(span));

    // Reset state
    mStream->clear();
    mStream->seekg(currentPos);

    return out;
}

void InputBuffer::readChunk()
{
    mStream->read(mBuffer.data(), static_cast<std::streamsize>(BUFFER_SIZE));
    auto charsRead = static_cast<size_t>(mStream->gcount());

    if (charsRead == 0) {
        mDone = true;
        return;
    }

    mNumChars = charsRead;
    mTotalBytes += mNumChars;
    auto& logger = utils::getLogger();
    logger.trace("Read {} bytes from chunk {}", mNumChars, mTotalChunks);
    ++mTotalChunks;
    mIndex = 0;
}

}  // namespace pimento::tokenization