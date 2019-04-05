import glob
import subprocess

from jinja2 import Template

def main():
    test_files = glob.glob('src/tests/*_test.cpp')
    tests = []
    for test_file in test_files:
        with open(test_file) as f:
            for line in f:
                if line.startswith("TestResult test"):
                    tests.append(line.split(" ")[1].split("(")[0])


    test_entry_contents = Template(open('py/templates/test.cpp').read()).render(
            test_files=test_files,
            tests=tests)
    open('src/generated/test_entry.cpp', 'w').write(test_entry_contents)
    subprocess.run(['clang-format', '-i', 'src/generated/test_entry.cpp'])

if __name__ == "__main__":
    main()
