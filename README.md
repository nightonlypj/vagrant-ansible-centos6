# CentOS6 Vagrantfile提供

## 前提条件

下記がインストールされている事  
- Vagrant ( https://www.vagrantup.com/downloads.html )
- VirtualBox ( https://www.virtualbox.org/wiki/Downloads )

※VMwareでの動作は未確認です。
※古いバージョンでは動かない場合があります。

作成時に使用したバージョン  
- Vagrant 1.9.3 (Windows 64-bit)
- VirtualBox 5.1.22 (Windows)

## 初期設定

Mac・Linuxターミナル(Windowsはエクスプローラー・エディタ等で操作)  
```
$ cp Vagrantfile,def Vagrantfile
$ vi Vagrantfile
```

SSH接続ポート変更(管理用)：使用してないポートを指定  
>  config.vm.network "forwarded_port", guest: 22, host: `2206`, host_ip: "127.0.0.1", id: "ssh"

IPアドレス変更(HTTP/S等の接続用)：使用してないIPを指定  
>  config.vm.network "public_network", ip: "`192.168.12.206`"

※複数のNICに繋がっている場合はブリッジアダプターを設定しておくと便利  
>  config.vm.network "public_network", ip: "`192.168.12.206`"`, bridge: "en1: Wi-Fi (AirPort)"`

ホスト名変更  
>   config.vm.hostname = "`dev-centos6.local`"

SSHユーザー名・パスワード変更、rootパスワード変更  
>    useradd -g wheel `admin`  
>    echo "`admin`:`abc123`" | chpasswd  
>    echo "root:`xyz789`" | chpasswd

## 使用方法(例)

Windowsコマンドプロンプト/Mac・Linuxターミナル  
※最新のBoxのURLは、[CentOS6 Vagrant Box提供(VirtualBox向け)](../vagrant-box-centos6)を参照してください。  
```
$ vagrant box add CentOS6 https://github.com/nightonlypj/vagrant-box-centos6/CentOS6.9.box
$ vagrant up
```

SSHターミナル  
※ユーザー名・パスワード・ポートは初期設定の値に変更してください。  
```
$ ssh admin@127.0.0.1 -p 2206
: abc123
$ su -
: xyz789
```
