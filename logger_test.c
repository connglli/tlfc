#include "logger.h"

int main() {
  log("This is log: %d, %c, %lf, %s\n", 1, 'a', 0.12, "test cases");
  logln("This is logln: %d, %c, %lf, %s", 1, 'a', 0.12, "test cases");
  logc("This is logc: %d, %c, %lf, %s", 1, 'a', 0.12, "test cases");
  logi("This is logi: %d, %c, %lf, %s", 1, 'a', 0.12, "test cases");
  logw("This is logw: %d, %c, %lf, %s", 1, 'a', 0.12, "test cases");
  logd("This is logd: %d, %c, %lf, %s", 1, 'a', 0.12, "test cases");
  loge("This is loge: %d, %c, %lf, %s", 1, 'a', 0.12, "test cases");
}