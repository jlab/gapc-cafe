
set -e
set -u


ID=`id -u gapp`

iptables -N gapp_chain
iptables -A gapp_chain -m owner --uid-owner $ID -p tcp --dport 1:1024 -j REJECT
iptables -A gapp_chain -m owner --uid-owner $ID -p tcp -d 127.0.0.1 -j ACCEPT
iptables -A gapp_chain -m owner --uid-owner $ID -j REJECT
iptables -A OUTPUT -j gapp_chain


