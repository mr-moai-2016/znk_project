#!/system/bin/sh

chmod 755 ./moai
chmod 755 ./cgis/easter/*.cgi
chmod 755 ./cgis/custom_boy/*.cgi
chmod 755 ./cgis/cgi_samples/*.cgi
chmod 755 ./cgis/proxy_finder/*.cgi
env LD_LIBRARY_PATH=$LD_LIBRARY_PATH:. ./moai

