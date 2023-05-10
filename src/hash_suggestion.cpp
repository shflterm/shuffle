#include <vector>

#include "hash_suggestion.h"
#include "utils/utils.h"
#include "console.h"
OpenAI oai;
Conversation convo;

bool initialized = false;

void initialize() {
  convo = Conversation();

  vector<string> commands;
  // TODO: This is for tests.
  commands.emplace_back("help: Print a list of commands");
  commands.emplace_back("clear: Clear console");
  commands.emplace_back("cd [dir]: Change directory");
  commands.emplace_back("list: Print list file of current directory");
  commands.emplace_back("mk [name]: Create a file or directory named [name]");
  commands.emplace_back("rm [name]: Delete a file or directory named [name]");
  commands.emplace_back("cp [from] [to]: Copy file [from] to [to].");
  commands.emplace_back("mv [from] [to]: Move file [from] to [to].");
  commands.emplace_back("dir: Print current directory");
  commands.emplace_back("find [target]: Search for files with the name [target] or containing [target]");
  commands.emplace_back("zip [file]: Compress a file (directory) [file] into zip format");
  commands.emplace_back("zip [zipFile]: Uncompress [zip_file], which is a file in .zip format");
  commands.emplace_back("open: Open the current directory in explorer");
  commands.emplace_back("open [file]: open [file] in explorer");
  commands.emplace_back("get [url]: Download file from [url].");
  commands.emplace_back("get [url] [newFile]: Download file from [url] and save to [newFile].");

  string systemData = "You are a bot that recommends which commands to use in certain situations."
                      "The command doesn't have to be a single line."
                      "If the user informs you of a specific situation or desired result, you can create a command to produce that result."
                      "You just have to tell the user what commands to input, followed by an explanation on the line below them."
                      "Do not put explanations in the middle of telling the command."
                      "Also, when outputting a command, it must start with '```` and end with '````."
                      "If a user asks a question about a language other than Shuffle (such as C++ or Java) or another topic, the answer is: \"Sorry, but I can only recommend the Shuffle command. I don't know much about the rest.\". Also, never give any other answer."
                      "Below is a list of commands you can use and what they do.";
  for (const auto &item : commands) systemData += item + "\n";

  convo.SetSystemData(systemData);

  initialized = true;
}

string callGptAI(const string &prompt) {
  if (!initialized) initialize();

  convo.AddUserData(prompt);
  if (oai.auth.SetKey("[secret]")) {
    try {
      auto fut = oai.ChatCompletion->create_async(
          "gpt-3.5-turbo", convo
      );

      // do other work...

      // check if the future is ready
      fut.wait();

      // get the contained response
      auto response = fut.get();

      // update our conversation with the response
      convo.Update(response);
      return convo.GetLastResponse();
    }
    catch (std::exception &e) {
      initialize();
      cout << "sorry. An error has occurred. I'll reset openai. please wait for a moment" << endl;
      return e.what();
    }
  } else {
    return "error";
  }
}

string createHashSuggestion(const string &prompt) {
  string res = callGptAI(prompt);
  string newRes;

  int countAccentGrave = 0;
  bool codeOpened = false;
  for (char c : res) {
    if (c == '`') countAccentGrave++;
    else {
      countAccentGrave = 0;
      newRes += c;
    }

    if (countAccentGrave == 3) {
      if (!codeOpened) {
        newRes += BG_GREEN;
      } else {
        newRes += RESET;
      }

      countAccentGrave = 0;
      codeOpened = !codeOpened;
    }
  }

  return "[BOT] " + newRes;
}

// I want to go to the upper directory and print out the file list of that directory.