# CentOS6 Vagrantfile＋Ansible playbook提供

Amazon Linux(EC2/Lightsail)対応

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
        mysql57         ：MySQL 5.7の設定
        postgresql      ：PostgreSQLの設定
        redis           ：Redisの設定
        imagick         ：ImageMagickの設定
        letsencrypt     ：Let's Encryptの設定　※自動更新対応
        httpd           ：Apacheの設定　※Load Balancer対応
        php-httpd       ：PHP for Apacheの設定
        nginx           ：Nginxの設定　※Load Balancer対応
        php-nginx       ：PHP for Nginxの設定(PHP-FPM)
    ansible.cfg     ：Ansibleの設定ファイル
    playbook.yml    ：どの設定ルールを使うかを制御する設定　※使用しないルールはコメントアウトしてください
README.md       ：説明や使い方（このファイル）
RELEASES.md     ：リリースノート
Vagrantfile,def ：Vagrantfileのサンプル（開発環境用）
```

## 初期設定

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
※Let's Encryptを使用する場合は、`ansible/roles/httpd/templates/etc/letsencrypt/live/test.mydomain`をホスト名にコピーまたはリネームしてください。

## development使用方法(例)

Windowsコマンドプロンプト/Mac・Linuxターミナル  
※最新のBoxのURLは、[CentOS6 Vagrant Box提供(VirtualBox向け)](../vagrant-box-centos6)を参照してください。  
```
$ vagrant box add CentOS6 https://github.com/nightonlypj/vagrant-box-centos6/CentOS6.9.box
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

---

## サーバー側使用方法(例)

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
$ ssh-keygen -t rsa
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
$ ansible-playbook playbook.yml -i hosts/test -l all --ask-sudo-pass
SUDO password: ********(ansibleのPW)
Are you sure you want to continue connecting (yes/no)? yes
$ exit
```
※特定のサーバーのみ実施する場合は`all`を`web-servers`等に変えてください。

### Let's Encrypt初期設定（各サーバー）[使用時のみ]

※インターネットから http://[対象ドメイン]/.well-known/acme-challenge/ にアクセス出来る必要があります。（存在確認の為）

各サーバーで実施（初回のみ）  
※下記のドメイン名・メールアドレスを変更して実行してください。  
※複数のドメインを使用する場合は、certbot-autoの行を複数回実行してください。
```
# mv /etc/letsencrypt /etc/letsencrypt,`date +"%Y%m%d%H%M%S"`
# cd /usr/bin
# curl https://dl.eff.org/certbot-auto -o certbot-auto
# chmod 755 /usr/bin/certbot-auto
# unset PYTHON_INSTALL_LAYOUT
# certbot-auto certonly --webroot -w /var/www/html -d test.mydomain --email admin@mydomain --agree-tos --debug
Is this ok [y/d/N]: y
(Y)es/(N)o: y
IMPORTANT NOTES:
 - Congratulations! Your certificate and chain have been saved at:
# apachectl configtest
Syntax OK
# apachectl graceful
```
※更新(certbot-auto renew)はバッチ(/etc/cron.weekly/renew_letsencrypt.cron)で定期的に実行されます。

※メール「Please Confirm Your EFF Subscription」が届きます -> URLをクリック
