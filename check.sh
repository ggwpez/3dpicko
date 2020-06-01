
#/bin/bash

git checkout -- .
[ -z "$(find Main/src -regex '.*\.\(cc\|h\|hpp\|cpp\)' -exec clang-format -style=file {} \;)" ] || (echo '--- C++ Format error' && false)
[ -z '$(find . -name "*.html" -type f -exec xmllint --noout --html --format "{}" \;)' ] || (echo '--- HTML Format error' && false)
