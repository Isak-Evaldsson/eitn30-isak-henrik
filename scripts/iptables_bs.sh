#!/bin/bash
# make sures our iptables are flushed before applying the new rules
sudo iptables -F
sudo iptables -t nat -F

# forward tun0 -> eth0 
sudo iptables -A FORWARD -i tun0 -o eth0 -j ACCEPT

# forward eth0 -> tun0
sudo iptables -A FORWARD -i eth0 -o tun0 -m state --state RELATED,ESTABLISHED -j ACCEPT

# Setup masquerade
sudo iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE