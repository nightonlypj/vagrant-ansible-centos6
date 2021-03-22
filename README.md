# CentOS6 Vagrantfile＋Ansible playbook提供（Railsアプリ対応）

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
（未使用）mysql           ：MySQLの設定
        mysql56         ：MySQL 5.6の設定
（未使用）mysql57         ：MySQL 5.7の設定
（未使用）postgresql      ：PostgreSQLの設定
（未使用）postgresql96    ：PostgreSQL 9.6の設定
（未使用）redis           ：Redisの設定
（未使用）mongodb         ：MongoDBの設定
        imagick         ：ImageMagickの設定
（未使用）java18          ：Java 1.8の設定
（未使用）tomcat          ：Tomcatの設定
        letsencrypt     ：Let's Encryptの設定　※自動更新対応
（未使用）httpd           ：Apacheの設定　※Load Balancer対応
（未使用）php-httpd       ：PHP for Apacheの設定
        nginx           ：Nginxの設定　※Load Balancer対応
（未使用）php-nginx       ：PHP for Nginxの設定(PHP-FPM)
（未使用）php71-nginx     ：PHP 7.1 for Nginxの設定(PHP-FPM)
（未使用）php72-nginx     ：PHP 7.2 for Nginxの設定(PHP-FPM)
（未使用）squid           ：Squidの設定
    ansible.cfg     ：Ansibleの設定ファイル
    playbook.yml    ：どの設定ルールを使うかを制御する設定　※使用しないルールはコメントアウトしてください
README.md       ：説明や使い方（このファイル）
RELEASES.md     ：リリースノート
Vagrantfile,def ：Vagrantfileのサンプル（開発環境用）
```

## 環境構築手順

Mac・Linuxターミナル(Windowsはエクスプローラー・エディタ等で操作)

### Railsアプリ設置

このリポジトリと同じ場所にRailsアプリをclone

※異なる場所に設置する場合は、以降の設定で下記を変更してください。  
Vagrantfile  
>  config.vm.synced_folder "../rails-app-origin", "/mnt/rails-app-origin", mount_options: ['dmode=777', 'fmode=777']

※DBの接続情報(config/database.yml)を変更する場合は、以降の設定で下記を変更してください。  
ansible/hosts/development  
> mysql_dbname=rails_app_development  
> mysql_username=rails_app  
> mysql_password=abc123  
> mysql_test_dbname=rails_app_test  
> mysql_test_username=rails_app_test  
> mysql_test_password=abc123

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
>   config.vm.hostname = "`localhost.local`"

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
```
> default: エラー: Cannot find a valid baseurl for repo: base
> → https://qiita.com/imunew/items/3810a41960f40db85c94
> default: Error getting repository data for epel, repository not found
> → https://qiita.com/maruware/items/eb659266a45021cf486c
```
$ vagrant ssh
$ sudo -s
# sed -i -e "s/^mirrorlist=http:\/\/mirrorlist.centos.org/#mirrorlist=http:\/\/mirrorlist.centos.org/g" /etc/yum.repos.d/CentOS-Base.repo
# sed -i -e "s/^#baseurl=http:\/\/mirror.centos.org/baseurl=http:\/\/vault.centos.org/g" /etc/yum.repos.d/CentOS-Base.repo
# yum -y install epel-release
# sed -i "s/mirrorlist=https/mirrorlist=http/" /etc/yum.repos.d/epel.repo
# yum -y --enablerepo=epel install ansible
# exit
$ exit
```
```
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
> Error: Package: 2:nodejs-12.16.1-1nodesource.x86_64 (nodesource)
> → https://www.saintsouth.net/blog/update-libstdcpp-on-centos6/
```
# yum -y install gmp-devel mpfr-devel libmpc-devel glibc-devel.i686
# cd /usr/local/src
# curl -LO http://ftp.tsukuba.wide.ad.jp/software/gcc/releases/gcc-4.8.4/gcc-4.8.4.tar.gz
curl: (7) Failed to connect to 2001:200:0:7c06::9393: ネットワークに届きません
→ 未解決
```
※特定の設定ルール(roles)のみ実行する場合はansible-playbookコマンドでtagsを指定する。例：`-t httpd,php-httpd`

### Rubyインストール

```
# su - rails-app
$ gpg2 --keyserver hkp://pool.sks-keyservers.net --recv-keys 409B6B1796C275462A1703113804BB82D39DC0E3 7D2BAF1CF37B13E2069D6956105BD0E739499BDB
gpg:           インポート: 2  (RSA: 2)

$ 'curl' -sSL https://get.rvm.io | bash -s stable
Donate: https://opencollective.com/rvm/donate

$ source ~/.rvm/scripts/rvm
$ rvm -v
rvm 1.29.12 (latest) by Michal Papis, Piotr Kuczynski, Wayne E. Seguin [https://rvm.io]
※バージョンは異なっても良い

$ rvm install 3.0.0
$ ruby -v
ruby 3.0.0p0 (2020-12-25 revision 95aff21468) [x86_64-linux]
```

### Node.jsインストール

```
$ curl -o- https://raw.githubusercontent.com/creationix/nvm/v0.33.11/install.sh | bash
```
> => Unknown option: -c
> Failed to clone nvm repo. Please report this!
> → https://teratail.com/questions/140492
> → https://www.torutk.com/projects/swe/wiki/CentOS_6%E3%81%A7git%E3%81%AE%E3%83%90%E3%83%BC%E3%82%B8%E3%83%A7%E3%83%B3%E3%82%92%E4%B8%8A%E3%81%92%E3%82%8B
```
$ exit
# vi /etc/yum.repos.d/wandisco.repo
---- ここから ----
[wandisco-git]
name=WANdisco Distribution of git
baseurl=http://opensource.wandisco.com/centos/6/git/$basearch
enabled=0
gpgcheck=1
gpgkey=file:///etc/pki/rpm-gpg/RPM-GPG-KEY-WANdisco
---- ここまで ----
# rpm --import http://opensource.wandisco.com/RPM-GPG-KEY-WANdisco
# yum update git --enablerepo=wandisco-git
これでいいですか? [y/N]y
# git --version
git version 2.22.0
※バージョンは異なっても良い
# su - rails-app
$ curl -o- https://raw.githubusercontent.com/creationix/nvm/v0.33.11/install.sh | bash
```
```
$ source ~/.bashrc
$ nvm --version
0.33.11
※バージョンは異なっても良い
```
```
$ nvm ls-remote | grep 'Latest LTS'
       v14.15.5   (Latest LTS: Fermium)
$ nvm install v14.15.5
※バージョンは異なっても良いが、本番と同じが理想
```
> node: /usr/lib64/libstdc++.so.6: version `GLIBCXX_3.4.14' not found (required by node)
> node: /usr/lib64/libstdc++.so.6: version `GLIBCXX_3.4.18' not found (required by node)
> node: /usr/lib64/libstdc++.so.6: version `CXXABI_1.3.5' not found (required by node)
> node: /usr/lib64/libstdc++.so.6: version `GLIBCXX_3.4.15' not found (required by node)
> node: /lib64/libc.so.6: version `GLIBC_2.16' not found (required by node)
> node: /lib64/libc.so.6: version `GLIBC_2.17' not found (required by node)
> node: /lib64/libc.so.6: version `GLIBC_2.14' not found (required by node)
→ 未解決
```
$ node -v
v14.15.5
```

### Railsアプリ起動

```
$ cd rails-app-origin
$ cp -a config/settings/development.yml,dev config/settings/development.yml

$ bundle install
```
> client.c:911: error: ‘MYSQL_DEFAULT_AUTH’ undeclared (first use in this function)
> client.c:911: error: (Each undeclared identifier is reported only once
> client.c:911: error: for each function it appears in.)
> client.c: In function ‘set_default_auth’:
> client.c:1350: error: ‘MYSQL_DEFAULT_AUTH’ undeclared (first use in this function)
> → https://www.366service.com/jp/qa/3b5494d33fea4a5aec489adc87bb5894
```
$ vi Gemfile
---- ここから ----
# gem 'mysql2', '>= 0.4.4'
gem 'mysql2', '~> 0.4.10'
---- ここまで ----
$ bundle install
```
> cc1plus: error: unrecognized command line option "-std=c++11"
> → https://teratail.com/questions/260766
> → http://c-loft.com/blog/?p=2410
```
$ exit
# cd /etc/yum.repos.d
# wget http://people.centos.org/tru/devtools-2/devtools-2.repo
# yum -y install devtoolset-2-gcc devtoolset-2-binutils devtoolset-2-gcc-c++
# su - rails-app
$ scl enable devtoolset-2 bash
$ gcc -v
gcc version 4.8.2 20140120 (Red Hat 4.8.2-15) (GCC) 
$ cd rails-app-origin
$ bundle install
Bundle complete!

$ yarn
（$ rails webpacker:install）
> sh: node: コマンドが見つかりません
> sh: nodejs: コマンドが見つかりません
→ 未解決
Done

$ rails db:migrate
※「Mysql2::Error: Specified key was too long; max key length is 767 bytes」の場合は「rails db:migrate:reset」で回避

$ rails db:seed
$ rails s
```

PCのhostsに下記を追加  
※IPは、VMのIPを指定（Vagrantfileで設定した値）
```
$ sudo vi /etc/hosts
192.168.12.206   localhost.local
```

- http://localhost.local
  - メールアドレスとパスワードは、`db/seed/development/users.yml`参照
- http://localhost.local/admin
  - メールアドレスとパスワードは、`db/seed/admin_users.yml`参照

※この接続ではプライバシーが保護されません [詳細設定] -> [localhost.local にアクセスする（安全ではありません）]

### Tips: hostsを使わないようにしたい場合

※サブドメインを利用するアプリの場合、hostsにワイルドカードが使えない為、毎回設定を追加する必要がある。

Railsアプリのconfig/settings/development.yml  
※IPは、VMのIPを指定（Vagrantfileで設定した値）  
> base_domain: '192.168.12.206.nip.io'  
> cookie_domain: '192.168.12.206.nip.io'

ansible/hosts/developmentを変更後、ansible-playbookコマンドを実行    
> httpd_front_servername='192.168.12.206.nip.io'	# Tips: Railsアプリのドメイン(config/settings/development.ymlのbase_domain)と同じにする

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

---

## デプロイ（各サーバー）

### Rubyインストール

```
# su - rails-app
$ gpg2 --keyserver hkp://pool.sks-keyservers.net --recv-keys 409B6B1796C275462A1703113804BB82D39DC0E3 7D2BAF1CF37B13E2069D6956105BD0E739499BDB
gpg:           インポート: 2  (RSA: 2)

$ 'curl' -sSL https://get.rvm.io | bash -s stable
Donate: https://opencollective.com/rvm/donate

$ source ~/.rvm/scripts/rvm
$ rvm -v
rvm 1.29.12 (latest) by Michal Papis, Piotr Kuczynski, Wayne E. Seguin [https://rvm.io]
※バージョンは異なっても良い

$ rvm install 3.0.0
$ ruby -v
ruby 3.0.0p0 (2020-12-25 revision 95aff21468) [x86_64-linux]
```

### Node.jsインストール

```
$ curl -o- https://raw.githubusercontent.com/creationix/nvm/v0.33.11/install.sh | bash
$ source ~/.bashrc
$ nvm --version
0.33.11
※バージョンは異なっても良い
```
```
$ nvm install v14.15.5
※バージョンは異なっても良いが、開発環境と同じが理想

$ node -v
v14.15.5
```

### Railsアプリ起動

```
$ mkdir .ssh
$ chmod 700 .ssh
$ vi .ssh/id_rsa
※Git用の鍵を貼り付け
$ chmod 600 .ssh/id_rsa

$ git clone git@ssh.dev.azure.com:v3/nightonly/rails-app-origin/rails-app-origin
Are you sure you want to continue connecting (yes/no)? yes
$ cd rails-app-origin
$ git checkout develop
$ git branch
* develop
  master

$ mkdir -p tmp/{pids,sockets}
$ cd config/settings
本番> $ ln -s production.yml,prod production.yml
ステージング> $ ln -s production.yml,stg production.yml
テスト> $ ln -s production.yml,test production.yml
$ cd ../../public
本番> $ ln -s robots.txt,allow robots.txt
その他> $ ln -s robots.txt,disallow robots.txt
$ cd ..

$ bundle install --without test development
Bundle complete!

$ yarn
Done

$ rails secret
※出力内容を下記(SECRET_KEY_BASE)に入れてメモ。環境毎に一意の値
$ vi ~/.bashrc
---- ここから ----
### START ###
export RAILS_ENV=production
export SECRET_KEY_BASE=979420d38cd4781a5a36f7596d9ea220a7abbe749df136d5f4ae3c122aec1ff3ee1245b0289e6aff1e037000ed5fb52756295c395bcb07afde651d52bc0c933b
export DATABASE_URL=mysql2://rails_app:changepasswd@localhost/rails_app_test
### END ###
---- ここまで ----
$ source ~/.bashrc

$ rails db:migrate
※「Mysql2::Error: Specified key was too long; max key length is 767 bytes」の場合は「rails db:migrate:reset」で回避

$ rails db:seed
$ rails assets:precompile
$ rails unicorn:start
```

- http://mydomain
- http://stg.mydomain
- http://test.mydomain
