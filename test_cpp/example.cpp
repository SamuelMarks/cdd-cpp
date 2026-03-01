/**
 * @description A user in the system
 */
struct User {
  int id;           // @description The unique identifier
  std::string name; // @description The user's name
};

/**
 * @route GET /users
 * @summary Get all users
 */
std::vector<User> get_users();

/**
 * @route POST /users
 * @summary Create a new user
 */
void create_user(User u);
