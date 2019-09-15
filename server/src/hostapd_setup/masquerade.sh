#/bin/bash
#
# $Revision: 1.2 $
#


sudo iptables -A FORWARD -o eth0 -i wlan0 \
 -s 10.70.0.0/26 -m conntrack --ctstate NEW -j ACCEPT
sudo iptables -A FORWARD -m conntrack --ctstate ESTABLISHED,RELATED -j ACCEPT
sudo iptables -t nat -F POSTROUTING
sudo iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE
