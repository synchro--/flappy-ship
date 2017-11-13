#ifndef _LOG_H_
#define _LOG_H_

#include "types.h"
#include <cstdarg>
#include <iostream>
#include <vector>

// pretty simple logging utility with printf-like formatting
// Levels are: INFO, ERROR, PANIC, defined in typesh

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

  // file I/O
  void append(const char *tag, const char *file_name, const char *fmt, ...);
  void append(const char *tag, const char *file_name, std::string &str);
  std::vector<game::Entry> readRankingData(const char *file_name);
  void logRanking(const char *file_name, std::vector<game::Entry> const &data);
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

// file editing
void append(const char *file_name, const char *fmt, ...);
void append(const char *tag, const char *file_name, std::string &str);
std::vector<game::Entry> readRankingData(const char *file_name);
void logRanking(const char *file_name, std::vector<game::Entry> const &data);

} // namespace lg

#endif //_LOG_H__
