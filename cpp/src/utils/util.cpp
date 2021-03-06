
#include "util.h"

namespace neb {

std::string get_stdout_from_command(std::string &cmd) {
  std::string data;
  FILE *stream;
  const int max_buffer = 256;
  char buffer[max_buffer];
  cmd.append(" 2>&1");

  stream = popen(cmd.c_str(), "r");
  if (stream) {
    while (!feof(stream)) {
      if (fgets(buffer, max_buffer, stream) != NULL) {
        data.append(buffer);
      }
    }
    pclose(stream);
  }
  return data;
}

std::vector<std::string> split_by_comma(const std::string &str, char comma) {
  std::vector<std::string> v;
  std::stringstream ss(str);
  std::string token;

  while (getline(ss, token, comma)) {
    v.push_back(token);
  }
  return v;
}

void read_lines_from_file(const std::string &file,
                          std::vector<std::string> &lines) {
  std::ifstream f(file);
  std::string line;
  if (f.is_open()) {
    while (std::getline(f, line)) {
      lines.push_back(line);
    }
    f.close();
  }
}

bool is_number(const std::string &s) {
  return !s.empty() && std::find_if(s.begin(), s.end(), [](char ch) {
                         return !std::isdigit(ch);
                       }) == s.end();
}

/*
 * fill prefix zero to number at most $digit digits
 * */
std::string fill_prefix_zero(int number, int digit) {
  std::ostringstream out;
  out << std::internal << std::setfill('0') << std::setw(digit) << number;
  return out.str();
}

/*
 * convert ptime (contruct from string) in local time to ptime in utc time
 * */
boost::posix_time::ptime ptime_str_to_ptime(const std::string &ptime_str) {

  boost::posix_time::ptime time_local =
      boost::posix_time::time_from_string(ptime_str);

  typedef boost::date_time::local_adjustor<boost::posix_time::ptime, +8,
                                           boost::posix_time::no_dst>
      cn_eastern;
  return cn_eastern::local_to_utc(time_local);
}

time_t ptime_str_to_time_t(const std::string &ptime_str) {
  boost::posix_time::ptime time_utc = ptime_str_to_ptime(ptime_str);
  return boost::posix_time::to_time_t(time_utc);
}

std::string replace_month_str_to_month_of_year(std::string &simple_str) {

  std::string months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                          "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

  for (size_t i = 0; i < sizeof(months) / sizeof(months[0]); i++) {
    std::string::size_type pos = simple_str.find(months[i]);
    if (pos != simple_str.npos) {
      size_t month_of_year = i + 1;
      simple_str.replace(pos, months[i].length(),
                         fill_prefix_zero(month_of_year, 2));
      return simple_str;
    }
  }
  return std::string();
}

std::string
ptime_str_local_to_ptime_str_utc(const std::string &ptime_str_local) {
  boost::posix_time::ptime time_utc = ptime_str_to_ptime(ptime_str_local);
  std::string simple_str = boost::posix_time::to_simple_string(time_utc);
  return replace_month_str_to_month_of_year(simple_str);
}

time_t get_universal_timestamp() {
  boost::posix_time::ptime time_utc(
      boost::posix_time::second_clock::universal_time());
  return boost::posix_time::to_time_t(time_utc);
}

std::string time_t_to_date(time_t t) {
  boost::posix_time::ptime pt = boost::posix_time::from_time_t(t);
  boost::gregorian::date date = pt.date();
  std::ostringstream oss;
  oss << date.year();
  oss << date.month();
  oss << fill_prefix_zero(date.day(), 2);
  std::string s = oss.str();
  return replace_month_str_to_month_of_year(s);
}

} // namespace neb
