#!/usr/bin/env bash
set -e

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$DIR"

echo "=== Generating Sentinel Nexus Mutual TLS (mTLS) Infrastructure ==="

# 1. Root Certificate Authority (CA)
openssl genrsa -out ca.key 4096
openssl req -x509 -new -nodes -key ca.key -sha256 -days 3650 -out ca.crt \
    -subj "/C=US/ST=Defense/L=HQ/O=Sentinel-Grid/CN=Sentinel-Root-CA"

# 2. Server Certificate (Nexus Command Plane)
openssl genrsa -out server.key 2048
openssl req -new -key server.key -out server.csr \
    -subj "/C=US/ST=Defense/L=HQ/O=Sentinel-Grid/CN=nexus.local"

cat > server.ext <<EOF
authorityKeyIdentifier=keyid,issuer
basicConstraints=CA:FALSE
keyUsage = digitalSignature, nonRepudiation, keyEncipherment, dataEncipherment
subjectAltName = @alt_names

[alt_names]
DNS.1 = localhost
DNS.2 = nexus.local
IP.1 = 127.0.0.1
IP.2 = 0.0.0.0
EOF

openssl x509 -req -in server.csr -CA ca.crt -CAkey ca.key -CAcreateserial \
    -out server.crt -days 1825 -sha256 -extfile server.ext

# 3. Client Certificate (Appliance Test Certificate)
openssl genrsa -out appliance_client.key 2048
openssl req -new -key appliance_client.key -out appliance_client.csr \
    -subj "/C=US/ST=Defense/L=Edge/O=Sentinel-Edge/CN=appliance-test-node"

openssl x509 -req -in appliance_client.csr -CA ca.crt -CAkey ca.key -CAcreateserial \
    -out appliance_client.crt -days 1825 -sha256

rm -f *.csr *.ext *.srl

echo "[+] mTLS Certificates successfully generated in configs/certs/"
echo "    - Root CA: ca.crt"
echo "    - Server Cert: server.crt / server.key"
echo "    - Appliance Cert: appliance_client.crt / appliance_client.key"