#pragma once

#include <functional>
#include <memory>
#include <string>
#include <type_traits>

#include "server/http/http_enums.h"
#include "server/http/request_body.h"
#include "server/utils/error.h"
#include "server/utils/result.h"
#include "nlohmann/json.hpp"

class ServerMethodBase {
  public:
    virtual ~ServerMethodBase() = default;
    virtual std::string get_path() const = 0;
    virtual HttpMethod get_method() const = 0;
    virtual Result<nlohmann::json> handle_request(std::string raw_body) const = 0;
};

template <typename BodyType>
class ServerMethod : public ServerMethodBase {
    static_assert(std::is_base_of_v<RequestBody, BodyType>,
                  "BodyType must inherit from RequestBody");

  private:
    std::string path;
    HttpMethod method;
    std::function<Result<nlohmann::json>(const BodyType&)> handler;

  public:
    ServerMethod(std::string path, HttpMethod method,
                 std::function<Result<nlohmann::json>(const BodyType&)> handler)
        : path(std::move(path)), method(method), handler(std::move(handler)) {}

    std::string get_path() const override { return path; }

    HttpMethod get_method() const override { return method; }

    Result<nlohmann::json> handle_request(std::string raw_body) const override {
        nlohmann::json json_body;
        try {
            json_body = nlohmann::json::parse(raw_body);
        } catch (const nlohmann::json::parse_error& e) {
            return Result<nlohmann::json>(
                Error("Invalid JSON format: " + std::string(e.what()),
                      HttpStatusCode::BAD_REQUEST));
        }

        auto parse_result = BodyType().validate(json_body);
        if (parse_result.is_err()) {
            return Result<nlohmann::json>(parse_result.unwrap_err());
        }

        std::unique_ptr<RequestBody> validated_body = parse_result.unwrap();
        auto typed_body = dynamic_cast<BodyType*>(validated_body.get());
        if (typed_body == nullptr) {
            return Result<nlohmann::json>(
                Error("Invalid request body type", HttpStatusCode::BAD_REQUEST));
        }

        return handler(*typed_body);
    }
};