#pragma once

#include <functional>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>

class GuiEffectLogger
{
public:
    using Sink = std::function<void(const std::string&)>;

    explicit GuiEffectLogger(Sink sink)
        : sink_(std::move(sink))
    {
    }

    ~GuiEffectLogger()
    {
        flush();
    }

    template <typename T>
    GuiEffectLogger& operator<<(const T& value)
    {
        buffer_ << value;
        return *this;
    }

    GuiEffectLogger& operator<<(std::ostream& (*manip)(std::ostream&))
    {
        buffer_ << manip;
        if (manip == static_cast<std::ostream& (*)(std::ostream&)>(std::endl))
            flush();
        return *this;
    }

private:
    Sink sink_;
    std::ostringstream buffer_;

    void flush()
    {
        if (!sink_ || buffer_.str().empty())
            return;

        std::string text = buffer_.str();
        buffer_.str({});
        buffer_.clear();

        std::size_t start = 0;
        while (start < text.size())
        {
            const std::size_t end = text.find('\n', start);
            std::string line = text.substr(start, end == std::string::npos ? std::string::npos : end - start);

            if (!line.empty() && line.back() == '\r')
                line.pop_back();

            if (!line.empty())
                sink_(line);

            if (end == std::string::npos)
                break;

            start = end + 1;
        }
    }
};
