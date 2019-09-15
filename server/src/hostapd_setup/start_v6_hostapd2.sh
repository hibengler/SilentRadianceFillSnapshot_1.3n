#/bin/bash
#
# $Revision: 1.2 $
#


echo initial sleep
sleep 5

#echo turn off wifi control from nm
#echo nmcli off
#sudo nmcli radio wifi off
#^^ above is for 14.04 we are 16.04
# vvvv below is commented out because we have a second wifi 
# card that is managed in some cases
#nmcli  r wifi off

rfkill list
echo unblock wlan with rfkill
# vvvv v6 extra
echo rfkill unblock
# ^^^ v6 extra
sudo rfkill unblock wlan
rfkill list


echo ifconfig
sudo ifconfig wlan0 10.70.0.1/16 up
# vvvv v6 extra
ifconfig wlan0 inet6 add fe80::d333:a111:2344:1111
# ^^^ v6 extra
echo sleeping
sleep 2
ifconfig wlan0

echo start hdcp
#sudo service isc-dhcp-server restart
service isc-dhcp-server stop
cp dhcpdv6_2.conf /etc/dhcp/dhcpd.conf
service isc-dhcp-server start

echo masquerade and forward
echo 1 > /proc/sys/net/ipv4/ip_forward
iptables -t nat -A POSTROUTING -s 10.70.0.0/16 ! -d 10.70.0.0/16  -j MASQUERADE 
echo hostapd
hostapd /etc/hostapd/hostapd2.conf
