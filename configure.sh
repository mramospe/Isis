#=================================================#
# Configures the installation of the Isis project #
#-------------------------------------------------#
# AUTHOR: Miguel Ramos Pernas		          #
# e-mail: miguel.ramos.pernas@cern.ch	          #
#=================================================#

# Gets the path to this script
currpath=$(readlink -f $(dirname ${BASH_SOURCE[0]}))

buildpath=$currpath/build

# If the library directory does not exist it is created
if [ ! -d "$buildpath" ]; then
    mkdir $buildpath
    echo "The Isis package will be built in $buildpath"
fi

echo "Accessing path $buildpath"
cd $buildpath

echo "Redefining variables CC and CXX:"
export CC=`which gcc`
export CXX=`which g++`
echo "-- CC set to:  $CC"
echo "-- CXX set to: $CXX"

echo "Invoking cmake"
cmake $currpath $@
echo "Type < make > to install the Isis package"