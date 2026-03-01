#include "../classes/emit_client.hpp"
#include "../mocks/emit.hpp"
#include "../openapi/emit.hpp"
#include "../openapi/parse.hpp"
#include "../openapi/upgraders/upgrader.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "../google_discovery/parse.hpp"
#include "../utils/cpp_parser.hpp"
#include "../utils/json_writer.hpp"

using namespace cdd_cpp;

void print_help() {
  std::cout << "CDD CLI (Code-Driven Development)\n"
            << "Usage:\n"
            << "  cdd-cpp --help\n"
            << "  cdd-cpp --version\n"
            << "  cdd-cpp from_openapi -i <spec.json>\n"
            << "  cdd-cpp to_openapi -f <path/to/code>\n"
            << "  cdd-cpp to_mocks -i <spec.json>\n"
            << "  cdd-cpp upgrade -i <spec.json> [--in-place] [-o <out.json>] "
               "[--from-google-discovery]\n"
            << "  cdd-cpp to_docs_json [--no-imports] [--no-wrapping] -i "
               "<spec.json>\n"
            << "\n"
            << "Commands:\n"
            << "  sync         : Bi-directional sync of code directory and OpenAPI spec.\n  from_openapi : Parses an OpenAPI spec and emits C++ code.\n"
            << "  to_openapi   : Parses C++ code and emits an OpenAPI spec.\n"
            << "  to_mocks     : Emits a Google Mock interface from an OpenAPI "
               "spec.\n"
            << "  upgrade      : Upgrades an older OpenAPI/Swagger spec to "
               "OpenAPI 3.2.0.\n"
            << "  to_docs_json : Generates JSON documentation for API calls.\n";
}

void print_version() { std::cout << "cdd-cpp version 1.0.0\n"; }

std::string read_file(const std::string &path) {
  std::ifstream fs(path);
  if (!fs)
    throw std::runtime_error("Could not open file: " + path);
  return std::string(std::istreambuf_iterator<char>(fs),
                     std::istreambuf_iterator<char>());
}


namespace cdd_cpp::cli {
    void sync_command(const std::string& code_dir, const std::string& spec_file);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    print_help();
    return 1;
  }

  std::string command = argv[1];

  if (command == "--help" || command == "-h") {
    print_help();
    return 0;
  }
  if (command == "--version" || command == "-v") {
    print_version();
    return 0;
  }

  
  if (command == "sync") {
    std::string folder, spec;
    for (int i = 2; i < argc; ++i) {
      std::string arg = argv[i];
      if ((arg == "-d" || arg == "--dir") && i + 1 < argc) folder = argv[++i];
      if ((arg == "-s" || arg == "--spec") && i + 1 < argc) spec = argv[++i];
    }
    if (folder.empty() || spec.empty()) {
      std::cerr << "Usage: cdd_cpp sync -d <dir> -s <spec.json>\n";
      return 1;
    }
    cdd_cpp::cli::sync_command(folder, spec);
    return 0;
  }

  else if (command == "from_openapi") {
    std::string input;
    for (int i = 2; i < argc; ++i) {
      std::string arg = argv[i];
      if ((arg == "-i" || arg == "--input") && i + 1 < argc) {
        input = argv[++i];
      }
    }
    if (input.empty()) {
      std::cerr << "Missing -i <spec.json>\n";
      return 1;
    }

    std::cout << "Generating client from " << input << "...\n";
    try {
      std::string content = read_file(input);
      std::string upgraded_spec =
          openapi::upgraders::upgrade_to_latest(content);
      auto spec = openapi::parse(upgraded_spec);
      std::string client_code = classes::emit_client(spec);

      std::cout << client_code << "\n";
    } catch (const std::exception &e) {
      std::cerr << "Error: " << e.what() << "\n";
      return 1;
    }

  } else if (command == "to_openapi") {
    std::string folder;
    for (int i = 2; i < argc; ++i) {
      std::string arg = argv[i];
      if ((arg == "-f" || arg == "--folder") && i + 1 < argc) {
        folder = argv[++i];
      }
    }
    if (folder.empty()) {
      std::cerr << "Missing -f <path/to/code>\n";
      return 1;
    }
    std::cout << "Generating spec from " << folder << "...\n";
    try {
      auto spec = utils::parse_cpp_project(folder);
      std::cout << openapi::emit(spec) << "\n";
    } catch (const std::exception &e) {
      std::cerr << "Error: " << e.what() << "\n";
      return 1;
    }
  }

  else if (command == "to_mocks") {
    std::string input;
    for (int i = 2; i < argc; ++i) {
      std::string arg = argv[i];
      if ((arg == "-i" || arg == "--input") && i + 1 < argc) {
        input = argv[++i];
      }
    }
    if (input.empty()) {
      std::cerr << "Missing -i <spec.json>\n";
      return 1;
    }

    try {
      std::string content = read_file(input);
      std::string upgraded_spec =
          openapi::upgraders::upgrade_to_latest(content);
      auto spec = openapi::parse(upgraded_spec);
      std::string mock_code = mocks::emit(spec);
      std::cout << mock_code << "\n";
    } catch (const std::exception &e) {
      std::cerr << "Error: " << e.what() << "\n";
      return 1;
    }
  }

  else if (command == "upgrade") {
    std::string input;
    std::string output;
    bool in_place = false;
    bool from_discovery = false;

    for (int i = 2; i < argc; ++i) {
      std::string arg = argv[i];
      if ((arg == "-i" || arg == "--input") && i + 1 < argc) {
        input = argv[++i];
      } else if ((arg == "-o" || arg == "--output") && i + 1 < argc) {
        output = argv[++i];
      } else if (arg == "--in-place") {
        in_place = true;
      } else if (arg == "--from-google-discovery") {
        from_discovery = true;
      }
    }

    if (input.empty()) {
      std::cerr << "Missing -i <spec.json>\n";
      return 1;
    }

    try {
      std::string content = read_file(input);
      std::string upgraded_spec;

      if (from_discovery) {
        auto specs = google_discovery::parse(content);
        if (specs.empty()) {
          upgraded_spec = openapi::emit(specs[0]);
        } else {
          upgraded_spec = "[\n";
          for (size_t i = 0; i < specs.size(); ++i) {
            upgraded_spec += openapi::emit(specs[i]);
            if (i < specs.size() - 1)
              upgraded_spec += ",\n";
          }
          upgraded_spec += "\n";
        }
      } else {
        upgraded_spec = openapi::upgraders::upgrade_to_latest(content);
      }

      if (in_place) {
        std::ofstream fs(input);
        if (!fs)
          throw std::runtime_error("Could not open file for writing: " + input);
        fs << upgraded_spec << "\n";
        std::cout << "Successfully upgraded " << input << " in-place.\n";
      } else if (!output.empty()) {
        std::ofstream fs(output);
        if (!fs)
          throw std::runtime_error("Could not open file for writing: " +
                                   output);
        fs << upgraded_spec << "\n";
        std::cout << "Successfully wrote upgraded spec to " << output << "\n";
      } else {
        std::cout << upgraded_spec << "\n";
      }
    } catch (const std::exception &e) {
      std::cerr << "Error: " << e.what() << "\n";
      return 1;
    }
  } else if (command == "to_docs_json") {
    bool no_imports = false;
    bool no_wrapping = false;
    std::string input_file;

    for (int i = 2; i < argc; ++i) {
      std::string arg = argv[i];
      if (arg == "--no-imports")
        no_imports = true;
      else if (arg == "--no-wrapping")
        no_wrapping = true;
      else if ((arg == "-i" || arg == "--input") && i + 1 < argc)
        input_file = argv[++i];
    }

    if (input_file.empty()) {
      std::cerr << "Missing -i <spec.json>\n";
      return 1;
    }

    try {
      std::string content = read_file(input_file);
      auto spec = openapi::parse(content);

      utils::JsonWriter jw;
      jw.start_array();
      jw.start_object();
      jw.key_value("language", "cpp");
      jw.key("operations");
      jw.start_array();

      if (spec.paths.has_value() && !spec.paths->empty()) {
        for (const auto &[path, pi] : spec.paths.value()) {
          auto add_op = [&](const std::string &method,
                            const std::optional<openapi::Operation> &op) {
            if (!op.has_value())
              return;
            jw.start_object();
            jw.key_value("method", method);
            jw.key_value("path", path);
            if (op->operationId.has_value()) {
              jw.key_value("operationId", op->operationId.value());
            }
            jw.key("code");
            jw.start_object();

            if (!no_imports) {
              jw.key_value("imports",
                           "#include <iostream>\n#include <cdd_client.hpp>");
            }
            if (!no_wrapping) {
              jw.key_value("wrapper_start",
                           "int main() {\n    cdd_cpp::Client client;\n");
            }

            std::string op_name = op->operationId.value_or("request");
            jw.key_value("snippet",
                         "    auto res = client." + op_name +
                             "();\n    std::cout << res << \"\\n\";");

            if (!no_wrapping) {
              jw.key_value("wrapper_end", "    return 0;\n}");
            }
            jw.end_object();
            jw.end_object();
          };

          add_op("GET", pi.get);
          add_op("POST", pi.post);
          add_op("PUT", pi.put);
          add_op("DELETE", pi.delete_op);
          add_op("PATCH", pi.patch);
        }
      }

      jw.end_array();
      jw.end_object();
      jw.end_array();

      std::cout << jw.str() << "\n";

    } catch (const std::exception &e) {
      std::cerr << "Error: " << e.what() << "\n";
      return 1;
    }
  } else {
    std::cerr << "Unknown command: " << command << "\n";
    print_help();
    return 1;
  }

  return 0;
}
