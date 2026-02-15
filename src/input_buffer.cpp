#include <pimento/input_buffer.hpp>
#include <pimento/utils.hpp>

namespace pimento::tokenization {

InputBuffer::InputBuffer(std::istream* istream)
    : mStream(istream)
{
    readChunk();
}

[[nodiscard]] std::optional<char> InputBuffer::peek() noexcept
{
    if (mIndex >= mNumChars) {
        // TODO(lthomas): I don't love that this technically advances the lexer, but
        // it fixes the issues with tokens that bridge between chunks...
        readChunk();
        if (mDone) {
            return {};
        }
    }

    return mBuffer[mIndex];
}

[[nodiscard]] std::optional<char> InputBuffer::consume()
{
    if (mIndex >= mNumChars) {
        readChunk();
        if (mDone) {
            return {};
        }
    }

    const char curr = mBuffer[mIndex];
    advance();

    return curr;
}

void InputBuffer::advance()
{
    // End of chunk or EoF
    if (mIndex >= mNumChars) {
        readChunk();
        return;
    }

    char const current = mBuffer[mIndex++];
    if (current == '\n') {
        auto& logger = utils::getLogger();
        logger.trace("Finished line: {} with {} columns", mLine, mColumn);
        ++mLine;
        mColumn = 0;
    }
    else {
        ++mColumn;
    }

    ++mOffset;
}

[[nodiscard]] std::string InputBuffer::get(size_t offset, size_t span)
{
    std::string out;
    mStream->clear();

    mStream->seekg(static_cast<std::streamsize>(offset), std::ios::beg);
    out.resize(span);
    mStream->read(out.data(), static_cast<std::streamsize>(span));

    return out;
}

void InputBuffer::readChunk()
{
    mStream->read(mBuffer.data(), static_cast<std::streamsize>(mBuffer.size()));
    mNumChars = static_cast<size_t>(mStream->gcount());

    if (mNumChars == 0) {
        mDone = true;
        return;
    }

    mTotalBytes += mNumChars;
    ++mTotalChunks;
    mIndex = 0;
}

}  // namespace pimento::tokenization