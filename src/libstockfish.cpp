#include "libstockfish.h"

#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <streambuf>
#include <string>
#include <thread>
#include <condition_variable>
#include <queue>

#include "attacks.h"
#include "bitboard.h"
#include "misc.h"
#include "position.h"
#include "tune.h"
#include "uci.h"

static void (*g_output_callback)(const char*) = nullptr;

// Custom streambuf that calls the output callback line-by-line
class CallbackStreambuf : public std::streambuf {
    std::string buffer;
protected:
    int overflow(int c) override {
        if (c == EOF) return c;
        if (c == '\n') {
            if (g_output_callback && !buffer.empty())
                g_output_callback(buffer.c_str());
            buffer.clear();
        } else {
            buffer += static_cast<char>(c);
        }
        return c;
    }
    int sync() override {
        if (g_output_callback && !buffer.empty()) {
            g_output_callback(buffer.c_str());
            buffer.clear();
        }
        return 0;
    }
};

// Custom streambuf that reads from a thread-safe queue
class QueueStreambuf : public std::streambuf {
    std::queue<std::string> lines;
    std::mutex              mtx;
    std::condition_variable cv;
    std::string             current;
    size_t                  pos = 0;

public:
    void push(const std::string& line) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            lines.push(line + "\n");
        }
        cv.notify_one();
    }

protected:
    int underflow() override {
        if (pos >= current.size()) {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this] { return !lines.empty(); });
            current = lines.front();
            lines.pop();
            pos = 0;
        }
        return static_cast<unsigned char>(current[pos]);
    }

    int uflow() override {
        int c = underflow();
        if (c != EOF) pos++;
        return c;
    }
};

static CallbackStreambuf* g_cout_buf = nullptr;
static QueueStreambuf*    g_cin_buf  = nullptr;
static std::thread        g_engine_thread;
static bool               g_initialized = false;

extern "C" {

void sf_set_output_callback(void (*callback)(const char*)) {
    g_output_callback = callback;
}

void sf_init() {
    if (g_initialized) return;
    g_initialized = true;

    g_cout_buf = new CallbackStreambuf();
    g_cin_buf  = new QueueStreambuf();
    std::cout.rdbuf(g_cout_buf);
    std::cin.rdbuf(g_cin_buf);

    g_engine_thread = std::thread([] {
        Stockfish::Bitboards::init();
        Stockfish::Attacks::init();
        Stockfish::Position::init();

        char arg0[] = "stockfish";
        char* argv[] = {arg0, nullptr};
        auto uci = std::make_unique<Stockfish::UCIEngine>(1, argv);
        Stockfish::Tune::init(uci->engine_options());
        uci->loop();
    });
    g_engine_thread.detach();
}

void sf_command(const char* command) {
    if (g_cin_buf)
        g_cin_buf->push(std::string(command));
}

}
