{% if front_access_control != 0 or admin_access_control != 0 %}
### START ###
allow 127.0.0.1;
allow ::1;
allow 10.0.0.0/8;
allow 172.16.0.0/12;
allow 192.168.0.0/16;
allow fc00::/7;
# *** Tips: Please add here
### END ###
{% endif %}
