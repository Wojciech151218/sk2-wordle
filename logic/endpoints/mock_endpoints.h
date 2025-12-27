#pragma once

#include "server/server/server_method.h"
#include "logic/endpoints/request_bodies.h"
#include <memory>
#include "server/cron/cron.h"

extern std::unique_ptr<Cron> get_mock_game_cron();

extern ServerMethod<JoinRequest> mock_join_method;
extern ServerMethod<StateRequest> mock_state_method;
extern ServerMethod<GuessRequest> mock_guess_method;
extern ServerMethod<JoinRequest> mock_leave_method;
extern ServerMethod<StateRequest> mock_ready_method;

