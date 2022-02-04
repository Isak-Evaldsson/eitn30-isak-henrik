# Config git, ensures that our commits will be done in our name and not 
# some random person who previolus used the same pi
git config user.name "Isak Evaldsson"
git config user.email "isak.evaldsson@outlook.com"

# Make all scripts executable
chmod a+x scripts/*.sh

# Installs dependinces
./scripts/install_rf24.sh