#include "hash_suggestion.h"

#include <vector>

#include "console.h"
#include "commandmgr.h"

OpenAI oai;
Conversation convo;

bool initialized = false;

void initialize() {
  convo = Conversation();

  vector<string> hints;
  // TODO: This is for tests.
  for (const auto &item : commands) {
    Command &command = *item;
    string text = command.getName() + ": " + command.getDescription();
    hints.push_back(text);
  }

  string systemData = "You are a bot that recommends which commands to use in certain situations."
                      "The command doesn't have to be a single line."
                      "If the user informs you of a specific situation or desired result, you can create a command to produce that result."
                      "You just have to tell the user what commands to input, followed by an explanation on the line below them."
                      "Do not put explanations in the middle of telling the command."
                      "Also, when outputting a command, it must start with '```` and end with '````."
                      "If a user asks a question about a language other than Shuffle (such as C++ or Java) or another topic, the answer is: \"Sorry, but I can only recommend the Shuffle command. I don't know much about the rest.\". Also, never give any other answer."
                      "Below is a list of commands you can use and what they do.";
  for (const auto &item : hints) systemData += item + "\n";

  convo.SetSystemData(systemData);

  initialized = true;
}

string callGptAI(const string &prompt) {
  if (!initialized) initialize();

  convo.AddUserData(prompt);
  if (oai.auth.SetKey("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")) {
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