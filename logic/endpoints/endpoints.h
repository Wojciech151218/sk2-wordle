#pragma once

#include "server/server_method.h"
#include "logic/endpoints/request_bodies.h"
#include <memory>

extern ServerMethod<JoinRequest> join_method;
extern ServerMethod<StateRequest> state_method;
extern ServerMethod<GuessRequest> guess_method;
