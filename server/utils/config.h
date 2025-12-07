#pragma once

#include "server/utils/global_state.h"

#include <string>
#include <optional>

class Config : public GlobalState<Config> {
  public:
    void set_allowed_origin(const std::string& origin);
    std::string get_allowed_origin() const;

  private:
    std::optional<std::string> allowed_origin;

    Config();
    friend class GlobalState<Config>;
};

