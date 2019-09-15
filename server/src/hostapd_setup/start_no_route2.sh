
# not fully tested and ready - was working on it
# basically need to fool the android and ios servers to think that they are connected when we are in nowhere land
sudo nmcli nm wifi off
sudo rfkill unblock wlan

sudo ifconfig wlan0 10.70.0.1/16 up
sleep 1
#sudo service isc-dhcp-server restart
service isc-dhcp-server start
echo 0 > /proc/sys/net/ipv4/ip_forward
iptables -D POSTROUTING 1
hostapd /etc/hostapd/hostapd_no_route2.conf
