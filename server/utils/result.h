#pragma once

#include "server/utils/error.h"

#include <functional>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>

#include "server/utils/logger.h"


template <typename T>
class Result {
  private:
    std::optional<T> right;
    std::optional<Error> left;
    Logger& logger = Logger::instance();

    [[nodiscard]] Error current_error() const;

  public:
    Result(const T& value);
    Result(T&& value);
    Result(const Error& error);
    Result(Error&& error);

    Result(Result<T>&& other) noexcept = default;
    Result(const Result<T>&) = delete;

    static Result<int> from_bsd(int value, const std::string& error_message);

    bool is_ok() const;
    bool is_err() const;
    T unwrap(bool should_exit = true) const;
    T unwrap(bool should_exit = true);
    Error unwrap_err(bool should_exit = true) const;

    Result<T>& operator=(Result<T>&& other) {
        if (this != &other) {
            right = std::move(other.right);
            left = std::move(other.left);
        }
        return *this;
    }

    template <typename U>
    Result<U> chain(Result<U> result) const;

    template <typename U>
    Result<U> chain(const std::function<Result<U>(T)>& func) const;

    template <typename U>
    Result<U> chain(const std::function<U()>& func) const;

    Result<int> chain_from_bsd(int value, const std::string& error_message) const;

    template <typename U>
    Result<U> finally(const std::function<U()>& func) const;

    template <typename U>
    Result<U> finally(const std::function<U(T)>& func) const;

    const Result<T>& log_error() const &;
    Result<T> log_error() &&;

    const Result<T>& log_error(const std::string& error_message) const &;
    

    const Result<T>& log_debug() const &;
    Result<T> log_debug() &&;

    const Result<T>& log_warn() const &;
    Result<T> log_warn() &&;
};

template <typename T>
[[nodiscard]] inline Error Result<T>::current_error() const {
    //logger.error(left.value());
    return left.value();
}

template <typename T>
inline Result<T>::Result(const T& value) : right(value) {}

template <typename T>
inline Result<T>::Result(T&& value) : right(std::move(value)) {}

template <typename T>
inline Result<T>::Result(const Error& error) : left(error) {}

template <typename T>
inline Result<T>::Result(Error&& error) : left(std::move(error)) {}

template <typename T>
inline Result<int> Result<T>::from_bsd(int value, const std::string& error_message) {
    if (value < 0) {
        Error error(error_message);
        return Result<int>(error);
    }
    return Result<int>(value);
}

template <typename T>
inline bool Result<T>::is_ok() const {
    return right.has_value();
}

template <typename T>
inline bool Result<T>::is_err() const {
    return left.has_value();
}

template <typename T>
inline T Result<T>::unwrap(bool should_exit) const {
    if (is_ok()) {
        return right.value();
    }
    left.value().handle_error(should_exit);
    throw std::runtime_error("Attempted to unwrap error result");
}


template <typename T>
inline T Result<T>::unwrap(bool should_exit) {
    if (is_ok()) {
        return std::move(right.value());
    }
    left.value().handle_error(should_exit);
    throw std::runtime_error("Attempted to unwrap error result");
}

template <typename T>
inline Error Result<T>::unwrap_err(bool should_exit) const {
    if (is_err()) {
        return left.value();
    }
    throw std::runtime_error("Attempted to unwrap ok result");
}

template <typename T>
template <typename U>
inline Result<U> Result<T>::chain(Result<U> result) const {
    if (is_ok()) {
        return result;
    }
    return Result<U>(current_error());
}

template <typename T>
template <typename U>
inline Result<U> Result<T>::chain(const std::function<Result<U>(T)>& func) const {
    if (is_ok()) {
        return func(right.value());
    }

    return Result<U>(current_error());
}

template <typename T>
inline Result<int> Result<T>::chain_from_bsd(int value, const std::string& error_message) const {
    if (is_err()) {
        return Result<int>(current_error());
    }
    return Result<int>::from_bsd(value, error_message);
}

template <typename T>
template <typename U>
inline Result<U> Result<T>::finally(const std::function<U()>& func) const {
    if (is_ok()) {
        return Result<U>(func());
    }
    return Result<U>(current_error());
}

template <typename T>
template <typename U>
inline Result<U> Result<T>::finally(const std::function<U(T)>& func) const {
    if (is_ok()) {
        return Result<U>(func(right.value()));
    }

    return Result<U>(current_error());
}

template <typename T>
inline const Result<T>& Result<T>::log_error() const & {
    if (is_err()) {
        logger.error(left.value());
    }
    return *this;
}

template <typename T>
inline Result<T> Result<T>::log_error() && {
    if (is_err()) {
        logger.error(left.value());
    }
    return std::move(*this);
}

template <typename T>
inline const Result<T>& Result<T>::log_error(const std::string& error_message) const & {
    if (is_err()) {
        logger.error(error_message + " : " + left.value().get_message() );
    }
    return *this;
}

template <typename T>
inline const Result<T>& Result<T>::log_debug() const & {
    if (is_err()) {
        logger.debug(unwrap_err().get_message());
    }
    return *this;
}

template <typename T>
inline Result<T> Result<T>::log_debug() && {
    if (is_err()) {
        logger.debug(unwrap_err().get_message());
    }
    return std::move(*this);
}

template <typename T>
inline const Result<T>& Result<T>::log_warn() const & {
    if (is_err()) {
        logger.warn(left.value());
    }
    return *this;
}

template <typename T>
inline Result<T> Result<T>::log_warn() && {
    if (is_err()) {
        logger.warn(left.value());
    }
    return std::move(*this);
}