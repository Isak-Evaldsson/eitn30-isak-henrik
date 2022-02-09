# Config git, ensures that our commits will be done in our name and not 
# some random person who previolus used the same pi
if [ $random % 2 == 1 ] 
then
    git config user.name "Isak Evaldsson"
    git config user.email "isak.evaldsson@outlook.com"
else    
    git config user.name "Henrik Paulcén"
    git config user.email "henrik.in.sweden@gmail.com"
fi

# Hopefully less merge issues
git config pull.rebase true

# Make all scripts executable
chmod a+x scripts/*.sh

# Installs dependinces
./scripts/install_rf24.sh