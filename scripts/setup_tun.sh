addr=10.0.0.1/8

sudo ip link set tun0 up
sudo ip addr add $addr dev tun0