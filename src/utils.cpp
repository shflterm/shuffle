#ifndef UTILS
#define UTILS

#include <fstream>
#include <string>
#include <vector>

using namespace std;

vector<string> split(string input, char delimiter) {
  vector<string> answer;
  stringstream ss(input);
  string temp;

  while (getline(ss, temp, delimiter)) {
    answer.push_back(temp);
  }

  return answer;
}

const string WHITESPACE = " \n\r\t\f\v";

string ltrim(const string &s) {
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == string::npos) ? "" : s.substr(start);
}

string rtrim(const string &s) {
  size_t end = s.find_last_not_of(WHITESPACE);
  return (end == string::npos) ? "" : s.substr(0, end + 1);
}

string trim(const string &s) { return rtrim(ltrim(s)); }

// https://github.com/guilhermeagostinelli/levenshtein/
int levenshteinDist(string word1, string word2) {
  int size1 = word1.size();
  int size2 = word2.size();
  int verif[size1 + 1][size2 + 1];  // Verification matrix i.e. 2D array which
  // will store the calculated distance.

  // If one of the words has zero length, the distance is equal to the size of
  // the other word.
  if (size1 == 0) return size2;
  if (size2 == 0) return size1;

  // Sets the first row and the first column of the verification matrix with the
  // numerical order from 0 to the length of each word.
  for (int i = 0; i <= size1; i++) verif[i][0] = i;
  for (int j = 0; j <= size2; j++) verif[0][j] = j;

  // Verification step / matrix filling.
  for (int i = 1; i <= size1; i++) {
    for (int j = 1; j <= size2; j++) {
      // Sets the modification cost.
      // 0 means no modification (i.e. equal letters) and 1 means that a
      // modification is needed (i.e. unequal letters).
      int cost = (word2[j - 1] == word1[i - 1]) ? 0 : 1;

      // Sets the current position of the matrix as the minimum value between a
      // (deletion), b (insertion) and c (substitution). a = the upper adjacent
      // value plus 1: verif[i - 1][j] + 1 b = the left adjacent value plus 1:
      // verif[i][j - 1] + 1 c = the upper left adjacent value plus the
      // modification cost: verif[i - 1][j - 1] + cost
      verif[i][j] = min(min(verif[i - 1][j] + 1, verif[i][j - 1] + 1),
                        verif[i - 1][j - 1] + cost);
    }
  }

  // The last position of the matrix will contain the Levenshtein distance.
  return verif[size1][size2];
}

string replace(string str, const string& from, const string& to) {
  size_t start_pos = 0;
  while((start_pos = str.find(from, start_pos)) != string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length();
  }
  return str;
}

string read_file(const string& path) {
  string res;

  ifstream openFile(path);
  if (openFile.is_open()) {
    string line;
    while (getline(openFile, line)) {
      res += line + "\n";
    }
    openFile.close();
  }

  return res;
}

#endif