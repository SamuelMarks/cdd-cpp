/// @file error.hpp
/// @brief Strongly typed error codes, error structures, and Result aliases.

#pragma once
#include <expected>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>

/// @namespace cdd_cpp::utils
/// @brief Core utility types and functions for cdd-cpp.
namespace cdd_cpp::utils {

/// @brief Strongly typed error codes covering all failure domains in cdd-cpp.
enum class ErrorCode {
  JsonParseError,           ///< Error occurred while parsing JSON via simdjson.
  InvalidOpenApiDocument,   ///< The document is not a valid OpenAPI object.
  MissingRequiredField,     ///< A required field is missing from the object.
  InvalidFieldType,         ///< A field has an unexpected JSON or C++ type.
  UnsupportedSpecVersion,   ///< The OpenAPI/Swagger version is not supported.
  HttpInitFailed,           ///< Failed to initialize the HTTP client (libcurl).
  HttpPerformFailed,        ///< HTTP request execution failed.
  WasiUnsupportedOperation, ///< Operation is not supported in WASI/Emscripten.
  FileReadError,            ///< Failed to open or read a file from disk.
  FileWriteError,           ///< Failed to open or write a file to disk.
  DirectoryIterationError,  ///< Failed to iterate over a filesystem directory.
  CppAstParseError,         ///< Failed to parse C++ source into an AST.
  CliArgumentError,         ///< Invalid or missing CLI command-line arguments.
  JsonRpcInvalidRequest,    ///< Malformed JSON-RPC 2.0 request envelope.
  JsonRpcMethodNotFound,    ///< Requested JSON-RPC method or tool not found.
  JsonRpcInvalidParams,     ///< Invalid or missing JSON-RPC method parameters.
  McpProtocolError          ///< Model Context Protocol domain error.
};

/// @brief Convert an ErrorCode enumerator to its human-readable string name.
/// @param code The ErrorCode value to convert.
/// @return A string view representing the ErrorCode name.
constexpr std::string_view error_code_to_string(ErrorCode code) noexcept {
  switch (code) {
  case ErrorCode::JsonParseError:
    return "JsonParseError";
  case ErrorCode::InvalidOpenApiDocument:
    return "InvalidOpenApiDocument";
  case ErrorCode::MissingRequiredField:
    return "MissingRequiredField";
  case ErrorCode::InvalidFieldType:
    return "InvalidFieldType";
  case ErrorCode::UnsupportedSpecVersion:
    return "UnsupportedSpecVersion";
  case ErrorCode::HttpInitFailed:
    return "HttpInitFailed";
  case ErrorCode::HttpPerformFailed:
    return "HttpPerformFailed";
  case ErrorCode::WasiUnsupportedOperation:
    return "WasiUnsupportedOperation";
  case ErrorCode::FileReadError:
    return "FileReadError";
  case ErrorCode::FileWriteError:
    return "FileWriteError";
  case ErrorCode::DirectoryIterationError:
    return "DirectoryIterationError";
  case ErrorCode::CppAstParseError:
    return "CppAstParseError";
  case ErrorCode::CliArgumentError:
    return "CliArgumentError";
  case ErrorCode::JsonRpcInvalidRequest:
    return "JsonRpcInvalidRequest";
  case ErrorCode::JsonRpcMethodNotFound:
    return "JsonRpcMethodNotFound";
  case ErrorCode::JsonRpcInvalidParams:
    return "JsonRpcInvalidParams";
  case ErrorCode::McpProtocolError:
    return "McpProtocolError";
  }
  return "UnknownError";
}

/// @brief Structured error containing a typed code, message, and context path.
struct Error {
  ErrorCode code =
      ErrorCode::InvalidOpenApiDocument; ///< Categorized error code.
  std::string message;      ///< Detailed human-readable error message.
  std::string context_path; ///< Optional JSON pointer or file/line context.

  /// @brief Default constructor.
  Error() = default;

  /// @brief Construct an Error from a message string (defaults to
  /// InvalidOpenApiDocument).
  /// @param msg The human-readable error message.
  // NOLINTNEXTLINE(runtime/explicit)
  Error(std::string msg) noexcept
      : code(ErrorCode::InvalidOpenApiDocument), message(std::move(msg)) {}

  /// @brief Construct an Error from a C-string message.
  /// @param msg The human-readable C-string error message.
  // NOLINTNEXTLINE(runtime/explicit)
  Error(const char *msg) noexcept
      : code(ErrorCode::InvalidOpenApiDocument), message(msg ? msg : "") {}

  /// @brief Construct an Error with a specific ErrorCode, message, and optional
  /// context path.
  /// @param c The specific ErrorCode enumerator.
  /// @param msg The human-readable error message.
  /// @param ctx Optional context path (e.g., JSON path or filename).
  Error(ErrorCode c, std::string msg, std::string ctx = "") noexcept
      : code(c), message(std::move(msg)), context_path(std::move(ctx)) {}

  /// @brief Implicit conversion to std::string returning the error message.
  /// @return Reference to the underlying message string.
  operator const std::string &() const noexcept { return message; }

  /// @brief Retrieve the C-string representation of the error message.
  /// @return Null-terminated C-string pointer.
  const char *c_str() const noexcept { return message.c_str(); }

  /// @brief Check if the error message is empty.
  /// @return True if message is empty, false otherwise.
  bool empty() const noexcept { return message.empty(); }

  /// @brief Search for a substring within the error message.
  /// @param str Substring to search for.
  /// @param pos Starting position for the search.
  /// @return Index of the substring or std::string::npos.
  size_t find(std::string_view str, size_t pos = 0) const noexcept {
    return message.find(str, pos);
  }

  /// @brief Format the full error including code, context path, and message.
  /// @return Formatted diagnostic string.
  std::string format() const noexcept {
    std::string out = std::string(error_code_to_string(code));
    if (!context_path.empty()) {
      out += " [" + context_path + "]";
    }
    out += ": " + message;
    return out;
  }

  /// @brief Equality comparison between two Error instances.
  /// @param other The other Error to compare with.
  /// @return True if code, message, and context_path are equal.
  bool operator==(const Error &other) const noexcept = default;

  /// @brief Equality comparison between an Error and a string view (compares
  /// message).
  /// @param msg The message string view to compare against.
  /// @return True if the error message matches msg.
  bool operator==(std::string_view msg) const noexcept {
    return message == msg;
  }

  /// @briefEquality comparison between an Error and a C-string (compares
  /// message).
  /// @param msg The C-string to compare against.
  /// @return True if the error message matches msg.
  bool operator==(const char *msg) const noexcept {
    return message == (msg ? msg : "");
  }
};

/// @brief Append an Error's message to a std::string.
/// @param lhs The left-hand string.
/// @param rhs The right-hand Error.
/// @return Concatenated string.
inline std::string operator+(const std::string &lhs, const Error &rhs) {
  return lhs + rhs.message;
}

/// @brief Append a C-string and an Error's message.
/// @param lhs The left-hand C-string.
/// @param rhs The right-hand Error.
/// @return Concatenated string.
inline std::string operator+(const char *lhs, const Error &rhs) {
  return std::string(lhs ? lhs : "") + rhs.message;
}

/// @brief Stream insertion operator for Error.
/// @param os Output stream.
/// @param err The Error to write.
/// @return Reference to the output stream.
inline std::ostream &operator<<(std::ostream &os, const Error &err) {
  os << err.message;
  return os;
}

/// @brief Standard Result alias wrapping std::expected<T, Error>.
template <typename T> using Result = std::expected<T, Error>;

/// @brief Helper to construct an unexpected Error with an ErrorCode and
/// message.
/// @param code The ErrorCode enumerator.
/// @param message The diagnostic message.
/// @param context Optional context path.
/// @return std::unexpected<Error> instance.
inline std::unexpected<Error> make_error(ErrorCode code, std::string message,
                                         std::string context = "") noexcept {
  return std::unexpected<Error>(
      Error(code, std::move(message), std::move(context)));
}

} // namespace cdd_cpp::utils
