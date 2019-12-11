{% if front_access_control != 0 %}
### START ###
Allow from 127.0.0.1
Allow from ::1
Allow from 10.0.0.0/8
Allow from 172.16.0.0/12
Allow from 192.168.0.0/16
Allow from fc00::/7
# *** Tips: Please add here
### END ###
{% endif %}
