#pragma once
#include <string>

enum class err_type {
  LOGIC,
  LOGIC_MATH,
  LOGIC_IMAGE,

  IO,
  IO_LTIFF,
  IO_LRAW,
};

enum class err_severity { DEBUG, WARNING, ERROR, SEVERE };

class image_error {
  int c_error_;
  err_type type_;
  err_severity severity_;
  std::string message_;
  std::string extra_info_;

  image_error(int c_error, enum err_type type, enum err_severity severity,
              std::string message, std::string extra_info)
      : c_error_(c_error), type_(type), severity_(severity),
        message_(std::move(message)), extra_info_(std::move(extra_info)) {}

public:
  static image_error LOGIC(int c_error, enum err_severity err_severity,
                           std::string message, std::string extra_info) {
    return image_error(c_error, err_type::LOGIC, err_severity,
                       std::move(message), std::move(extra_info));
  }
  static image_error LOGIC_MATH(int c_error, enum err_severity severity,
                                std::string message, std::string extra_info) {
    return image_error(c_error, err_type::LOGIC_MATH, severity,
                       std::move(message), std::move(extra_info));
  }

  static image_error IO(int c_error, enum err_severity severity,
                        std::string message, std::string extra_info) {
    return image_error(c_error, err_type::IO, severity, std::move(message),
                       std::move(extra_info));
  }

  static image_error IO_LTIFF(int c_error, enum err_severity severity,
                              std::string message, std::string extra_info) {
    return image_error(c_error, err_type::IO_LTIFF, severity,
                       std::move(message), std::move(extra_info));
  }

  static image_error IO_LRAW(int c_error, enum err_severity severity,
                             std::string message, std::string extra_info) {
    return image_error(c_error, err_type::IO_LRAW, severity, std::move(message),
                       std::move(extra_info));
  }
};
