#!/bin/bash
set -e -x

cd /github/workspace/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/src/dotnet/shared/Microsoft.NETCore.App/6.0.0-rc.1.21451.13
echo $LD_LIBRARY_PATH
rm -rf _skbuild/
/opt/python/cp310-cp310/bin/pip install --upgrade --no-cache-dir "pip<21.3"
/opt/python/cp310-cp310/bin/pip install auditwheel
/opt/python/cp310-cp310/bin/pip install .
/opt/python/cp310-cp310/bin/pip wheel . -w ./dist --no-deps
find . -type f -iname "*-linux*.whl" -execdir sh -c "auditwheel repair '{}' -w ./ --plat 'manylinux2014_`uname -m`' || { echo 'Repairing wheels failed.'; auditwheel show '{}'; exit 1; }" \;
echo "Succesfully built wheels:"
find . -type f -iname "*-manylinux*.whl"