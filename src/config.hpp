#ifndef _config_hpp_INCLUDED
#define _config_hpp_INCLUDED

namespace CourseSAT {

class Options;

struct Config {

  static bool has (const char *);
  static bool set (Options &, const char *);
  static void usage ();

  static const char **begin ();
  static const char **end ();
};

} // namespace CourseSAT

#endif
