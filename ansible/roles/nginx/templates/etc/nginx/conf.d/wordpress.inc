### START ###
index index.php;
location /license.txt {
    deny all;
}
location /readme.html {
    deny all;
}
### END ###

## Reference: https://wpdocs.osdn.jp/Nginx
# WordPress multisite subdirectory rules.
# Designed to be included in any server {} block.

# This order might seem weird - this is attempted to match last if rules below fail.
# http://wiki.nginx.org/HttpCoreModule
location / {
    try_files $uri $uri/ /index.php?$args;
}

# Add trailing slash to */wp-admin requests.
rewrite /wp-admin$ $scheme://$host$uri/ permanent;

# Directives to send expires headers and turn off 404 error logging.
location ~* \.(js|css|png|jpg|jpeg|gif|ico)$ {
    expires 24h;
### START ###
#    log_not_found off;
### END ###
}

# Pass uploaded files to wp-includes/ms-files.php.
rewrite /files/$ /index.php last;

# For multisite:  Use a caching plugin that creates symlinks to the correct subdirectory structure to get some performance gains.
set $cachetest "$document_root/wp-content/cache/ms-filemap/${host}${uri}";
if ($uri ~ /$) {
    set $cachetest "";
}
if (-f $cachetest) {
    # Rewrites the URI and stops rewrite processing so it doesn't start over and attempt to pass it to the next rule.
    rewrite ^ /wp-content/cache/ms-filemap/${host}${uri} break;
}

if ($uri !~ wp-content/plugins) {
    rewrite /files/(.+)$ /wp-includes/ms-files.php?file=$1 last;
}

# Uncomment one of the lines below for the appropriate caching plugin (if used).
#include global/wordpress-ms-subdir-wp-super-cache.conf;
#include global/wordpress-ms-subdir-w3-total-cache.conf;

# Rewrite multisite '.../wp-.*' and '.../*.php'.
if (!-e $request_filename) {
    rewrite ^/[_0-9a-zA-Z-]+(/wp-.*) $1 last;
    rewrite ^/[_0-9a-zA-Z-]+.*(/wp-admin/.*\.php)$ $1 last;
    rewrite ^/[_0-9a-zA-Z-]+(/.*\.php)$ $1 last;
}

# Pass all .php files onto a php-fpm/php-fcgi server.
{% if wordpress_admin_access_control != 0 %}
### START ###
# Admin access control
location ~* /wp-login\.php|/wp-admin/((?!admin-ajax\.php).)*$ {
{% if wordpress_admin_access_control == 2 %}
    satisfy any;
{% endif %}
    include /etc/nginx/conf.d/allow-ip.inc;
    deny all;
{% if wordpress_admin_access_control == 2 %}
    auth_basic "Please enter your ID and Password.";
    auth_basic_user_file /etc/nginx/conf.d/.htpasswd;
{% endif %}

    location ~ \.php$ {
        try_files $uri =404;

        fastcgi_split_path_info ^(.+\.php)(/.+)$;
        include fastcgi_params;

        fastcgi_index index.php;
        fastcgi_param SCRIPT_FILENAME $document_root$fastcgi_script_name;
        fastcgi_pass php-fpm;
    }
}
### END ###
{% endif %}
location ~ \.php$ {
    # Zero-day exploit defense.
    # http://forum.nginx.org/read.php?2,88845,page=3
    # Won't work properly (404 error) if the file is not stored on this server, which is entirely possible with php-fpm/php-fcgi.
    # Comment the 'try_files' line out if you set up php-fpm/php-fcgi on another machine.  And then cross your fingers that you won't get hacked.
    try_files $uri =404;

    fastcgi_split_path_info ^(.+\.php)(/.+)$;
    include fastcgi_params;

    fastcgi_index index.php;
    fastcgi_param SCRIPT_FILENAME $document_root$fastcgi_script_name;
#    fastcgi_intercept_errors on;
### START ###
#    fastcgi_pass php;
    fastcgi_pass php-fpm;
### END ###
}
