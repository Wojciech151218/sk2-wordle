#pragma once

#include "server/server_method.h"
#include "logic/word_request.h"
#include <memory>

extern ServerMethod<WordRequest> join_method;
extern ServerMethod<WordRequest> state_method;
extern ServerMethod<WordRequest> guess_method;
