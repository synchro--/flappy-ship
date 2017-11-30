#include "log.h"

#include <cstdarg>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <vector>

namespace lg {

const char *level_to_str(Level lv) {
  switch (lv) {
  case Level::INFO:
    return "I";

  case Level::ERROR:
    return "E";

  case Level::PANIC:
    return "PANIC:";

  default:
    // shouldn't arrive here
    std::cout << "Logging Level not recognized.\n";
    std::terminate();
  }
}

Logger::Logger(std::ostream &os, Level lv) : m_os(os), m_level(lv) {}

Level Logger::get_level() { return m_level; }

void Logger::set_level(Level lv) { m_level = lv; }

void Logger::vprint(const char *tag, Level lv, const char *fmt,
                    std::va_list l) {

  // if less important than current level exit
  if (lv < m_level) {
    return;
  }

  if (!tag) {
    tag = "<no tag>";
  }

  m_os << level_to_str(lv) << ": " << tag << ": ";

  char *str = nullptr;
  // let's just call vasprintf
  vasprintf(&str, fmt, l);

  m_os << str << std::endl;

  // free the pointer dude
  std::free(str);
}

void Logger::print(const char *tag, Level lv, const char *fmt, ...) {
  std::va_list ap;
  va_start(ap, fmt);

  vprint(tag, lv, fmt, ap);

  va_end(ap);
}

void Logger::vpanic(const char *tag, const char *fmt, std::va_list ap) {
  Level lv = Level::PANIC;
  vprint(tag, lv, fmt, ap);
  va_end(ap);

  m_os.flush();
  std::terminate();
}

void Logger::i(const char *tag, const char *fmt, ...) {
  std::va_list ap;
  va_start(ap, fmt);

  Level lv = Level::INFO;
  vprint(tag, lv, fmt, ap);

  va_end(ap);
}

void Logger::e(const char *tag, const char *fmt, ...) {
  std::va_list ap;
  va_start(ap, fmt);

  Level lv = Level::ERROR;
  vprint(tag, lv, fmt, ap);

  va_end(ap);
}

void Logger::panic(const char *tag, const char *fmt, ...) {
  std::va_list ap;
  va_start(ap, fmt);

  vpanic(tag, fmt, ap);
}

// log formatted text appending it to the specified file
void Logger::append(const char *tag, const char *file_name, const char *fmt,
                    ...) {
  std::va_list ap;
  va_start(ap, fmt);
  char *str = nullptr;
  // let's just call vasprintf
  vasprintf(&str, fmt, ap);

  std::ofstream outfile;

  outfile.open(file_name, std::ios_base::app);
  outfile << tag << ':' << str;
  // free the pointer dude
  std::free(str);
  va_end(ap);
}

// std::string version
void Logger::append(const char *tag, const char *file_name, std::string &str) {
  std::ofstream outfile;
  outfile.open(file_name, std::ios_base::app);
  outfile << tag << ':' << str;
}

std::vector<game::Entry> Logger::readRankingData(const char *file_name) {
  std::vector<game::Entry> ret;
  std::ifstream infile(file_name);
  std::string name;
  double time;

  while (infile >> name >> time) {
    ret.emplace_back(std::make_pair(name, time));
  }

  return ret;
}

void Logger::logRanking(const char *file_name,
                        const std::vector<game::Entry> &data) {
  const static auto MAX_ENTRIES = 5;
  std::ofstream outfile;
  outfile.open(file_name, std::ofstream::trunc);

  for (auto &entry : data) {
    std::ostringstream oss;
    oss << entry.first << " " << entry.second << std::endl;
    outfile << oss.str();
    // std::cout << oss.str();
  }
}

// *************** //
// Global Instance //

static Logger s_logger;

Logger &global() { return s_logger; }

Level get_level() { return s_logger.get_level(); }

void set_level(Level lv) { s_logger.set_level(lv); }

void print(const char *tag, Level lv, const char *fmt, ...) {
  std::va_list ap;
  va_start(ap, fmt);

  s_logger.vprint(tag, lv, fmt, ap);

  va_end(ap);
}

void i(const char *tag, const char *fmt, ...) {
  std::va_list ap;
  va_start(ap, fmt);

  Level lv = Level::INFO;
  s_logger.vprint(tag, lv, fmt, ap);

  va_end(ap);
}

void e(const char *tag, const char *fmt, ...) {
  std::va_list ap;
  va_start(ap, fmt);

  Level lv = Level::ERROR;
  s_logger.vprint(tag, lv, fmt, ap);

  va_end(ap);
}

void panic(const char *tag, const char *fmt, ...) {
  std::va_list ap;
  va_start(ap, fmt);

  s_logger.vpanic(tag, fmt, ap);
}

// log formatted text appending it to the specified file
void append(const char *tag, const char *file_name, const char *fmt, ...) {
  s_logger.append(tag, file_name, fmt);
}

// std::string version
void append(const char *tag, const char *file_name, std::string &str) {
  s_logger.append(tag, file_name, str);
}

std::vector<game::Entry> readRankingData(const char *file_name) {
  return s_logger.readRankingData(file_name);
}

void logRanking(const char *file_name, const std::vector<game::Entry> &data) {
  s_logger.logRanking(file_name, data);
}

} // namespace lg
