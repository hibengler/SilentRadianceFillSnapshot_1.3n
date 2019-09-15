#/bin/bash
#
# $Revision: 1.3 $
#


echo initial sleep
sleep 5

ip link set dev wlan0 up


#echo turn off wifi control from nm
#echo nmcli off
#sudo nmcli radio wifi off
#^^ above is for 14.04 we are 16.04
# vvvv below is commented out because we have a second wifi 
# card that is managed in some cases
#nmcli  r wifi off

rfkill list
echo unblock wlan with rfkill
sudo rfkill unblock wlan
rfkill list

echo ifconfig
sudo ifconfig wlan0 10.68.0.1/16 up
echo sleeping
sleep 2
ifconfig wlan0
echo start hdcp
service isc-dhcp-server stop
cp dhcpd0.conf /etc/dhcp/dhcpd.conf
service isc-dhcp-server start

#echo unblock other wifi card
#nmcli  r wifi on
#rfkill unblock 3


echo masquerade and forward
echo 1 > /proc/sys/net/ipv4/ip_forward
iptables -t nat -A POSTROUTING -s 10.68.0.0/16 ! -d 10.68.0.0/16  -j MASQUERADE 
echo hostapd
hostapd /etc/hostapd/hostapd0.conf
