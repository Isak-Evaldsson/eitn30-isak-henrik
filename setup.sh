# Config git, ensures that our commits will be done in our name and not 
# some random person who previolus used the same pi
git config user.name "Isak Evaldsson"
git config user.email "isak.evaldsson@outlook.com"

# Installs dependinces
# RF24
chmod a+x scripts/install_rf24.sh
./scripts/install_rf24.sh