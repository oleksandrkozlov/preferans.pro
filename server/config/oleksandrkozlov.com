# SPDX-License-Identifier: AGPL-3.0-only
#
# Copyright (c) 2025 Oleksandr Kozlov

# https://certbot.eff.org/instructions?ws=nginx&os=pip
# sudo rm /etc/nginx/sites-enabled/default
# sudo touch /etc/nginx/sites-available/yoursite.com
# sudo ln -s /etc/nginx/sites-available/yoursite.com /etc/nginx/sites-enabled/
# sudo nginx -t
# sudo chmod o+x /home/olkozlo/Work/workspace/preferans
# sudo nginx

server {
    listen 80;
    listen [::]:80;
    server_name oleksandrkozlov.com www.oleksandrkozlov.com;
    return 301 https://oleksandrkozlov.com$request_uri;
}

server {
    listen 443 ssl;
    listen [::]:443 ssl;
    server_name oleksandrkozlov.com;

    ssl_certificate /etc/letsencrypt/live/oleksandrkozlov.com/fullchain.pem;
    ssl_certificate_key /etc/letsencrypt/live/oleksandrkozlov.com/privkey.pem;
    include /etc/letsencrypt/options-ssl-nginx.conf;
    ssl_dhparam /etc/letsencrypt/ssl-dhparams.pem;

    location = / {
        return 301 /preferans;
    }

    location = /prefbuff {
        return 301 /prefbuff/;
    }

    location /preferans {
        alias /home/olkozlo/Work/workspace/preferans/build-client/bin/;
        index index.html;
        try_files $uri $uri/ /preferans/index.html;
    }

    location /prefbuff/ {
        proxy_pass http://127.0.0.1:8081/;  # trailing slash is important
        proxy_http_version 1.1;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
    }

    location /assets/cards/ {
        proxy_pass http://127.0.0.1:8081;
        proxy_http_version 1.1;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
    }

    location = /favicon.ico {
        alias /home/olkozlo/Work/workspace/preferans/build-client/bin/favicon.ico;
        log_not_found off;
        access_log off;
    }
}

server {
    listen 443 ssl;
    listen [::]:443 ssl;
    server_name www.oleksandrkozlov.com;
    ssl_certificate /etc/letsencrypt/live/oleksandrkozlov.com/fullchain.pem;
    ssl_certificate_key /etc/letsencrypt/live/oleksandrkozlov.com/privkey.pem;
    include /etc/letsencrypt/options-ssl-nginx.conf;
    ssl_dhparam /etc/letsencrypt/ssl-dhparams.pem;
    return 301 https://oleksandrkozlov.com$request_uri;
}
