#pragma once

#include "server/utils/global_state.h"
#include "server/utils/result.h"
#include <string>
#include <optional>
#include <unordered_map>

class Config : public GlobalState<Config> {
  public:
    
    void set_allowed_origin(const std::string& origin);
    std::string get_allowed_origin() const;

    void load_config();
    std::optional<std::string> get_config(const std::string& key) const;
    void set_logger_options();
    

  private:
    std::optional<std::string> allowed_origin;
    std::unordered_map<std::string, std::string> config;


    

    Config();
    friend class GlobalState<Config>;
};

