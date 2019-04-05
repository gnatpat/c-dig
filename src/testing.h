struct TestResult {
  bool success;
  const char* file;
  const char* method;
  int line;
};

#define CHECK(value) if(!value) {return { value, __FILE__+11, __func__, __LINE__ };}
#define SUCCESS return { true, __FILE__+11, __func__, __LINE__ };
