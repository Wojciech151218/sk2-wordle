#include "logic/word_request.h"
#include "server/server_method.h"
#include "logic/wordle_types.h"

WordleState wordle_state = WordleState(10);

ServerMethod word_method = ServerMethod("word", new WordRequest(), [](const RequestBody& request) {
    auto word = dynamic_cast<const WordRequest&>(request).word;
    wordle_state.add_word(WordleWord::get_random_colors(word));
    return nlohmann::json(wordle_state);
});