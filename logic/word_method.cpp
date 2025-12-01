#include "logic/word.h"
#include "server/server_method.h"


int counter = 0;

ServerMethod word_method = ServerMethod(
    "word", 
    new Word(), 
    [](const RequestBody& request) {
    auto word = dynamic_cast<const Word&>(request).word;
    counter++;
    return nlohmann::json({{"word", word}, {"counter", counter}});
});