/*
 * Due to Qt6 has some conflict with std.ixx, we changed this file, replaced the part of std.ixx, now the cmake config may outdated
 *
 * version: 1.3.1
 * owner: Sympsel
 * c++ standard: 23
 * support: linux / windows
 * license: MIT
 * compiler: clang++
 *
 * Build Requirements:
 * -------------------
 * CMake:
 *   - cmake_minimum_required(VERSION 4.2)
 *   - set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")  # before project()
 *   - set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")                          # before project(), clang only
 *   - set(CMAKE_CXX_MODULE_STD ON)
 *   - set(CMAKE_CXX_STANDARD 23)
 *   - set(CMAKE_CXX_EXTENSIONS ON)
 *   - Module file must be declared via: target_sources(<target> PUBLIC FILE_SET cxx_modules TYPE CXX_MODULES FILES Log.ixx)
 *
 * Compiler (Clang):
 *   - Clang 22+ with -stdlib=libc++
 *   - Requires libc++ installed: sudo pacman -S libc++ (Arch Linux)
 *
 * Build System:
 *   - Ninja (Make does not support C++ modules)
 *
 * Dependencies:
 *   - import std (C++23 standard library module, experimental)
 */
module;

#include <bitset>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <print>
#include <source_location>
#include <sstream>
#include <string>

#ifdef _WIN32
#include <process.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

export module symusic.utils.log;

#ifdef ERROR
#undef ERROR
#endif

namespace symlog {
    /**
    * @brief 日志级别，内部使用。
    * 因为该命名较为常用，因此封装保证使用 symlog 命名空间后，
    * 不会和外界自己的 Level 冲突
    *
    * 外部接口使用整数表示日志级别：
    * - 1 = DEBUG（调试）
    * - 2 = INFO（信息）
    * - 3 = WARN（警告）
    * - 4 = ERROR（错误）
    * - 5 = FATAL（致命）
    */
    enum class Level {
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };

    namespace Color {
        std::string RESET = "\033[0m";
        std::string BLACK = "\033[30m";
        std::string RED = "\033[31m";
        std::string GREEN = "\033[32m";
        std::string YELLOW = "\033[33m";
        std::string BLUE = "\033[34m";
        std::string MAGENTA = "\033[35m";
        std::string CYAN = "\033[36m";
        std::string WHITE = "\033[37m";
        std::string BOLD_BLACK = "\033[30;1m";
        std::string BOLD_RED = "\033[31;1m";
        std::string BOLD_GREEN = "\033[32;1m";
        std::string BOLD_YELLOW = "\033[33;1m";
        std::string BOLD_BLUE = "\033[34;1m";
        std::string BOLD_MAGENTA = "\033[35;1m";
        std::string BOLD_CYAN = "\033[36;1m";
        std::string BOLD_WHITE = "\033[37;1m";
    }

    class Utils {
    public:
        static std::string getCurrTime() {
            const time_t stamp = time(nullptr);
            tm dateTime{};
#ifdef _WIN32
            localtime_s(&dateTime, &stamp);
#else
            localtime_r(&stamp, &dateTime);
#endif
            return std::format("{:02}:{:02}:{:02}",
                               dateTime.tm_hour,
                               dateTime.tm_min,
                               dateTime.tm_sec
            );
        }

        static std::string getCurrDate() {
            const time_t stamp = time(nullptr);
            tm dateTime{};
#ifdef _WIN32
            localtime_s(&dateTime, &stamp);
#else
            localtime_r(&stamp, &dateTime);
#endif
            return std::format("{:04}-{:02}-{:02}",
                               dateTime.tm_year + 1900,
                               dateTime.tm_mon + 1,
                               dateTime.tm_mday
            );
        }

        static std::string logLevelToStr(const Level &level) {
            switch (level) {
                case Level::DEBUG: return "DEBUG";
                case Level::INFO: return "INFO";
                case Level::WARN: return "WARNING";
                case Level::ERROR: return "ERROR";
                case Level::FATAL: return "FATAL";
                default: return "UNKNOWN";
            }
        }

        static std::string logLevelToStr(int level) {
            return logLevelToStr(static_cast<Level>(level));
        }

        static std::string removeColorCodes(const std::string &msg) {
            std::string result;
            result.reserve(msg.size());
            bool inEscape = false;
            for (const char c: msg) {
                if (c == '\033') {
                    inEscape = true;
                    continue;
                }
                if (inEscape && c == 'm') {
                    inEscape = false;
                    continue;
                }
                if (!inEscape) {
                    result += c;
                }
            }
            return result;
        }

        static std::string getColorForLevel(const Level &level) {
            switch (level) {
                case Level::DEBUG: return Color::BOLD_CYAN;
                case Level::INFO: return Color::BOLD_BLUE;
                case Level::WARN: return Color::BOLD_YELLOW;
                case Level::ERROR: return Color::BOLD_RED;
                case Level::FATAL: return Color::BOLD_MAGENTA;
                default: return Color::RESET;
            }
        }

        static std::string getColorForLevel(int level) {
            return getColorForLevel(static_cast<Level>(level));
        }
    };

    export enum class LogElem : uint32_t {
        NONE = 0,
        TIMESTAMP = 1 << 0,
        DATE = 1 << 1,
        TIME = 1 << 2,
        LEVEL = 1 << 3,
        FILE_PATH = 1 << 4,
        FILE_NAME = 1 << 5,
        LINE_NUMBER = 1 << 6,
        FUNCTION_NAME = 1 << 7,
        THREAD_ID = 1 << 8,
        PROCESS_ID = 1 << 9,
        COLOR = 1 << 10,

        DEFAULT = TIME | LEVEL | FILE_NAME | LINE_NUMBER | COLOR,
        MINIMAL = LEVEL,
        DETAILED = DATE | TIME | LEVEL | FILE_PATH | LINE_NUMBER | FUNCTION_NAME | THREAD_ID | PROCESS_ID,
        ALL = 0xFFFFFFFF
    };

    export class ShowSetting {
    private:
        explicit ShowSetting(const LogElem elem = LogElem::DEFAULT) {
            set(elem);
        }

    public:
        static ShowSetting &getInstance() {
            static ShowSetting instance;
            return instance;
        }

        ShowSetting(const ShowSetting &) = delete;

        ShowSetting(ShowSetting &&) = delete;

        ShowSetting &operator=(const ShowSetting &) = delete;

        ShowSetting &operator=(ShowSetting &&) = delete;

        void set(LogElem elem) {
            _flags |= static_cast<uint32_t>(elem);
        }

        void unset(LogElem elem) {
            _flags &= ~static_cast<uint32_t>(elem);
        }

        [[nodiscard]] bool isEnabled(LogElem elem) const {
            return (_flags.to_ulong() & static_cast<uint32_t>(elem)) != 0;
        }

        ShowSetting &enable(const LogElem elem) {
            set(elem);
            return *this;
        }

        ShowSetting &disable(const LogElem elem) {
            unset(elem);
            return *this;
        }

        static ShowSetting minimal() {
            return ShowSetting(LogElem::MINIMAL);
        }

        static ShowSetting detailed() {
            return ShowSetting(LogElem::DETAILED);
        }

        static ShowSetting all() {
            return ShowSetting(LogElem::ALL);
        }

    private:
        std::bitset<32> _flags;
    };

    export class Log;

    export class LogConfig {
    public:
        LogConfig &enable(const LogElem elem) {
            ShowSetting::getInstance().enable(elem);
            return *this;
        }

        LogConfig &disable(const LogElem elem) {
            ShowSetting::getInstance().disable(elem);
            return *this;
        }

        LogConfig &showAll() {
            enable(LogElem::DATE)
                    .enable(LogElem::TIME)
                    .enable(LogElem::LEVEL)
                    .enable(LogElem::FILE_NAME)
                    .enable(LogElem::LINE_NUMBER)
                    .enable(LogElem::COLOR);
            return *this;
        }

        LogConfig &showMinimal() {
            disable(LogElem::DATE)
                    .disable(LogElem::TIME)
                    .disable(LogElem::FILE_NAME)
                    .disable(LogElem::LINE_NUMBER)
                    .disable(LogElem::PROCESS_ID)
                    .disable(LogElem::THREAD_ID)
                    .disable(LogElem::FILE_PATH)
                    .disable(LogElem::FUNCTION_NAME)
                    .enable(LogElem::LEVEL);
            return *this;
        }

        LogConfig &showDetailed() {
            enable(LogElem::DATE)
                    .enable(LogElem::TIME)
                    .enable(LogElem::LEVEL)
                    .enable(LogElem::FILE_PATH)
                    .enable(LogElem::LINE_NUMBER)
                    .enable(LogElem::PROCESS_ID)
                    .enable(LogElem::THREAD_ID)
                    .enable(LogElem::COLOR);
            return *this;
        }

        LogConfig &showTimeOnly() {
            disable(LogElem::DATE).enable(LogElem::TIME);
            return *this;
        }

        LogConfig &showDateOnly() {
            enable(LogElem::DATE).disable(LogElem::TIME);
            return *this;
        }

        LogConfig &showDateTime() {
            enable(LogElem::DATE).enable(LogElem::TIME);
            return *this;
        }

        LogConfig &withLevel(const bool enabled = true) {
            if (enabled) {
                enable(LogElem::LEVEL);
            } else {
                disable(LogElem::LEVEL);
            }
            return *this;
        }

        LogConfig &withColor(const bool enabled = true) {
            if (enabled) {
                enable(LogElem::COLOR);
            } else {
                disable(LogElem::COLOR);
            }
            return *this;
        }

        LogConfig &withFile(const bool fullPath = false) {
            if (fullPath) {
                enable(LogElem::FILE_PATH).disable(LogElem::FILE_NAME);
            } else {
                enable(LogElem::FILE_NAME).disable(LogElem::FILE_PATH);
            }
            return *this;
        }

        LogConfig &withLineNumber(const bool enabled = true) {
            if (enabled) {
                enable(LogElem::LINE_NUMBER);
            } else {
                disable(LogElem::LINE_NUMBER);
            }
            return *this;
        }

        LogConfig &withPID(const bool enabled = true) {
            if (enabled) {
                enable(LogElem::PROCESS_ID);
            } else {
                disable(LogElem::PROCESS_ID);
            }
            return *this;
        }

        LogConfig &withTID(const bool enabled = true) {
            if (enabled) {
                enable(LogElem::THREAD_ID);
            } else {
                disable(LogElem::THREAD_ID);
            }
            return *this;
        }

        static LogConfig &configure() {
            static LogConfig instance;
            return instance;
        }

        LogConfig &filterLogLevel(Level minLevel);

        LogConfig &filterLogLevel(int minLevel);
    };

    class OutputStrategy {
    public:
        virtual ~OutputStrategy() = default;

        virtual void sync(const std::string &) = 0;

        virtual void setColorEnabled(bool) = 0;

        [[nodiscard]] virtual bool isColorEnabled() const = 0;

        virtual void flush() = 0;

    protected:
        std::mutex _mutex;
    };

    class ToConsole : public OutputStrategy {
    public:
        explicit ToConsole(const bool colorEnabled = true)
            : _colorEnabled(colorEnabled) {
        }

        void sync(const std::string &logMsg) override {
            std::lock_guard locker(_mutex);
            std::println("{}", logMsg);
        }

        void setColorEnabled(const bool enabled) override {
            std::lock_guard locker(_mutex);
            _colorEnabled = enabled;
        }

        [[nodiscard]] bool isColorEnabled() const override {
            return _colorEnabled;
        }

        void flush() override {
        }

    private:
        bool _colorEnabled;
    };

    class ToFile : public OutputStrategy {
    private:
        std::string getFilenameHelper() const {
            if (_filename.empty()) {
                return std::format("{}.symlog", _currDate);
            }

            if (const size_t dot_pos = _filename.find_last_of('.');
                dot_pos != std::string::npos) {
                const std::string name = _filename.substr(0, dot_pos);
                const std::string ext = _filename.substr(dot_pos);
                return std::format("{}_{}{}", name, _currDate, ext);
            }
            return std::format("{}_{}.symlog", _filename, _currDate);
        }

        void openLogFile() {
            if (_ofs.is_open()) {
                _ofs.close();
            }

            try {
                if (!std::filesystem::exists(_dir)) {
                    std::filesystem::create_directories(_dir);
                }

                const std::filesystem::path dirpath = std::filesystem::absolute(_dir);
                const std::filesystem::path filepath = dirpath / getFilenameHelper();

                _ofs.open(filepath, std::ios::app);
                if (!_ofs.is_open()) {
                    std::println(stderr, "[Logger Error] Failed to open log file: {}", filepath.string());
                }
            } catch (const std::filesystem::filesystem_error &e) {
                std::println(stderr, "[Logger Error] Filesystem error: {}", e.what());
            }
        }

    public:
        explicit ToFile(
            std::string dir = "./log",
            std::string filename = ""
        ) : _dir(std::move(dir)),
            _filename(std::move(filename)),
            _currDate(Utils::getCurrDate()) {
            std::lock_guard<std::mutex> locker(_mutex);
            if (!std::filesystem::exists(_dir)) {
                try {
                    std::filesystem::create_directories(_dir);
                } catch (const std::filesystem::filesystem_error &e) {
                    std::println(stderr, "[Logger Error] Failed to create log directory: {}", e.what());
                }
            }
            openLogFile();
        }

        ~ToFile() override {
            std::lock_guard<std::mutex> locker(_mutex);
            if (_ofs.is_open()) {
                _ofs.close();
            }
        }

        void sync(const std::string &logMsg) override {
            std::lock_guard locker(_mutex);

            if (const std::string today = Utils::getCurrDate(); today != _currDate) {
                _currDate = today;
                openLogFile();
            }

            if (_ofs.is_open()) {
                const std::string cleanMsg = Utils::removeColorCodes(logMsg);
                _ofs << cleanMsg << std::endl;
            }
        }

        void setColorEnabled(const bool enabled) override {
        }

        bool isColorEnabled() const override {
            return false;
        }

        void flush() override {
            std::lock_guard locker(_mutex);
            if (_ofs.is_open()) {
                _ofs.flush();
            }
        }

    private:
        std::string _dir;
        std::string _filename;
        std::string _currDate;
        std::ofstream _ofs;
    };

    class ToBothFileAndConsole : public OutputStrategy {
    public:
        explicit ToBothFileAndConsole(
            std::unique_ptr<ToConsole> console,
            std::unique_ptr<ToFile> file
        ) : _console(std::move(console)), _file(std::move(file)) {
        }

        ~ToBothFileAndConsole() override = default;

        void sync(const std::string &logMsg) override {
            _console->sync(logMsg);
            _file->sync(logMsg);
        }

        void setColorEnabled(const bool enabled) override {
            _console->setColorEnabled(enabled);
        }

        [[nodiscard]] bool isColorEnabled() const override {
            return _console->isColorEnabled();
        }

        void flush() override {
            _console->flush();
            _file->flush();
        }

    private:
        std::unique_ptr<ToConsole> _console;
        std::unique_ptr<ToFile> _file;
    };

    export class Log {
    public:
        enum class OutputTo {
            CONSOLE,
            FILE,
            BOTH
        };

        struct Head {
            int _level;
            std::string _date;
            std::string _time;
            std::string _filename;
            int _line;
            int _pid;
            int _tid;
        };

        class Msg {
        private:
            void buildMessage() {
                std::stringstream ss;
                const auto &setting = ShowSetting::getInstance();

                if (setting.isEnabled(LogElem::COLOR)) {
                    ss << Utils::getColorForLevel(_head._level);
                }

                ss << "[";

                const bool hasDate = setting.isEnabled(LogElem::DATE);
                const bool hasTime = setting.isEnabled(LogElem::TIME);

                if (hasDate || hasTime) {
                    if (hasDate && hasTime) {
                        ss << _head._date << " " << _head._time;
                    } else if (hasDate) {
                        ss << _head._date;
                    } else {
                        ss << _head._time;
                    }
                    ss << " | ";
                }

                if (setting.isEnabled(LogElem::LEVEL)) {
                    ss << Utils::logLevelToStr(_head._level) << " | ";
                }

                if (setting.isEnabled(LogElem::PROCESS_ID)) {
                    ss << "PID:" << _head._pid << " | ";
                }

                if (setting.isEnabled(LogElem::THREAD_ID)) {
                    ss << "TID:" << _head._tid << " | ";
                }

                if (setting.isEnabled(LogElem::FILE_PATH)) {
                    ss << _head._filename;
                } else if (setting.isEnabled(LogElem::FILE_NAME)) {
                    ss << std::filesystem::path(_head._filename).filename().string();
                }

                if (setting.isEnabled(LogElem::LINE_NUMBER)) {
                    ss << ":" << _head._line;
                }

                if (ss.str().ends_with(" | ")) {
                    ss.seekp(-3, std::ios_base::end);
                }
                ss << "] ";

                _formattedHeader = ss.str();
            }

        public:
            Msg(Head head, std::string text)
                : _head(std::move(head)), _body(std::move(text)) {
                buildMessage();
            }

            ~Msg() {
                const auto &instance = getInstance();
                if (_head._level >= instance.getLogLevelFilter()) {
                    const std::string fullMessage = _formattedHeader + _body;
                    instance.sync(fullMessage);
                }
            }

            template<typename T>
            Msg &operator<<(const T &value) {
                _body += std::to_string(value);
                return *this;
            }

            Msg &operator<<(const std::string &value) {
                _body += value;
                return *this;
            }

            Msg &operator<<(const char *value) {
                _body += value;
                return *this;
            }

        private:
            Head _head;
            std::string _formattedHeader;
            std::string _body;
        };

    private:
        void sync(const std::string &msg) const {
            std::lock_guard<std::mutex> locker(_mutex);
            if (_output) {
                _output->sync(msg);
            }
        }

        explicit Log(const OutputTo where = OutputTo::CONSOLE)
            : _where(where), _filter(static_cast<int>(Level::DEBUG)) {
            setOutputTo(where);
        }

    public:
        static Log &getInstance() {
            static Log instance;
            return instance;
        }

        ~Log() = default;

        Log(const Log &) = delete;

        Log &operator=(const Log &) = delete;

        Log(Log &&) = delete;

        Log &operator=(Log &&) = delete;

        int getLogLevelFilter() const {
            return _filter;
        }

        void setLogLevelFilter(int minLevel) {
            if (minLevel > 5) {
                minLevel = 5;
            }
            if (minLevel < 0) {
                minLevel = 0;
            }
            _filter = minLevel;
        }

        void outputToConsole(bool colorEnabled = true) {
            std::lock_guard<std::mutex> locker(_mutex);
            _output = std::make_unique<ToConsole>(colorEnabled);
        }

        void outputToFile(const std::string &dir = "./log", const std::string &filename = "") {
            std::lock_guard<std::mutex> locker(_mutex);
            _output = std::make_unique<ToFile>(dir, filename);
        }

        void outputToBoth(
            const std::string &dir = "./log",
            const std::string &filename = "",
            bool colorEnabled = true) {
            std::lock_guard<std::mutex> locker(_mutex);
            _output = std::make_unique<ToBothFileAndConsole>(
                std::make_unique<ToConsole>(colorEnabled),
                std::make_unique<ToFile>(dir, filename)
            );
        }

        void setOutputTo(const OutputTo where, bool colorEnabled = true) {
            std::lock_guard<std::mutex> locker(_mutex);
            _where = where;
            switch (where) {
                case OutputTo::CONSOLE:
                    _output = std::make_unique<ToConsole>(colorEnabled);
                    break;
                case OutputTo::FILE:
                    _output = std::make_unique<ToFile>();
                    break;
                case OutputTo::BOTH:
                    _output = std::make_unique<ToBothFileAndConsole>(
                        std::make_unique<ToConsole>(colorEnabled), std::make_unique<ToFile>()
                    );
                    break;
            }
        }

        Msg operator()(const int level,
                       const std::string &filename,
                       const int line) const {
            return {
                {
                    level,
                    Utils::getCurrDate(),
                    Utils::getCurrTime(),
                    filename,
                    line,
#ifdef _WIN32
                    static_cast<int>(GetCurrentProcessId()),
                            static_cast<int>(GetCurrentThreadId())
#else
                    getpid(),
                    gettid()
#endif
                },
                {}
            };
        }

    private:
        OutputTo _where;
        mutable std::mutex _mutex;
        std::unique_ptr<OutputStrategy> _output;
        int _filter;
    };

    /**
     *
     * @param minLevel min level of log to show
     */
    inline LogConfig &LogConfig::filterLogLevel(const Level minLevel) {
        Log::getInstance().setLogLevelFilter(static_cast<int>(minLevel));
        return *this;
    }

    /**
    *
    * @param minLevel 最低日志级别（1=DEBUG, 2=INFO, 3=WARN, 4=ERROR, 5=FATAL）
    */
    inline LogConfig &LogConfig::filterLogLevel(const int minLevel) {
        Log::getInstance().setLogLevelFilter(minLevel);
        return *this;
    }

    inline Log::Msg log(const int level, const std::source_location& loc = std::source_location::current()) {
        return Log::getInstance()(level, loc.file_name(), static_cast<int>(loc.line()));
    }

    /** @brief 记录 DEBUG 级别日志（级别=1） */
    export inline Log::Msg logDebug(const std::source_location& loc = std::source_location::current()) {
        return log(static_cast<int>(Level::DEBUG), loc);
    }

    /** @brief 记录 INFO 级别日志（级别=2） */
    export inline Log::Msg logInfo(const std::source_location& loc = std::source_location::current()) {
        return log(static_cast<int>(Level::INFO), loc);
    }

    /** @brief 记录 WARN 级别日志（级别=3） */
    export inline Log::Msg logWarn(const std::source_location& loc = std::source_location::current()) {
        return log(static_cast<int>(Level::WARN), loc);
    }

    /** @brief 记录 ERROR 级别日志（级别=4） */
    export inline Log::Msg logError(const std::source_location& loc = std::source_location::current()) {
        return log(static_cast<int>(Level::ERROR), loc);
    }

    /** @brief 记录 FATAL 级别日志（级别=5） */
    export inline Log::Msg logFatal(const std::source_location& loc = std::source_location::current()) {
        return log(static_cast<int>(Level::FATAL), loc);
    }

    export inline void useConsoleLog(const bool colorEnabled = true) {
        Log::getInstance().outputToConsole(colorEnabled);
    }

    export inline void useFileLog(const std::string &dir = "./log", const std::string &filename = "") {
        Log::getInstance().outputToFile(dir, filename);
    }

    export inline void useCombinedLog(const std::string &dir = "./log",
                                      const std::string &filename = "",
                                      const bool colorEnabled = true) {
        Log::getInstance().outputToBoth(dir, filename, colorEnabled);
    }

    export inline void setOutputTo(const Log::OutputTo where, const bool colorEnabled = true) {
        Log::getInstance().setOutputTo(where, colorEnabled);
    }

    export inline void enableLogElement(const LogElem elem) {
        ShowSetting::getInstance().enable(elem);
    }

    export inline void disableLogElement(const LogElem elem) {
        ShowSetting::getInstance().disable(elem);
    }

    export inline LogConfig &logConfig() {
        return LogConfig::configure();
    }


     /**
     * @brief 设置日志过滤级别
     * @param minLevel 最低日志级别（1=DEBUG, 2=INFO, 3=WARN, 4=ERROR, 5=FATAL）
     */
    export inline LogConfig &filterLogLevel(const int minLevel) {
        return logConfig().filterLogLevel(static_cast<Level>(minLevel));
    }
}

// Macros - 宏定义
#define LOG(level) symlog::Log::getInstance()(level, __FILE__, __LINE__)
#define LOG_DEBUG() LOG(symlog::Level::DEBUG)
#define LOG_INFO()  LOG(symlog::Level::INFO)
#define LOG_WARN()  LOG(symlog::Level::WARN)
#define LOG_ERROR() LOG(symlog::Level::ERROR)
#define LOG_FATAL() LOG(symlog::Level::FATAL)

#define USE_CONSOLE_LOG(colorEnabled) symlog::Log::getInstance().outputToConsole(colorEnabled)
#define USE_FILE_LOG(dir, filename) symlog::Log::getInstance().outputToFile(dir, filename)
#define USE_COMBINED_LOG(dir, filename, colorEnabled) symlog::Log::getInstance().outputToBoth(dir, filename, colorEnabled)
#define SET_OUTPUT_TO(where, colorEnabled) symlog::Log::getInstance().setOutputTo(where, colorEnabled)

#define ENABLE_LOG_ELEMENT(elem) symlog::ShowSetting::getInstance().enable(elem)
#define DISABLE_LOG_ELEMENT(elem) symlog::ShowSetting::getInstance().disable(elem)
#define LOG_CONFIG symlog::LogConfig::configure()

#define LOG_FILTER_DEBUG() LOG_CONFIG.filterLogLevel(symlog::Level::DEBUG)
#define LOG_FILTER_INFO() LOG_CONFIG.filterLogLevel(symlog::Level::INFO)
#define LOG_FILTER_WARN() LOG_CONFIG.filterLogLevel(symlog::Level::WARN)
#define LOG_FILTER_ERROR() LOG_CONFIG.filterLogLevel(symlog::Level::ERROR)
#define LOG_FILTER_FATAL() LOG_CONFIG.filterLogLevel(symlog::Level::FATAL)

export using namespace symlog;
