// A minimal C++ stream decorator: CountingInputStream.
// It keeps the std::istream interface and adds chars()/lines()/pos().
//
// Compile: g++ -std=c++17 -Wall -Wextra counting_stream.cpp -o counting_stream

#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>

class CountingBuf : public std::streambuf {
public:
    explicit CountingBuf(std::streambuf* src) : src_(src) {}

    std::streamsize chars() const { return chars_; }
    std::streamsize lines() const { return lines_; }
    std::streamsize pos() const { return pos_; }

protected:
    int_type underflow() override {
        return src_->sgetc();
    }

    int_type uflow() override {
        int_type c = src_->sbumpc();
        if (!traits_type::eq_int_type(c, traits_type::eof())) {
            char ch = traits_type::to_char_type(c);
            ++chars_;
            if (pos_++ == 0) ++lines_;
            if (ch == '\n') pos_ = 0;
        }
        return c;
    }

    std::streamsize xsgetn(char* s, std::streamsize n) override {
        std::streamsize done = 0;
        while (done < n) {
            int_type c = uflow();
            if (traits_type::eq_int_type(c, traits_type::eof())) break;
            s[done++] = traits_type::to_char_type(c);
        }
        return done;
    }

private:
    std::streambuf* src_;
    std::streamsize chars_ = 0;
    std::streamsize lines_ = 0;
    std::streamsize pos_ = 0;
};

class CountingInputStream : public std::istream {
public:
    explicit CountingInputStream(std::istream& in)
        : std::istream(nullptr), buf_(in.rdbuf()) {
        rdbuf(&buf_);
        clear();
    }

    std::streamsize chars() const { return buf_.chars(); }
    std::streamsize lines() const { return buf_.lines(); }
    std::streamsize pos() const { return buf_.pos(); }

private:
    CountingBuf buf_;
};

int main() {
    std::istringstream src("hello\nworld\n");
    CountingInputStream in(src);

    std::string line;
    while (std::getline(in, line)) {
        std::cout << "[" << line << "]\n";
    }

    std::cout << "chars=" << in.chars()
              << " lines=" << in.lines()
              << " pos=" << in.pos() << "\n";
    return 0;
}
