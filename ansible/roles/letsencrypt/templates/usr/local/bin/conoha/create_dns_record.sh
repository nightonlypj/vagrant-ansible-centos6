#!/bin/sh

BATCH_NAME='create_conoha_dns'
LOG_FILE="/var/log/$BATCH_NAME.log"
IDENTITY_SERVICE='{{ conoha_identity_service }}'
DNS_SERVICE='{{ conoha_dns_service }}'
USERNAME='{{ conoha_api_username }}'
PASSWORD='{{ conoha_api_password }}'
TENANTID='{{ conoha_tenant_id }}'

[ -z "$CERTBOT_DOMAIN" ] && TARGET_DOMAIN=$1 || TARGET_DOMAIN=$CERTBOT_DOMAIN
[ -z "$CERTBOT_VALIDATION" ] && TARGET_DATA=$2 || TARGET_DATA=$CERTBOT_VALIDATION
[ -z "$3" ] && TARGET_NAME='_acme-challenge' || TARGET_NAME=$3
[ -z "$4" ] && TARGET_TYPE='TXT' || TARGET_TYPE=$4

error_msg=''
write_log() {
	echo -e "`date +"%Y/%m/%d %H:%M:%S"` ($$) [$1] $2" >> $LOG_FILE
	[ $1 = 'ERROR' ] && error_msg="$error_msg[$1] $2\n"
}

send_error_mail() {
	[ -z "$error_msg" ] && return
	echo -e "$error_msg" | mail -s "[WARNING]$BATCH_NAME report for `hostname`" -r crond warning
	write_log 'INFO' 'Send error mail'
}

create_dns() {
	# トークン発行 - Identity API v2.0
	curl $IDENTITY_SERVICE/tokens -X POST -H "Accept: application/json" \
		-d '{ "auth": { "passwordCredentials": { "username": "'$USERNAME'", "password": "'$PASSWORD'" }, "tenantId": "'$TENANTID'" } }' > $1 2> /dev/null
	if [ $? -ne 0 ]; then
		write_log 'ERROR' "POST $IDENTITY_SERVICE/tokens"
		return
	fi

	token_id=`cat $1 | jq -r ".access.token.id"`
	if [ -z "$token_id" ]; then
		write_log 'ERROR' "Not found access.token.id: `cat $1`"
		return
	fi
	write_log 'INFO' "access.token.id: $token_id"

	# ドメイン一覧表示 - DNS API v1.0
	curl $DNS_SERVICE/v1/domains -X GET -H "Accept: application/json" -H "Content-Type: application/json" -H "X-Auth-Token: $token_id" > $1 2> /dev/null
	if [ $? -ne 0 ]; then
		write_log 'ERROR' "GET $DNS_SERVICE/v1/domains"
		return
	fi

	domain_id=`cat $1 | jq -r '.domains[] | select(.name == "'$TARGET_DOMAIN'.") | .id'`
	if [ -z "$domain_id" ]; then
		write_log 'ERROR' "Not found domains.id: `cat $1`"
		return
	fi
	write_log 'INFO' "domains.id: $domain_id"

	# レコード作成 - DNS API v1.0
	curl $DNS_SERVICE/v1/domains/$domain_id/records -X POST -H "Accept: application/json" -H "Content-Type: application/json" -H "X-Auth-Token: $token_id" \
		-d '{ "name": "'$TARGET_NAME'.'$TARGET_DOMAIN'.", "type": "'$TARGET_TYPE'", "data": "'$TARGET_DATA'" }' > $1 2> /dev/null
	if [ $? -ne 0 ]; then
		write_log 'ERROR' "POST $DNS_SERVICE/v1/domains/$domain_id/records"
		return
	fi

	id=`cat $1 | jq -r ".id"`
	if [ -z "$id" ] || [ "$id" = 'null' ]; then
		write_log 'ERROR' "Not found id: `cat $1`"
		return
	fi
	write_log 'INFO' "OK id: $id"
}

write_log 'INFO' '=== START ==='
write_log 'INFO' "TARGET_DOMAIN: $TARGET_DOMAIN, TARGET_DATA: $TARGET_DATA, TARGET_NAME: $TARGET_NAME, TARGET_TYPE: $TARGET_TYPE"
tempfile=$(mktemp)
create_dns $tempfile
rm -f $tempfile
send_error_mail
write_log 'INFO' '=== END ==='
exit 0
