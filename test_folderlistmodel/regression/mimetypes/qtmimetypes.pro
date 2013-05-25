include(doc/doc.pri)

TEMPLATE = subdirs

module_include.subdir = include
module_include.target = module_include

module_src.subdir = src
module_src.target = module_src
module_src.depends = module_include

module_tests.file = tests/tests.pro
module_tests.target = module_tests
module_tests.depends = module_src

module_examples.file = examples/examples.pro
module_examples.target = module_examples
module_examples.depends = module_src

exists(include/include.pro): SUBDIRS += module_include
exists(src/src.pro): SUBDIRS += module_src
exists(tests/tests.pro): SUBDIRS += module_tests
exists(examples/examples.pro): SUBDIRS += module_examples
