# CentOS6 Vagrantfile＋Ansible playbook提供（WordPress対応）

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
（未使用）mysql56         ：MySQL 5.6の設定
（未使用）mysql57         ：MySQL 5.7の設定
（未使用）postgresql      ：PostgreSQLの設定
（未使用）postgresql96    ：PostgreSQL 9.6の設定
（未使用）redis           ：Redisの設定
        imagick         ：ImageMagickの設定
（未使用）java18          ：Java 1.8の設定
（未使用）tomcat          ：Tomcatの設定
        letsencrypt     ：Let's Encryptの設定　※自動更新対応
（未使用）httpd           ：Apacheの設定　※Load Balancer対応
（未使用）php-httpd       ：PHP for Apacheの設定
        nginx           ：Nginxの設定　※Load Balancer対応
（未使用）php-nginx       ：PHP for Nginxの設定(PHP-FPM)
（未使用）php71-nginx     ：PHP 7.1 for Nginxの設定(PHP-FPM)
        php72-nginx     ：PHP 7.2 for Nginxの設定(PHP-FPM)
（未使用）squid           ：Squidの設定
    ansible.cfg     ：Ansibleの設定ファイル
    playbook.yml    ：どの設定ルールを使うかを制御する設定　※使用しないルールはコメントアウトしてください
README.md       ：説明や使い方（このファイル）
RELEASES.md     ：リリースノート
Vagrantfile,def ：Vagrantfileのサンプル（開発環境用）
```

## 初期設定

Mac・Linuxターミナル(Windowsはエクスプローラー・エディタ等で操作)

### WordPress設置

下記からダウンロードして、/src/に設置＆/src/に解凍

https://ja.wordpress.org/download/

#### Tips: ソースをGit管理する場合

`/.gitignore` の下記の行をコメントアウト

> /src/wordpress-*.zip
> /src/wordpress-*.tar.gz
> /src/wordpress/

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

Windowsコマンドプロンプト/Mac・Linuxターミナル  
※下記の`~/Downloads/CentOS6.9.box`はダウンロードしたBoxのパスを指定してください。  
```
$ vagrant box add CentOS6 ~/Downloads/CentOS6.9.box
$ vagrant plugin install vagrant-vbguest
$ vagrant up
$ vagrant vbguest
$ vagrant reload
```

SSHターミナル  
※ユーザー名・パスワード・ポートは初期設定の値に変更して実行してください。  
※Mac・Linuxの場合は`vagrant ssh`でも接続可（Windowsは設定すれば接続可）
```
$ ssh admin@127.0.0.1 -p 2206
: abc123
$ su -
: xyz789
# cd /vagrant/ansible
# ansible-playbook playbook.yml -i hosts/development -l development
```
※特定の設定ルール(roles)のみ実行する場合はansible-playbookコマンドでtagsを指定する。例：`-t httpd,php-httpd`

## WordPress設定(例)

### hosts追加

> 192.168.12.206  dev-centos6.local

### ブラウザでアクセスして設定

https://dev-centos6.local/

```
WordPressへようこそ
	[さあ、始めましょう！]
データベース接続
	データベース名: wordpress	<- ansible/hosts/developmentのmysql_dbname
	ユーザー名: wordpress		<- ansible/hosts/developmentのmysql_username
	パスワード: abc123		<- ansible/hosts/developmentのmysql_password
	データベースのホスト名: localhost
	テーブル接頭辞: wp_
	[送信]
この部分のインストールは無事完了
	[インストール実行]
必要情報
	サイトのタイトル: （入力）
	ユーザー名: （入力）
	パスワード: （自動生成のをコピー or 入力）
	メールアドレス: （入力）
	検索エンジンでの表示: □検索エンジンがサイトをインデックスしないようにする
	[WordPressをインストール]
成功しました !
```

### Tips: WordPressマルチサイト設定

src/wordpress/wp-config.php に下記を追加

> define( 'WP_ALLOW_MULTISITE', true );

WordPressサイトネットワークの作成

> WordPress -> [ツール] -> [サイトネットワークの設置]

src/wordpress/wp-config.php に上記で表示された内容を追加  
※Nginxの設定変更は不要  
※サブドメインの場合は、hostsに対象ドメインを追加

---

## サーバー側使用方法(例)

※以降は、サーバー構築時のみ実施

### ansibleユーザー作成・鍵作成（ローカル）

ローカルで実施（初回のみ）
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

### ansibleユーザー作成・鍵設置（各サーバー）

各サーバーで実施（初回のみ）
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

各サーバーで実施（初回のみ）
```
# grep -e "^%wheel\s*ALL=(ALL)\s*ALL$" /etc/sudoers > /dev/null
# echo $?
```
※上記で1と表示されたら下記を実行
```
# cp -a /etc/sudoers /etc/sudoers,`date +"%Y%m%d%H%M%S"`
# echo -e "%wheel\tALL=(ALL)\tALL" >> /etc/sudoers
```

### Playbook実行（ローカル）

ローカルで実施  
※下記はtestの例です。他の環境を使用する場合は、hosts/`test`を変更して実行してください。
```
# su - ansible
$ cd /vagrant/ansible
$ ansible-playbook playbook.yml -i hosts/test -l all --ask-become-pass
SUDO password: ********(ansibleのPW)
Are you sure you want to continue connecting (yes/no)? yes
$ exit
```
※特定のサーバーのみ実施する場合は`all`を`web-servers`等に変えてください。

### Let's Encrypt初期設定（各サーバー）[使用時のみ]

#### シングルドメイン証明書の場合

※インターネットから http://[対象ドメイン]/.well-known/acme-challenge/ にアクセス出来る必要があります。（存在確認の為）

各サーバーで実施（初回のみ）  
※下記のドメイン名・メールアドレスを変更して実行してください。  
※複数のドメインを使用する場合は、certbot-autoの行を複数回実行してください。
```
# mv /etc/letsencrypt /etc/letsencrypt,`date +"%Y%m%d%H%M%S"`
# cd /usr/bin
# curl http://dl.eff.org/certbot-auto -o certbot-auto
# chmod 755 certbot-auto
# unset PYTHON_INSTALL_LAYOUT

Apacheの場合
# certbot-auto certonly --webroot -w /var/www/html -d test.mydomain --email admin@mydomain --agree-tos --debug
Nginxの場合
# certbot-auto certonly --webroot -w /usr/share/nginx/html -d test.mydomain --email admin@mydomain --agree-tos --debug

Is this ok [y/d/N]: y
(Y)es/(N)o: y
IMPORTANT NOTES:
 - Congratulations! Your certificate and chain have been saved at:
```

Apacheの場合
```
# apachectl configtest
Syntax OK
# apachectl graceful
```

Nginxの場合
```
# nginx -t -c /etc/nginx/nginx.conf
nginx: the configuration file /etc/nginx/nginx.conf syntax is ok
nginx: configuration file /etc/nginx/nginx.conf test is successful
# service nginx restart
```

※更新(certbot-auto renew)はバッチ(/etc/cron.weekly/renew_letsencrypt.cron)で定期的に実行されます。

※メール「Please Confirm Your EFF Subscription」が届きます -> URLをクリック

#### マルチドメイン証明書の場合

各サーバーで実施（初回のみ）  
※下記のドメイン名・メールアドレスを変更して実行してください。  
※複数のドメインを使用する場合は、certbot-autoの行を複数回実行してください。
```
# mv /etc/letsencrypt /etc/letsencrypt,`date +"%Y%m%d%H%M%S"`
# cd /usr/bin
# curl http://dl.eff.org/certbot-auto -o certbot-auto
# chmod 755 certbot-auto
# unset PYTHON_INSTALL_LAYOUT
# certbot-auto certonly --manual -d test.mydomain -d *.test.mydomain --email admin@mydomain --agree-tos --manual-public-ip-logging-ok --preferred-challenges dns-01 --debug
Is this ok [y/d/N]: y
(Y)es/(N)o: y
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Please deploy a DNS TXT record under the name
_acme-challenge.test.mydomain with the following value:

＜〜〜〜 文字列1 〜〜〜＞

Before continuing, verify the record is deployed.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
```

表示されたTXTレコードをDNSサーバーに登録して、反映されてから[Enter]を押してください。  
> $ nslookup -type=txt _acme-challenge.test.mydomain 8.8.8.8
> _acme-challenge.test.mydomain	text = "＜〜〜〜 文字列1 〜〜〜＞"

```
Press Enter to Continue

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Please deploy a DNS TXT record under the name
_acme-challenge.test.mydomain with the following value:

＜〜〜〜 文字列2 〜〜〜＞

Before continuing, verify the record is deployed.
(This must be set up in addition to the previous challenges; do not remove,
replace, or undo the previous challenge tasks yet. Note that you might be
asked to create multiple distinct TXT records with the same name. This is
permitted by DNS standards.)
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
```

表示されたTXTレコードをDNSサーバーに`追加`登録して、反映されてから[Enter]を押してください。  
> $ nslookup -type=txt _acme-challenge.test.mydomain 8.8.8.8
> _acme-challenge.test.mydomain	text = "＜〜〜〜 文字列1 〜〜〜＞"
> _acme-challenge.test.mydomain	text = "＜〜〜〜 文字列2 〜〜〜＞"

```
Press Enter to Continue

IMPORTANT NOTES:
 - Congratulations! Your certificate and chain have been saved at:
```

Apacheの場合
```
# apachectl configtest
Syntax OK
# apachectl graceful
```

Nginxの場合
```
# nginx -t -c /etc/nginx/nginx.conf
nginx: the configuration file /etc/nginx/nginx.conf syntax is ok
nginx: configuration file /etc/nginx/nginx.conf test is successful
# service nginx restart
```

※更新(certbot-auto renew)はバッチ(/etc/cron.weekly/renew_letsencrypt.cron)で定期的に実行されます。

※メール「Please Confirm Your EFF Subscription」が届きます -> URLをクリック

### WordPress設置(例)

```
# cd /usr/local/src
# wget https://ja.wordpress.org/latest-ja.zip
# unzip latest-ja.zip 
# chown -R nginx:nginx wordpress

# cd /usr/share/nginx
# mv wordpress wordpress,`date +"%Y%m%d%H%M%S"`
# mv /usr/local/src/wordpress ./
```
