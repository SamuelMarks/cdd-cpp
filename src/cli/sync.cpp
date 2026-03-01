#include "../openapi/parse.hpp"
#include "../openapi/emit.hpp"
#include "../utils/cpp_parser.hpp"
#include "../classes/emit_client.hpp"
#include "../mocks/emit.hpp"
#include <fstream>
#include <iostream>

namespace cdd_cpp::cli {

void sync_command(const std::string& code_dir, const std::string& spec_file) {
    std::cout << "Syncing code in " << code_dir << " with spec " << spec_file << "...\n";
    // Parse C++ to get updated routes/mocks
    auto cpp_spec = utils::parse_cpp_project(code_dir);
    
    // Parse existing OpenAPI spec to merge
    std::ifstream fs(spec_file);
    openapi::OpenAPI merged_spec = cpp_spec;
    if (fs) {
        std::string content((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());
        try {
            auto file_spec = openapi::parse(content);
            merged_spec.info = file_spec.info; // Keep original info
        } catch (...) {
            // ignore parse errors if it's broken, will overwrite
        }
    }
    
    // Update OpenAPI Spec file
    std::ofstream out_fs(spec_file);
    out_fs << openapi::emit(merged_spec) << "\n";
    std::cout << "Updated OpenAPI spec at " << spec_file << "\n";
    
    // Regenerate Mocks
    std::string mock_code = mocks::emit(merged_spec);
    std::ofstream mock_fs(code_dir + "/mocks_generated.hpp");
    mock_fs << mock_code << "\n";
    
    // Regenerate Client
    std::string client_code = classes::emit_client(merged_spec);
    std::ofstream client_fs(code_dir + "/client_generated.hpp");
    client_fs << client_code << "\n";
    
    std::cout << "Updated mocks and clients in " << code_dir << "\n";
}

}
