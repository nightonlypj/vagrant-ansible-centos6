# CentOS6 Vagrantfile＋Ansible playbook提供

Amazon Linux(EC2/Lightsail)対応  
※Amazon Linux 2(EC2)を使用する場合は、[CentOS7 Vagrantfile＋Ansible playbook提供](https://dev.azure.com/nightonly/vagrant-ansible-origin/_git/vagrant-ansible-centos7)を使用してください。

## 前提条件

下記がインストールされている事

- Vagrant ( https://www.vagrantup.com/downloads.html )  
- VirtualBox ( https://www.virtualbox.org/wiki/Downloads )

※VMwareでの動作は未確認です。  
※古いバージョンでは動かない場合があります。

作成時に使用したバージョン

- Vagrant 1.9.3 (Windows 64-bit)  
- VirtualBox 5.1.22 (Windows)

## ファイル構成

```
ansible
    hosts           ：環境設定
        development,def ：開発環境のサンプル
        test,def        ：テスト環境のサンプル
        staging,def     ：ステージング環境のサンプル
        production,def  ：本番環境のサンプル
    roles           ：設定ルール
        common          ：共通の設定等
        ntpd            ：ntpdの設定
        postfix         ：Postfix(localhost only)の設定
        sshd            ；sshdの設定
        mysql           ：MySQLの設定
        mysql56         ：MySQL 5.6の設定
        mysql57         ：MySQL 5.7の設定
        postgresql      ：PostgreSQLの設定
        postgresql96    ：PostgreSQL 9.6の設定
        redis           ：Redisの設定
        mongodb         ：MongoDBの設定
        imagick         ：ImageMagickの設定
        java18          ：Java 1.8の設定
        tomcat          ：Tomcatの設定
        letsencrypt     ：Let's Encryptの設定　※自動更新対応
        httpd           ：Apacheの設定　※Load Balancer対応
        php-httpd       ：PHP for Apacheの設定
        nginx           ：Nginxの設定　※Load Balancer対応
        php-nginx       ：PHP for Nginxの設定(PHP-FPM)
        php71-nginx     ：PHP 7.1 for Nginxの設定(PHP-FPM)
        php72-nginx     ：PHP 7.2 for Nginxの設定(PHP-FPM)
        squid           ：Squidの設定
    ansible.cfg     ：Ansibleの設定ファイル
    playbook.yml    ：どの設定ルールを使うかを制御する設定　※使用しないルールはコメントアウトしてください
README.md       ：説明や使い方（このファイル）
RELEASES.md     ：リリースノート
Vagrantfile,def ：Vagrantfileのサンプル（開発環境用）
```

## 環境構築手順

Mac・Linuxターミナル(Windowsはエクスプローラー・エディタ等で操作)

### Vagrantfile

```
$ cp Vagrantfile,def Vagrantfile
$ vi Vagrantfile
```

SSH接続ポート変更(管理用)：使用してないポートを指定  
>  config.vm.network "forwarded_port", guest: 22, host: `2206`, host_ip: "127.0.0.1", id: "ssh"

IPアドレス変更(HTTP/S等の接続用)：使用してないIPを指定  
>  config.vm.network "public_network", ip: "`192.168.12.206`"

※複数のNICに繋がっている場合はブリッジアダプターを設定しておくと便利  
>  config.vm.network "public_network", ip: "`192.168.12.206`"`, bridge: "en0: Wi-Fi (AirPort)"`

ホスト名変更  
>   config.vm.hostname = "`dev-centos6.local`"

SSHユーザー名・パスワード変更、rootパスワード変更  
>    useradd -g wheel `admin`  
>    echo "`admin`:`abc123`" | chpasswd  
>    echo "root:`xyz789`" | chpasswd

### ansible/hosts/development

```
$ cd ansible/hosts
$ cp development,def development
$ vi development
```

メール転送先：自分のメールアドレスを指定  
> aliases_notice=`admin@mydomain`  
> aliases_warning=`admin@mydomain`  
> aliases_critical=`admin@mydomain`

※その他、設定は必要に応じて変更してください。  
※test/staging/productionも必要に応じて設定してください。追加も可能です。

DNSで設定したホスト名を指定  
> httpd_front_servername=`test.mydomain`  
※Let's Encryptを使用する場合は、`ansible/roles/letsencrypt/templates/etc/letsencrypt/live/test.mydomain`をホスト名にコピーまたはリネームしてください。

## development使用方法(例)

[CentOS6 Vagrant Box提供(VirtualBox向け)](https://dev.azure.com/nightonly/vagrant-ansible-origin/_git/vagrant-box-centos6)から最新のBoxをダウンロードしてください。

Mac・Linuxターミナル/Windowsコマンドプロンプト  
※下記の`~/Downloads/CentOS6.9.box`はダウンロードしたBoxのパスを指定してください。  
```
$ vagrant box add CentOS6 ~/Downloads/CentOS6.9.box
$ vagrant plugin install vagrant-vbguest
$ vagrant up
$ vagrant vbguest
$ vagrant reload
```

※Mac・Linuxの場合（Windowsは設定すれば接続可）
```
$ vagrant ssh
$ sudo -s
```

※Windowsの場合（Mac・Linuxでも可）　※ユーザー名・パスワード・ポートは初期設定の値に変更して実行してください。
```
$ ssh admin@127.0.0.1 -p 2207
: abc123
$ su -
: xyz789
（または $ sudo -s）
```

```
# cd /vagrant/ansible
# ansible-playbook playbook.yml -i hosts/development -l development
```
※特定の設定ルール(roles)のみ実行する場合はansible-playbookコマンドでtagsを指定する。例：`-t httpd,php-httpd`

---

## サーバー側使用方法(例)

※以降は、サーバー構築時のみ実行

### ansibleユーザー作成・鍵作成

Tips: VMからではなく、Macから直接実行する場合は、サーバー側で実行（2台目の場合は除く）

VMに接続して実行（初回のみ）
```
# useradd -g wheel -u 400 ansible
# passwd ansible
: ********(ansibleのPW) ※ここで決めて、以降はそれを使用
: ********(ansibleのPW)
```
```
# su - ansible
$ ssh-keygen -t rsa -b 4096
Enter file in which to save the key (/home/ansible/.ssh/id_rsa): (空のままEnter)
Enter passphrase (empty for no passphrase): (空のままEnter)
Enter same passphrase again: (空のままEnter)
$ cat ~/.ssh/id_rsa.pub
※(*1)表示内容をメモ
$ exit
```

Tips: VMからではなく、Macから直接実行する場合は、下記も実行

各サーバーで実行（初回のみ）
```
# su - ansible
$ cd .ssh
$ ln -s id_rsa.pub authorized_keys
$ exit
```

### ansibleユーザー作成・鍵設置（各サーバー）

Tips: VMからではなく、Macから直接実行する場合は、サーバーで作成した鍵をMacに設置（2台目の場合は実行）

VMに接続して実行（初回のみ）
```
# useradd -g wheel -u 400 ansible
# passwd ansible
New password: ********(ansibleのPW)
Retype new password: ********
```
```
# su - ansible
$ mkdir .ssh
$ chmod 700 .ssh
$ vi .ssh/authorized_keys
※(*1)の表示内容を貼り付け
$ chmod 600 .ssh/authorized_keys
$ exit
```

### sudo権限確認・追加（各サーバー）

※ansibleユーザー（wheelグループ）でsudo出来るようにします。

各サーバーで実行（初回のみ）
```
# grep -e "^%wheel\s*ALL=(ALL)\s*ALL$" /etc/sudoers > /dev/null
# echo $?
```
※上記で1と表示されたら下記を実行
```
# cp -a /etc/sudoers /etc/sudoers,`date +"%Y%m%d%H%M%S"`
# echo -e "%wheel\tALL=(ALL)\tALL" >> /etc/sudoers
```

### ssh/config設定（Macから直接実行する場合）

Macで実行
```
$ vi ~/.ssh/config
---- ここから ----
Host railsapp-test_ansible
  Hostname 【ドメイン名 or IP】
  User ansible
  IdentityFile 【サーバーで作成した鍵を設置したパス】
---- ここまで ----
```

```
$ vi ansible/hosts/test
---- ここから ----
test.mydomain
　↓
railsapp-test_ansible
---- ここまで ----
```

### Playbook実行

Tips: VMからではなく、Macから直接実行する場合は、suは不要でログディレクトリを作成してからansible-playbookコマンドを実行
> cd ansible  
> mkdir ../log

VMに接続して実行  
※下記はtestの例です。他の環境を使用する場合は、hosts/`test`を変更して実行してください。
```
# su - ansible
$ cd /vagrant/ansible
$ ansible-playbook playbook.yml -i hosts/test -l all --ask-become-pass
BECOME password: ********(ansibleのPW)
Are you sure you want to continue connecting (yes/no)? yes
$ exit
```
※特定のサーバーのみ実行する場合は`all`を`web-servers`等に変えてください。

### Let's Encrypt初期設定（各サーバー）[使用時のみ]

#### シングルドメイン証明書の場合

※インターネットから http://[対象ドメイン]/.well-known/acme-challenge/ にアクセス出来る必要があります。（存在確認の為）

各サーバーで実行（初回のみ）  
※下記のドメイン名・メールアドレスを変更して実行してください。  
※複数のドメインを使用する場合は、certbotの行を複数回実行してください。
```
# mv /etc/letsencrypt /etc/letsencrypt,`date +"%Y%m%d%H%M%S"`

Apacheの場合
# certbot certonly --webroot -w /var/www/html -d test.mydomain -d www.test.mydomain --email admin@mydomain --agree-tos --no-eff-email
Nginxの場合
# certbot certonly --webroot -w /usr/share/nginx/html -d test.mydomain -d www.test.mydomain --email admin@mydomain --agree-tos --no-eff-email

IMPORTANT NOTES:
 - Congratulations! Your certificate and chain have been saved at:

Apacheの場合
# apachectl configtest
Syntax OK
# apachectl graceful

Nginxの場合
# nginx -t -c /etc/nginx/nginx.conf
nginx: the configuration file /etc/nginx/nginx.conf syntax is ok
nginx: configuration file /etc/nginx/nginx.conf test is successful
# service nginx restart
```

※更新(certbot renew)はバッチ(/etc/cron.weekly/renew_letsencrypt.cron)で定期的に実行されます。

#### マルチドメイン証明書の場合

ConoHa DNSの例です。事前に下記を設定して、ansible-playbookコマンド(-t letsencrypt)を実行してください。  
手動でDNS設定/更新を行う場合は、下記設定および--manual-auth-hookと--manual-cleanup-hookは不要です。

ansible/hosts/対象環境
```
# ConoHa DNS  https://manage.conoha.jp/API/
conoha_enable_letsencrypt=0
conoha_identity_service=https://identity.tyo2.conoha.io/v2.0
conoha_dns_service=https://dns-service.tyo2.conoha.io
conoha_api_username=
conoha_api_password=
conoha_tenant_id=
```

各サーバーで実行（初回のみ）  
※下記のドメイン名・メールアドレスを変更して実行してください。  
※複数のドメインを使用する場合は、certbotの行を複数回実行してください。
```
# mv /etc/letsencrypt /etc/letsencrypt,`date +"%Y%m%d%H%M%S"`
# certbot certonly --manual --preferred-challenges dns -d test.mydomain -d *.test.mydomain --email admin@mydomain --agree-tos --no-eff-email --manual-auth-hook /usr/local/bin/conoha/create_dns_record.sh --manual-cleanup-hook /usr/local/bin/conoha/delete_dns_record.sh

IMPORTANT NOTES:
 - Congratulations! Your certificate and chain have been saved at:

Apacheの場合
# apachectl configtest
Syntax OK
# apachectl graceful

Nginxの場合
# nginx -t -c /etc/nginx/nginx.conf
nginx: the configuration file /etc/nginx/nginx.conf syntax is ok
nginx: configuration file /etc/nginx/nginx.conf test is successful
# service nginx restart
```

※更新(certbot renew)はバッチ(/etc/cron.weekly/renew_letsencrypt.cron)で定期的に実行されます。
