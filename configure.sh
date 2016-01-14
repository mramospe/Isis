#=================================================#
# Configures the installation of the Isis project #
#-------------------------------------------------#
# AUTHOR: Miguel Ramos Pernas		          #
# e-mail: miguel.ramos.pernas@cern.ch	          #
#=================================================#

# Loads the Isis environment
source bin/loadenv.sh
echo " Loaded Isis environment"

# If the library directory does not exist it is created
if [ ! -d "$ISIS/lib" ]; then
    mkdir $ISIS/lib
    echo " Created library folder at: $ISIS/lib"
fi

echo " Type < make > to install the Isis project"