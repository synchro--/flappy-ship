#ifndef _LOG_HXX
#define _LOG_HXX

#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <types.hxx>
#include <unistd.h>

// pretty simple logging utility with printf-like formatting
// Levels are: INFO, ERROR, PANIC, defined in types.hxx

// log is already in use by std Library
namespace lg {

class Logger {
private:
  std::ostream &m_os;
  Level m_level;

public:
  // singleton function must be friend
  // friend Logger &get_logger();
  Logger(std::ostream &os = std::cerr, Level lv = Level::INFO);

  Level get_level();
  void set_level(Level lv);

  void vprint(const char *tag, Level lv, const char *fmt, std::va_list l);
  void print(const char *tag, Level lv, const char *fmt, ...);
  void vpanic(const char *tag, const char *fmt, std::va_list l);

  void i(const char *tag, const char *fmt, ...);
  void e(const char *tag, const char *fmt, ...);
  void panic(const char *tag, const char *fmt, ...);
};

// return the static global logger instance
Logger &global();

// global logger functions
Level get_level();
void set_level(Level lv);

void vprint(const char *tag, Level lv, const char *fmt, std::va_list l);
void print(const char *tag, Level lv, const char *fmt, ...);
void vpanic(const char *tag, Level lv, const char *fmt, std::va_list l);

void i(const char *tag, const char *fmt, ...);
void e(const char *tag, const char *fmt, ...);
// Panic: GET OUTTA HERE
void panic(const char *tag, const char *fmt, ...);
}

#endif
