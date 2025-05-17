cd submodules/libgscsp

cpu1_install_path=$(pwd)/build/cpu1
obc_install_path=$(pwd)/build/obc

python3 ./tools/buildtools/gsbuildtools_bootstrap.py

python3 waf distclean

# cpu1 build
CFLAGS='-fPIC' python3 waf configure --prefix=$cpu1_install_path
python3 waf build install

# obc build (revise later)
# CFLAGS='-fPIC' python3 waf configure --prefix=$cobc_install_path
# python3 waf build install