# Publishing Generated Outputs
When you use `cdd-cpp` to generate a client or server, you might want to automate publishing those artifacts.
## Keeping Clients Up To Date
To keep a generated C++ client up to date with a changing OpenAPI server specification, you can create a cronjob using GitHub Actions.
1. Create a `.github/workflows/update-client.yml` workflow.
2. Schedule it to run daily or trigger it via webhooks.
3. Download the latest `openapi.json` from your server.
4. Run `cdd-cpp from_openapi -i openapi.json > client.cpp`.
5. Create a Pull Request with the changes using `peter-evans/create-pull-request`.
