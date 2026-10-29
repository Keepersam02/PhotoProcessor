#include <cstddef>
#include <cstdio>
#include <cstring>
#include <time.h>

const size_t MAX_ST_LEN = 1000;
const size_t MAX_CD_LEN = 1000;
const size_t MAX_PATH_LEN = 500;
const size_t MAX_ERR_MESSAGE_LEN = 250;

const size_t MAX_COMB_ERR_MESS_LEN = 550;
const size_t MAX_ADD_INFO_MESS_LEN = 2000;
const size_t MAX_SEV_MESS_LEN = 3000;

enum class severity { INFO, WARNING, ERROR, SEVERE };

enum class category { SYSTEM, FILESYSTEM, PROCESSING };

enum class phase { INIT, READ, WRITE, PRE_PROC, PROC };

class error {
public:
  virtual void combined_err(char err_message[MAX_COMB_ERR_MESS_LEN]) const = 0;

  virtual void add_info_mess(char add_info_[MAX_ADD_INFO_MESS_LEN]) const = 0;

  virtual void severe_info(char sev_mess[MAX_SEV_MESS_LEN]) const = 0;

  virtual ~error() = default;
};

class init_error : public error {
public:
  severity sev;
  category cat;

  char error_mess[250];
  char cust_err_mess[250];
  char add_info[500];

  void combined_err(char err_message[MAX_COMB_ERR_MESS_LEN]) const override {
    size_t err_mess_len = strlen(error_mess);
    size_t cust_err_mess_len = strlen(cust_err_mess);

    char intro[25] = "Standard error message: ";
    char trans[23] = "Custom error message: ";

    void *next = memcpy(err_message, intro, 24);
    void *n2 = memcpy(next, error_mess, err_mess_len - 1);
    void *n3 = memcpy(n2, trans, 22);
    memcpy(n3, cust_err_mess, cust_err_mess_len);
  }

  void add_info_mess(char add_info_[MAX_ADD_INFO_MESS_LEN]) const override {
    memcpy(add_info_, add_info, strlen(add_info_));
  }

  void severe_info(char sev_mess[MAX_SEV_MESS_LEN]) const override {
    char sev_cat_info[100];
    sprintf(sev_cat_info, "Severity: %2d\nCategory: %2d\n",
            static_cast<int>(sev), static_cast<int>(cat));

    void *next = memcpy(sev_mess, sev_cat_info, strlen(sev_cat_info));
  }
};
