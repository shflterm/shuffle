#include "utils/credit.h"

#include <string>
#include <sstream>
#include "version.h"
#include "utils/utils.h"
#include "term.h"

using namespace std;

string createCreditText() {
  stringstream ss;
  ss << color(FOREGROUND, Yellow) << "Shuffle " << SHUFFLE_VERSION.str() << endl;
  ss << "Github Repository: https://github.com/shflterm/shuffle" << endl;
  ss << endl;

  ss << color(FOREGROUND, Yellow) << "Created by: " << endl;
  for (const auto &item : CREATED_BY)
    ss << resetColor << "  - " << replace(item, "@", "https://github.com/") << endl;

  ss << color(FOREGROUND, Green) << "Contributors: " << endl;
  for (const auto &item : CONTRIBUTORS)
    ss << resetColor << "  - " << replace(item, "@", "https://github.com/") << endl;

  ss << color(FOREGROUND, Blue) << "Special Thanks: " << endl;👨‍💻
  for (const auto &item : SPECIAL_THANKS)
    ss << resetColor << "  - " << replace(item, "@", "https://github.com/") << endl;

  ss << color(FOREGROUND, Cyan) << "Open Source Libraries: " << endl;
  for (const auto &item : LIBRARIES)
    ss << resetColor << "  - " << item.first << " by " << replace(item.second, "@", "https://github.com/") << endl;

  ss << endl;

  ss << color(FOREGROUND_BRIGHT, Yellow) << "Thank you for using Shuffle!" << endl;
  ss<< color(FOREGROUND_BRIGHT, Black) << "If you have any issues, please let us know here: https://github.com/shflterm/shuffle/issues/new" << endl;

  return ss.str();
}