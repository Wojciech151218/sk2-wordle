#pragma once

#include "server/http/server_method.h"
#include "logic/endpoints/request_bodies.h"
#include <memory>
#include "server/cron/cron.h"

extern std::unique_ptr<Cron> get_game_cron();

extern ServerMethod<JoinRequest> join_method;
extern ServerMethod<StateRequest> state_method;
extern ServerMethod<GuessRequest> guess_method;
extern ServerMethod<JoinRequest> leave_method;
extern ServerMethod<StateRequest> ready_method;