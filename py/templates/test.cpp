#include "../testing.h"
#include "../includes.cpp"

{% for test_file in test_files %}
#include "../{{test_file}}"
{% endfor %}

int main(void) {
  bool failure = false;
  TestResult result;

  {% for test in tests %}
  result = {{test}}();
  if (!result.success) {
    failure = true;
    printf("%s:%d - %s failed.\n", result.file, result.line, result.method);
  }
  {% endfor %}

  if (failure) {
    printf(" --- There were failures. :( --- \n");
  } else {
    printf(" --- All good! :) --- \n");
  }
}
